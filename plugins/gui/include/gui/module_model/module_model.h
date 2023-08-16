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
#include "gui/gui_utils/sort.h"
#include "hal_core/netlist/module.h"
#include "gui/module_model/module_item.h"

#include "gui/basic_tree_model/base_tree_model.h"
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QColor>
#include <set>
#include <array>

namespace hal
{    
    class ModuleTreeItem : public BaseTreeItem
    {
        public:
            enum class TreeItemType {Module, Gate, Net};

        private:
            u32 mId;
            TreeItemType mType;
            QString mName;


            ModuleTreeItem* mParent;
            QList<ModuleTreeItem*> mChildItems;

            QColor mColor;
            bool mHighlighted;
        public:

            ModuleTreeItem(const u32 id, const TreeItemType type = TreeItemType::Module);
            QVariant getData(int column) const override;
            void setData(QList<QVariant> data) override;
            void setDataAtIndex(int index, QVariant& data) override;
            void appendData(QVariant data) override;
            int getColumnCount() const override;
            /**
             * Gets the type of the netlist item this ModuleTreeItem represents.
             *
             * @returns the ModuleItem type
             */
            TreeItemType getType() const;
            /**
             * Gets the id of the netlist item this ModuleTreeItem represents.
             *
             * @returns the module id
             */
            u32 id() const;
            /**
             * Gets the data of this item model item i.e. the name of this ModuleTreeItem if column=1.
             *
             * @param column - The column to get the data for
             * @returns the data in the specified column of this ModuleItem
             */
            QVariant data(int column) const;
            /**
             * Checks if this ModuleTreeItem is currently highlighted.
             *
             * @returns <b>true</b> if this ModuleItem is currently highlighted.
             */
            bool highlighted() const;
            /**
             * Gets the index of this ModuleTreeItem in the list of children ModuleTreeItems of its parent.
             *
             * @returns the index in the parents ModuleItem children list
             */
            int row() const;
            /**
             * Gets the parent ModuleItem of this ModuleItem.
             *
             * @returns the parent ModuleItem. Returns a constant ModuleItem pointer
             */
            const ModuleTreeItem* constParent() const;
            /**
             * Given a set of ModuleItems (in a map [id]->[ModuleItem]) this function adds each ModuleItem of this set as
             * a new children if its underlying module is a submodule (child) of the underlying module of this ModuleItem.
             *
             * @param moduleMap - A map [id]->[ModuleItem] of children candidates
             */
            void appendExistingChildIfAny(const QMap<u32,ModuleTreeItem*>& moduleMap);
            /**
             * Gets the current amount of children of this ModuleTreeItem.
             *
             * @returns the amount of children
             */
            int childCount() const;
            /**
             * Gets the parent ModuleItem of this ModuleItem.
             *
             * @returns the parent ModuleItem
             */
            ModuleTreeItem* parent();
            /**
             * Sets the name of this ModuleItem (not the underlying module).
             *
             * @param name - The new name
             */
            void setName(const QString& name);

            /**
             * Sets the color of the module this ModuleItem represents.
             *
             * @param color - The new color
             */
            void setColor(const QColor& color);

            /**
             * Gets the color of the netlist item this ModuleItem represents.
             *
             * @returns the module color
             */
            QColor color() const;
    };

    /**
     * @ingroup gui
     * @brief Represents the netlist module's hierarchy.
     *
     * The ModuleModel is the item model that represents the modules and their hierarchy in the netlist.
     */
    class ModuleModel : public BaseTreeModel
    {
        Q_OBJECT

    public:
        /**
         * Constructor. <br>
         * Since the netlist is not necessarily loaded when this class is instantiated, the model won't be filled with
         * data until the init function is called. The constructor is an empty one.
         *
         * @param parent - The parent object.
         */
        explicit ModuleModel(QObject* parent = nullptr);

        /**
         * Returns the data stored under the given role for the item referred to by the index.
         *
         * @param index - The model index
         * @param role - The role the model is accessed with
         * @returns the data stored at index under the specified role
         */
        QVariant data(const QModelIndex& index, int role) const override;


        // === Others ===

        /**
         * Returns the ModuleItem stored under the specified model index.
         *
         * @param index - The model index to get the ModuleItem from
         * @returns the module item at the specified index
         */
        ModuleTreeItem* getItem(const QModelIndex& index) const;

        /**
         * Returns the index where the specified ModuleItem can be found.
         *
         * @param item - The ModuleItem to search for in the item model
         * @returns the model index of the specified ModuleItem
         */
        QModelIndex getIndex(const ModuleTreeItem* const item) const;

        /**
         * Returns the ModuleItem for a specified id and type.
         *
         * @param module_id - The id of the ModuleItem
         * @param type - The type of the ModuleItem
         * @returns the ModuleItem with the specified id and type.
         */
        ModuleTreeItem* getItem(const u32 id, ModuleTreeItem::TreeItemType type = ModuleTreeItem::TreeItemType::Module) const;

        /**
         * Initializes the item model using the global netlist object gNetlist.
         */
        void init();

        /**
         * Clears the item model and deletes all ModuleItems.
         */
        void clear();

        /**
         * Add a module to the item model. For the specified module a new ModuleItem is created and stored.
         *
         * @param id - The id of the module to add.
         * @param parent_module - The id of the parent module of the module to add.
         */
        void addModule(const u32 id, const u32 parent_module);

        /**
         * Add a gate to the item model. For the specified gate a new ModuleItem is created and stored.
         *
         * @param id - The id of the gate to add.
         * @param parent_module - The id of the parent module of the gate to add.
         */
        void addGate(const u32 id, const u32 parent_module);

        /**
         * Add a net to the item model. For the specified net a new ModuleItem is created and stored.
         *
         * @param id - The id of the net to add.
         * @param parent_module - The id of the parent module of the net to add.
         */
        void addNet(const u32 id, const u32 parent_module);

        /**
         * Recursively adds all given modules with all their sub modules (and their submodules and so on...)
         * and the gates and nets of those modules to the item model.
         *
         * @param modules - The list of modules which should be added to the item model together
         *                  with all their submodules, gates and nets.
         */
        void addRecursively(const std::vector<Module*>& modules, QSet<u32>& added_nets);

        /**
         * Removes a module from the item model. The specified module MUST be contained in the item model.
         *
         * @param id - The id of the module to remove
         */
        void remove_module(const u32 id);

        /**
         * Removes a gate from the item model. The specified gate MUST be contained in the item model.
         *
         * @param id - The id of the gate to remove
         */
        void remove_gate(const u32 id);

        /**
         * Removes a net from the item model. The specified net MUST be contained in the item model.
         *
         * @param id - The id of the net to remove
         */
        void remove_net(const u32 id);

        /**
         * Updates the ModuleItem for the specified module. The specified module MUST be contained in the item model.
         *
         * @param id - The id of the module to update
         */
        void updateModule(const u32 id);

//        void addModule(u32 id, u32 parent_module);
//        void addRecursively(const std::vector<Module*>& modules);
//        void remove_module(u32 id);
//        void updateModule(u32 id);


        /**
         * Returns <b>true</b> if the item model is currently changed/updated. This is the case while adding and
         * removing modules to/from the item model. It can be used to ignore certain signals sent by the item model
         * while the model is modified.
         *
         * @returns <b>true</b> while the item model is modified. Returns <b>false</b> otherwise.
         */
        bool isModifying();

        /**
         * Gets the module color of a module of a specific id.
         *
         * @param id - The module id of the module to get the color for
         * @returns the color of the module
         */
        QColor moduleColor(u32 id) const;

        /**
         * Changes the color of a module.
         *
         * @param id - The id of the module
         * @param col - The new color
         * @returns the old color of the module (used to create an undo action easier)
         */
        QColor setModuleColor(u32 id, const QColor& col);

        /**
         * Changes the color of a module to a random color.
         *
         * @param id - The id of the module
         * @returns the old color of the module (used to create an undo action easier)
         */
        QColor setRandomColor(u32 id);

        /**
         * Removes the color that belongs to the given id.
         *
         * @param id - The module id for which to remove the color.
         */
        void removeColor(u32 id);

    private:
        ModuleTreeItem* mTopModuleItem;

        QMap<u32, ModuleTreeItem*> mModuleMap;
        QMap<u32, ModuleTreeItem*> mGateMap;
        QMap<u32, ModuleTreeItem*> mNetMap;
        std::array<QMap<u32, ModuleTreeItem*>*, 3> mModuleItemMaps = {&mModuleMap, &mGateMap, &mNetMap};;
        QMap<u32, QColor> mModuleColors;

        bool mIsModifying;
    };
}    // namespace hal
