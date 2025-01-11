// MIT License
// 
// Copyright (c) 2019 Ruhr University Bochum, Chair for Embedded Security. All Rights reserved.
// Copyright (c) 2019 Marc Fyrbiak, Sebastian Wallat, Max Hoffmann ("ORIGINAL AUTHORS"). All rights reserved.
// Copyright (c) 2021 Max Planck Institute for Security and Privacy. All Rights reserved.
// Copyright (c) 2021 Jörn Langheinrich, Julian Speith, Nils Albartus, René Walendy, Simon Klix ("ORIGINAL AUTHORS"). All Rights reserved.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "hal_core/defines.h"
#include "gui/content_widget/content_widget.h"
#include "gui/module_model/module_item.h"
#include "gui/searchbar/searchbar.h"
#include "gui/selection_relay/selection_relay.h"
#include "gui/module_widget/module_tree_view.h"
#include "hal_core/netlist/module.h"
#include "gui/settings/settings_items/settings_item_keybind.h"


#include <QAction>
#include <QItemSelection>
#include <QList>
#include <QObject>
#include <QSortFilterProxyModel>

class QTreeView;

namespace hal
{
    class ModuleModel;
    class ModuleProxyModel;

    /**
     * @ingroup utility_widgets-module
     * @brief Shows the modules of the netlist hierarchical in a tree view.
     *
     * By right-clicking a module in the widget the user can do various actions with the module like changing the name and
     * type, move, append or delete the module.
     */
    class ModuleWidget : public ContentWidget
    {
        Q_OBJECT
        Q_PROPERTY(QString disabledIconStyle READ disabledIconStyle WRITE setDisabledIconStyle)
        Q_PROPERTY(QString activeIconStyle READ activeIconStyle WRITE setActiveIconStyle)
        Q_PROPERTY(QString showNetsIconPath READ showNetsIconPath WRITE setShowNetsIconPath)
        Q_PROPERTY(QString hideNetsIconPath READ hideNetsIconPath WRITE setHideNetsIconPath)
        Q_PROPERTY(QString showGatesIconPath READ showGatesIconPath WRITE setShowGatesIconPath)
        Q_PROPERTY(QString hideGatesIconPath READ hideGatesIconPath WRITE setHideGatesIconPath)
        Q_PROPERTY(QString searchIconPath READ searchIconPath WRITE setSearchIconPath)
        Q_PROPERTY(QString searchIconStyle READ searchIconStyle WRITE setSearchIconStyle)
        Q_PROPERTY(QString searchActiveIconStyle READ searchActiveIconStyle WRITE setSearchActiveIconStyle)
        Q_PROPERTY(QString renameIconPath READ renameIconPath WRITE setRenameIconPath)
        Q_PROPERTY(QString deleteIconPath READ deleteIconPath WRITE setDeleteIconPath)
        Q_PROPERTY(QString expandedIconPath READ expandedIconPath WRITE setExpandedIconPath)
        Q_PROPERTY(QString collapsedIconPath READ collapsedIconPath WRITE setCollapsedIconPath)

    public:
        /**
         * Constructor.
         *
         * @param parent - The parent widget
         */
        ModuleWidget(QWidget* parent = nullptr);

        /**
         * Configures the ModuleWidget's toolbar. Since the ModuleWidget has no toolbar actions, nothing is done here.
         *
         * @param Toolbar - The toolbar to configure
         */
        virtual void setupToolbar(Toolbar* toolbar) override;

        /**
         * Creates and registers the shortcuts associated with the ModuleWidget.
         *
         * @returns the list of created shortcuts
         */
        virtual QList<QShortcut*> createShortcuts() override;

        /**
         * Accesses the module model.
         *
         * @returns the module model
         */
        ModuleModel* getModuleModel() const;

        /**
         * Get the widget's proxy model that represents the ModuleModel.
         *
         * @return The proxy model.
         */
        ModuleProxyModel* proxyModel();

        /** @name Q_PROPERTY READ Functions
         */
        ///@{
        QString disabledIconStyle() const;
        QString activeIconStyle() const;
        QString showNetsIconPath() const;
        QString hideNetsIconPath() const;
        QString showGatesIconPath() const;
        QString hideGatesIconPath() const;
        QString searchIconPath() const;
        QString searchIconStyle() const;
        QString searchActiveIconStyle() const;
        QString deleteIconPath() const;
        QString renameIconPath() const;
        QString expandedIconPath() const;
        QString collapsedIconPath() const;

        ///@}

        /** @name Q_PROPERTY WRITE Functions
         */
        ///@{
        void setDisabledIconStyle(const QString& style);
        void setActiveIconStyle(const QString& style);
        void setShowNetsIconPath(const QString &path);
        void setHideNetsIconPath(const QString &path);
        void setShowGatesIconPath(const QString &path);
        void setHideGatesIconPath(const QString &path);
        void setSearchIconPath(const QString &path);
        void setSearchIconStyle(const QString &style);
        void setSearchActiveIconStyle(const QString &style);
        void setDeleteIconPath(const QString& path);
        void setRenameIconPath(const QString& path);
        void setExpandedIconPath(const QString& path);
        void setCollapsedIconPath(const QString& path);
        ///@}

    public Q_SLOTS:
        /**
         * Q_SLOT to open/close the searchbar of the ModuleWidget depending on whether it is already open or not.
         */
        void toggleSearchbar();

        /**
         * Q_SLOT to overwrite the filter with the regular expression given in <i>text</i>.
         *
         * @param text - Contains the regular expression filter as a string
         */
        void filter(const QString& text);

        /**
         * Q_SLOT to update the search icon style. The search icon style indicates wether a filter is applied or not.
         */
        void updateSearchIcon();

        /**
         * Q_SLOT to open and handle the context menu of a module in the ModuleWidget.
         *
         * @param point - The contents coordinates of the click
         */
        void handleTreeViewContextMenuRequested(const QPoint& point);

        /**
         * Q_SLOT to handle that the selection within the tree view has been changed.
         * Updates the selection in other widgets using the selection relay.
         *
         * @param selected - The newly selected items
         * @param deselected - The newly deselected items
         */
        void handleTreeSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

        /**
         * Q_SLOT to handle tree view navigation
         *
         * @param current - The current index
         * @param previous - The previous index (unused)
         */
        void handleCurrentChanged(const QModelIndex& current, const QModelIndex& previous = QModelIndex());

        /**
         * Q_SLOT to handle a double-click on a module of the ModuleWidget.
         *
         * @param index - The module index of the item that has been double clicked.
         */
        void handleItemDoubleClicked(const QModelIndex& index);

        /**
         * Q_SLOT to handle that the item selection has been changed by another widget.
         *
         * @param sender - The responsible class for the selection change
         */
        void handleSelectionChanged(void* sender);

        /**
         * Q_SLOT to handle that a module has been removed from the netlist.
         *
         * @param module - The module that has been removed
         * @param module_id - The id of the module that has been removed
         */
        void handleModuleRemoved(Module* module, u32 module_id);

    private Q_SLOTS:
        /**
         * Q_SLOT to toggle the visibility of nets in the module widget. Called when the 'Toggle Net Visibility'-buttons was clicked.
         */
        void handleToggleNetsClicked();

        /**
         * Q_SLOT to toggle the visibility of gates in the module widget. Called when the 'Toggle Gate Visibility'-buttons was clicked.
         */
        void handleToggleGatesClicked();

        /**
         * Q_SLOT to toggle whether the tree is entirely expanded or collapsed
         */
        void handleToggleExpandTreeClicked();

        /**
         * Q_SLOT to handle rename button
         */
        void handleRenameClicked();

    private Q_SLOTS:
        void handleDeleteShortcutOnFocusChanged(QWidget *oldWidget, QWidget *newWidget);

        void deleteSelectedItem();

    private:
        ModuleTreeView* mTreeView;
        Searchbar* mSearchbar;

        QAction* mToggleNetsAction;
        QAction* mToggleGatesAction;
        QAction* mRenameAction;
        QAction* mDeleteAction;
        QAction* mToggleExpandTreeAction;

        QString mDisabledIconStyle;
        QString mActiveIconStyle;

        QString mShowNetsIconPath;
        QString mHideNetsIconPath;
        QString mShowGatesIconPath;
        QString mHideGatesIconPath;
        QString mDeleteIconPath;

        QString mSearchIconPath;
        QString mSearchIconStyle;
        QString mSearchActiveIconStyle;
        QString mRenameIconPath;
        QString mExpandedIconPath;
        QString mCollapsedIconPath;

        QAction* mFilterAction;

        QList<QRegExp*> mRegexps;

        bool mIgnoreSelectionChange;

        ModuleModel* mModuleModel;
        ModuleProxyModel* mModuleProxyModel;

        QShortcut* mShortCutDeleteItem;

        void enableDeleteAction(bool enable);

        ModuleItem* getModuleItemFromIndex(const QModelIndex& index);

    };
}    // namespace hal
