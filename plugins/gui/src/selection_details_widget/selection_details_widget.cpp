#include "gui/selection_details_widget/selection_details_widget.h"
#include "gui/selection_details_widget/tree_navigation/selection_tree_view.h"
#include "gui/selection_details_widget/tree_navigation/selection_tree_proxy.h"
#include "gui/module_dialog/module_dialog.h"
#include "gui/grouping_dialog/grouping_dialog.h"
#include "gui/grouping/grouping_manager_widget.h"
#include "gui/graph_tab_widget/graph_tab_widget.h"
#include "gui/user_action/action_add_items_to_object.h"
#include "gui/user_action/action_create_object.h"
#include "gui/user_action/action_remove_items_from_object.h"
#include "gui/user_action/action_set_selection_focus.h"
#include "gui/user_action/user_action_compound.h"
#include "gui/user_action/user_action_object.h"
#include "gui/settings/settings_items/settings_item_checkbox.h"
#include "gui/selection_details_widget/gate_details_tab_widget.h"
#include "gui/selection_details_widget/net_details_tab_widget.h"
#include "gui/selection_details_widget/module_details_tab_widget.h"

#include "gui/gui_def.h"

#include "gui/gui_globals.h"
#include "hal_core/netlist/gate.h"
#include "hal_core/netlist/net.h"
#include "hal_core/netlist/netlist.h"
#include "gui/searchbar/searchbar.h"
#include "gui/toolbar/toolbar.h"
#include "gui/gui_utils/graphics.h"
#include "gui/gui_utils/netlist.h"

#include <QHeaderView>
#include <QInputDialog>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QShortcut>
#include <QSplitter>
#include <QListWidget>
#include <QLineEdit>
#include <QMenu>
#include <QAction>


namespace hal
{
    const QString SelectionDetailsWidget::sAddToGrouping("Add to grouping ");

    SettingsItemCheckbox* SelectionDetailsWidget::sSettingHideEmpty =
            new SettingsItemCheckbox(
                "Hide Empty Section",
                "selection_details/hide_empty_sections",
                false,
                "Appearance:Selection Details",
                "Specifies wheter empty sections are hidden or shown in the Selection Details Widget."
                );

    SelectionDetailsWidget::SelectionDetailsWidget(QWidget* parent)
        : ContentWidget("Selection Details", parent), mNumberSelectedItems(0),
          mSelectionToGrouping(new QAction),
          mSelectionToModule(new QAction)
    {
        //needed to load the properties
        ensurePolished();

        mSplitter = new QSplitter(Qt::Horizontal, this);
        //mSplitter->setStretchFactor(0,5); /* Doesn't do anything? */
        //mSplitter->setStretchFactor(1,10);

        //container for left side of splitter containing a selection tree view and a searchbar
        QWidget* treeViewContainer = new QWidget(mSplitter);
        //treeViewContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); /*Does not work, but should?*/
        treeViewContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); /*Kinda works? Does not give as much space as previous implementation without container.*/

        QVBoxLayout* containerLayout = new QVBoxLayout(treeViewContainer);





        mSelectionTreeView  = new SelectionTreeView(treeViewContainer);

        mModuleModel = new ModuleModel(this);
        mSelectionTreeProxyModel = new SelectionTreeProxyModel(this);
        mSelectionTreeProxyModel->setSourceModel(mModuleModel);
        mSelectionTreeView->setModel(mSelectionTreeProxyModel);

        //mSelectionTreeProxyModel->setSourceModel(mSelectionTreeView->model());
        //mSelectionTreeView->setModel(mSelectionTreeProxyModel);

        mSelectionTreeView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        mSelectionTreeView->setMinimumWidth(280);
        mSelectionTreeView->hide();

        mSearchbar = new Searchbar(treeViewContainer);
        mSearchbar->hide();
        mSearchbar->setColumnNames(mSelectionTreeProxyModel->getColumnNames());

        containerLayout->addWidget(mSelectionTreeView);
        containerLayout->addWidget(mSearchbar);
        containerLayout->setSpacing(0);
        containerLayout->setContentsMargins(0,0,0,0);

        mStackedWidget = new QStackedWidget(mSplitter);

        mGateDetailsTabs = new GateDetailsTabWidget(mStackedWidget);
        mStackedWidget->addWidget(mGateDetailsTabs);

        mNetDetailsTabs = new NetDetailsTabWidget(mStackedWidget);
        mStackedWidget->addWidget(mNetDetailsTabs);

        mModuleDetailsTabs = new ModuleDetailsTabWidget();
        mStackedWidget->addWidget(mModuleDetailsTabs);

        mItemDeletedLabel = new QLabel(mStackedWidget);
        mItemDeletedLabel->setText("Currently selected item has been removed. Please consider relayouting the Graph.");
        mItemDeletedLabel->setWordWrap(true);
        mItemDeletedLabel->setAlignment(Qt::AlignmentFlag::AlignTop);
        mStackedWidget->addWidget(mItemDeletedLabel);

        mNoSelectionLabel = new QLabel(mStackedWidget);
        mNoSelectionLabel->setText("No Selection");
        mNoSelectionLabel->setWordWrap(true);
        mNoSelectionLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);
        mStackedWidget->addWidget(mNoSelectionLabel);

        mStackedWidget->setCurrentWidget(mNoSelectionLabel);

        mContentLayout->addWidget(mSplitter);

        //    m_table_widget->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        //    m_table_widget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        //    m_table_widget->setSelectionBehavior(QAbstractItemView::SelectRows);
        //    m_table_widget->setSelectionMode(QAbstractItemView::NoSelection);
        //    m_table_widget->setShowGrid(false);
        //    m_table_widget->setAlternatingRowColors(true);
        //    m_table_widget->horizontalHeader()->setStretchLastSection(true);
        //    m_table_widget->viewport()->setFocusPolicy(Qt::NoFocus);
        mSelectionToGrouping->setToolTip("Assign to grouping");
        mSelectionToModule->setToolTip("Move to module");
        mSelectionToGrouping->setIcon(gui_utility::getStyledSvgIcon(mDisabledIconStyle, mToGroupingIconPath));
        mSelectionToModule->setIcon(gui_utility::getStyledSvgIcon(mDisabledIconStyle, mToModuleIconPath));
        canMoveToModule(0);

        mSearchAction->setToolTip("Search");
        enableSearchbar(false);  // enable upon first non-zero selection
        mSelectionToGrouping->setDisabled(true);
        mSelectionToModule->setDisabled(true);

        gSelectionRelay->registerSender(this, "SelectionDetailsWidget");
        connect(mSelectionToGrouping, &QAction::triggered, this, &SelectionDetailsWidget::selectionToGrouping);
        connect(mSelectionToModule, &QAction::triggered, gNetlistRelay, &NetlistRelay::addToModuleDialog);
        connect(mSearchAction, &QAction::triggered, this, &SelectionDetailsWidget::toggleSearchbar);
        connect(mSelectionTreeView, &SelectionTreeView::triggerSelection, this, &SelectionDetailsWidget::handleTreeSelection);
        connect(gSelectionRelay, &SelectionRelay::selectionChanged, this, &SelectionDetailsWidget::handleSelectionUpdate);

        connect(mSearchbar, &Searchbar::triggerNewSearch, this, &SelectionDetailsWidget::updateSearchIcon);
        connect(mSearchbar, &Searchbar::triggerNewSearch, mSelectionTreeProxyModel, &SelectionTreeProxyModel::startSearch);
    }

    void SelectionDetailsWidget::selectionToGrouping()
    {
        GroupingDialog gd(this);
        if (gd.exec() != QDialog::Accepted) return;
        if (gd.isNewGrouping())
        {
            selectionToGroupingAction();
            return;
        }
        QString groupName = QString::fromStdString(gNetlist->get_grouping_by_id(gd.groupId())->get_name());
        selectionToGroupingAction(groupName);
    }

    UserAction* SelectionDetailsWidget::groupingUnassignActionFactory(const UserActionObject& obj) const
    {
        Grouping* assignedGrouping = nullptr;
        QSet<u32> mods, gats, nets;
        Module* mod;
        Gate*   gat;
        Net*    net;
        switch(obj.type()) {
        case UserActionObjectType::Module:
            mod = gNetlist->get_module_by_id(obj.id());
            if (mod) assignedGrouping = mod->get_grouping();
            mods.insert(mod->get_id());
            break;
        case UserActionObjectType::Gate:
            gat = gNetlist->get_gate_by_id(obj.id());
            if (gat) assignedGrouping = gat->get_grouping();
            gats.insert(gat->get_id());
            break;
        case UserActionObjectType::Net:
            net = gNetlist->get_net_by_id(obj.id());
            if (net) assignedGrouping = net->get_grouping();
            nets.insert(net->get_id());
            break;
        default:
            break;
        }
        if (!assignedGrouping) return nullptr; // nothing to do
        ActionRemoveItemsFromObject* retval = new ActionRemoveItemsFromObject(mods,gats,nets);
        retval->setObject(UserActionObject(assignedGrouping->get_id(),
                                           UserActionObjectType::Grouping));
        retval->setObjectLock(true);
        return retval;
    }

    void SelectionDetailsWidget::selectionToGroupingAction(const QString& existingGrpName)
    {
        UserActionCompound* compound = new UserActionCompound;
        u32 grpId = 0;
        if (existingGrpName.isEmpty())
        {
            compound->addAction(new ActionCreateObject(UserActionObjectType::Grouping));
            compound->setUseCreatedObject();
        }
        else
        {
            Grouping* grp = gContentManager->getGroupingManagerWidget()->
                    getModel()->groupingByName(existingGrpName);
            if (grp) grpId = grp->get_id();
        }
        for (const UserActionObject& obj : gSelectionRelay->toUserActionObject())
        {
            UserAction* act = groupingUnassignActionFactory(obj);
            if (act) compound->addAction(act);
        }

        //get selected items from Model
        QSet<u32> mods = {};
        QSet<u32> gates = {};
        QSet<u32> nets = {};
        auto* sourceModel = static_cast<ModuleModel*>(mSelectionTreeProxyModel->sourceModel());

        //check each row for its Itemtype and append the ID to the corresponding QSet {mods, gates, nets}
        for(int i = 0; i < mSelectionTreeProxyModel->rowCount(); i++){

                QModelIndex sourceModelIndex = mSelectionTreeProxyModel->mapToSource(mSelectionTreeProxyModel->index(i,0));
                ModuleItem* item = dynamic_cast<ModuleItem*>(sourceModel->getItemFromIndex(sourceModelIndex));
                switch (item->getType())
                {
                    case ModuleItem::TreeItemType::Module:
                        mods.insert(item->id());
                        break;

                    case ModuleItem::TreeItemType::Gate:
                        gates.insert(item->id());
                        break;

                    case ModuleItem::TreeItemType::Net:
                        nets.insert(item->id());
                        break;
                    default:
                        break;
                }          
        }
        ActionAddItemsToObject* act = new ActionAddItemsToObject(mods,
                                                                 gates,
                                                                 nets);


        act->setObject(UserActionObject(grpId,UserActionObjectType::Grouping));
        compound->addAction(act);
        compound->addAction(new ActionSetSelectionFocus);
        compound->exec();
    }

    void SelectionDetailsWidget::enableSearchbar(bool enable)
    {
        QString iconStyle = enable ? mSearchIconStyle : mDisabledIconStyle;
        mSearchAction->setIcon(gui_utility::getStyledSvgIcon(iconStyle, mSearchIconPath));
        if (!enable && mSearchbar->isVisible())
        {
            mSearchbar->hide();
            setFocus();
        }
        mSearchAction->setEnabled(enable);
    }
    void SelectionDetailsWidget::canMoveToModule(int nodes)
    {
        QString iconStyle = nodes > 0
                ? mToModuleIconStyle
                : mDisabledIconStyle;
        mSelectionToModule->setIcon(gui_utility::getStyledSvgIcon(iconStyle, mToModuleIconPath));
        mSelectionToModule->setEnabled(gContentManager->getGraphTabWidget()->isSelectMode()
                                       && nodes > 0);
    }

    void SelectionDetailsWidget::handleSelectionUpdate(void* sender)
    {
        //called update methods with id = 0 to reset widget to the internal state of not updating because its not mVisible
        //when all details widgets are finished maybe think about more elegant way

        if (sender == this)
        {
            return;
        }
        
        SelectionTreeProxyModel* proxy = static_cast<SelectionTreeProxyModel*>(mSelectionTreeView->model());
        mSelectionTreeView->setModel(mSelectionTreeProxyModel);
        if (proxy->isGraphicsBusy()) return;
        if (!mSearchbar->getCurrentText().isEmpty())
        {
            mSearchbar->clear();
            updateSearchIcon();
        }

        mNumberSelectedItems = gSelectionRelay->numberSelectedItems();
        QVector<const ModuleItem*> defaultHighlight;

        if (mNumberSelectedItems)
        {
            // more than 1 item selected, populate and make mVisible
            mSelectionTreeView->populate(true);
            defaultHighlight.append(mSelectionTreeView->itemFromIndex());

            canMoveToModule(gSelectionRelay->numberSelectedNodes());
            enableSearchbar(true);

            bool toModuleEnabled = gContentManager->getGraphTabWidget()->isSelectMode();
            mSelectionToGrouping->setEnabled(true);
            mSelectionToModule->setEnabled(toModuleEnabled);
            mSelectionToGrouping->setIcon(gui_utility::getStyledSvgIcon(mToGroupingIconStyle, mToGroupingIconPath));
            mSelectionToModule->setIcon(gui_utility::getStyledSvgIcon(toModuleEnabled
                                                                      ? mToModuleIconStyle
                                                                      : mDisabledIconStyle,
                                                                      mToModuleIconPath));
        }
        else
        {
            // nothing selected
            singleSelectionInternal(nullptr);
            // clear and hide tree
            mSelectionTreeView->populate(false);
            canMoveToModule(0);
            enableSearchbar(false);
            mSelectionToGrouping->setDisabled(true);
            mSelectionToModule->setDisabled(true);
            mSelectionToGrouping->setIcon(gui_utility::getStyledSvgIcon(mDisabledIconStyle, mToGroupingIconPath));
            mSelectionToModule->setIcon(gui_utility::getStyledSvgIcon(mDisabledIconStyle, mToModuleIconPath));

            return;
        }


        if (gSelectionRelay->numberSelectedModules())
        {
            ModuleItem sti(gSelectionRelay->selectedModulesList().at(0), ModuleItem::TreeItemType::Module, mModuleModel);
            singleSelectionInternal(&sti);
        }
        else if (gSelectionRelay->numberSelectedGates())
        {
            ModuleItem sti(gSelectionRelay->selectedGatesList().at(0), ModuleItem::TreeItemType::Gate, mModuleModel);
            singleSelectionInternal(&sti);
        }
        else if (gSelectionRelay->numberSelectedNets())
        {
            ModuleItem sti(gSelectionRelay->selectedNetsList().at(0), ModuleItem::TreeItemType::Net, mModuleModel);
            singleSelectionInternal(&sti);
        }

        Q_EMIT triggerHighlight(defaultHighlight);
    }

    void SelectionDetailsWidget::handleTreeSelection(const ModuleItem *sti)
    {
        singleSelectionInternal(sti);
        QVector<const ModuleItem*> highlight;
        if (sti) highlight.append(sti);
        Q_EMIT triggerHighlight(highlight);
    }

    void SelectionDetailsWidget::showNoSelection()
    {
        if(mStackedWidget->currentWidget() == mModuleDetailsTabs)
            mModuleDetailsTabs->clear();
        mStackedWidget->setCurrentWidget(mNoSelectionLabel);
    }

    void SelectionDetailsWidget::singleSelectionInternal(const ModuleItem *sti)
    {
        if(!sti){
            showNoSelection();
            return;
        }

        switch (sti->getType()) {
        case ModuleItem::TreeItemType::Module:
            if (Module* m = gNetlist->get_module_by_id(sti->id()); m)
            {
                mModuleDetailsTabs->setModule(m);
                mStackedWidget->setCurrentWidget(mModuleDetailsTabs);
                //            if (mNumberSelectedItems==1) set_name("Module Details");
            }
            else
                showNoSelection();

            break;

        case ModuleItem::TreeItemType::Gate:
            showNoSelection();
            if (Gate* g = gNetlist->get_gate_by_id(sti->id()); g)
            {
                mGateDetailsTabs->setGate(g);
                mStackedWidget->setCurrentWidget(mGateDetailsTabs);
                //            if (mNumberSelectedItems==1) set_name("Gate Details");
            }
            break;
        case ModuleItem::TreeItemType::Net:
            showNoSelection();
            if (Net* n = gNetlist->get_net_by_id(sti->id()); n)
            {
                mNetDetailsTabs->setNet(n);
                mStackedWidget->setCurrentWidget(mNetDetailsTabs);
                //            if (mNumberSelectedItems==1) set_name("Net Details");
            }
            break;
        default:
            break;
        }
    }

    QList<QShortcut *> SelectionDetailsWidget::createShortcuts()
    {
        mSearchShortcut = new QShortcut(mSearchKeysequence, this);
        connect(mSearchShortcut, &QShortcut::activated, mSearchAction, &QAction::trigger);

        QList<QShortcut*> list;
        list.append(mSearchShortcut);

        return list;
    }

    void SelectionDetailsWidget::toggleSearchbar()
    {
        if (!mSearchAction->isEnabled())
            return;

        if (mSearchbar->isHidden())
        {
            mSearchbar->show();
            mSearchbar->setFocus();
        }
        else
        {
            mSearchbar->hide();
            setFocus();
        }
    }

    void SelectionDetailsWidget::updateSearchIcon()
    {
        if (mSearchbar->filterApplied() && mSearchbar->isVisible())
            mSearchAction->setIcon(gui_utility::getStyledSvgIcon(mSearchActiveIconStyle, mSearchIconPath));
        else
            mSearchAction->setIcon(gui_utility::getStyledSvgIcon(mSearchIconStyle, mSearchIconPath));
    }

    void SelectionDetailsWidget::setupToolbar(Toolbar* toolbar)
    {
        toolbar->addAction(mSelectionToGrouping);
        toolbar->addAction(mSelectionToModule);
        toolbar->addAction(mSearchAction);
    }

    SelectionTreeView* SelectionDetailsWidget::selectionTreeView()
    {
        return mSelectionTreeView;
    }

    QString SelectionDetailsWidget::disabledIconStyle() const
    {
        return mDisabledIconStyle;
    }

    void SelectionDetailsWidget::setDisabledIconStyle(const QString& style)
    {
        mDisabledIconStyle = style;
    }

    QString SelectionDetailsWidget::searchIconPath() const
    {
        return mSearchIconPath;
    }

    QString SelectionDetailsWidget::searchIconStyle() const
    {
        return mSearchIconStyle;
    }

    QString SelectionDetailsWidget::searchActiveIconStyle() const
    {
        return mSearchActiveIconStyle;
    }

    void SelectionDetailsWidget::setSearchIconPath(const QString& path)
    {
        mSearchIconPath = path;
    }

    void SelectionDetailsWidget::setSearchIconStyle(const QString& style)
    {
        mSearchIconStyle = style;
    }

    void SelectionDetailsWidget::setSearchActiveIconStyle(const QString& style)
    {
        mSearchActiveIconStyle = style;
    }

    QString SelectionDetailsWidget::toGroupingIconPath() const
    {
        return mToGroupingIconPath;
    }

    QString SelectionDetailsWidget::toGroupingIconStyle() const
    {
        return mToGroupingIconStyle;
    }

    void SelectionDetailsWidget::setToGroupingIconPath(const QString& path)
    {
        mToGroupingIconPath = path;
    }

    void SelectionDetailsWidget::setToGroupingIconStyle(const QString& style)
    {
        mToGroupingIconStyle = style;
    }
    
    QString SelectionDetailsWidget::toModuleIconPath() const
    {
        return mToModuleIconPath;
    }
    
    QString SelectionDetailsWidget::toModuleIconStyle() const
    {
        return mToModuleIconStyle;
    }
    
    void SelectionDetailsWidget::setToModuleIconPath(const QString& path)
    {
        mToModuleIconPath = path;
    }
    
    void SelectionDetailsWidget::setToModuleIconStyle(const QString& style)
    {
        mToModuleIconStyle = style;
    }
}
