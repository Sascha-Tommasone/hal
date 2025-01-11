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

#include "hal_core/plugin_system/plugin_interface_base.h"
#include "hal_core/plugin_system/gui_extension_interface.h"
#include <QMetaType>

Q_DECLARE_METATYPE(std::string)

namespace hal
{

    class LogicEvaluator;
    class NetlistSimulator;

    class PLUGIN_API LogicEvaluatorPlugin : public BasePluginInterface
    {
    public:
        LogicEvaluatorPlugin();

        std::string get_name() const override;
        std::string get_version() const override;
        std::string get_description() const override;

        void on_load() override;
        void on_unload() override;
        std::set<std::string> get_dependencies() const override;
    };

    class GuiExtensionLogicEvaluator : public GuiExtensionInterface
    {
        bool mSkipCompile;

    public:
        static bool acceptGate(const Gate* g);
        /**
         * @brief Default constructor for `GuiExtensionEvaluator`.
         */
        GuiExtensionLogicEvaluator() : GuiExtensionInterface(), mSkipCompile(false) {;}

        /**
         * @brief Get a vector of configurable parameters.
         *
         * @returns The vector of parameters.
         */
        std::vector<PluginParameter> get_parameter() const override;

        /**
         * @brief Set values for a vector of configurable parameters.
         *
         * @param[in] params - The parameters including their values.
         */
        void set_parameter(const std::vector<PluginParameter>& params) override;


        /**
         * Contribution to context menu.
         * This function gets called when a context menu in GUI graphical netlist view pops up. Plugins that
         * want to add their own entries in context menu will return one ContextMenuContribution record per
         * entry line.
         *
         * @param[in] nl - The current netlist in GUI
         * @param[in] mods - List of selected modules
         * @param[in] gats - List of selected gates
         * @param[in] nets - List of selected nets
         * @return
         */
        virtual std::vector<ContextMenuContribution> get_context_contribution(const Netlist* nl, const std::vector<u32>& mods, const std::vector<u32>& gats, const std::vector<u32>& nets) override;

        /**
         * Call from GUI to execute function.
         * This function gets called when user selected a plugin contribution (see get_context_contribution) in
         * context menu of GUI graphical netlist view or when user clicked a push button in contributed plugin menu
         *
         * @param[in] tag - The function tagname (unique identifier)
         * @param[in] nl - The current netlist in GUI
         * @param[in] mods - List of selected modules
         * @param[in] gats - List of selected gates
         * @param[in] nets - List of selected nets
         */
        virtual void execute_function(std::string tag, Netlist* nl, const std::vector<u32>& mods, const std::vector<u32>& gats, const std::vector<u32>& nets) override;
    };
}    // namespace hal
