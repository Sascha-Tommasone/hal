#include "generate_hre_task/plugin_generate_hre_task.h"
#include "hal_core/netlist/netlist_parser/netlist_parser_manager.h"
#include "hal_core/plugin_system/fac_extension_interface.h"
#include "generate_hre_task/generate_hre_task.h"
#include "netlist_simulator_controller/plugin_netlist_simulator_controller.h"
#include "netlist_simulator_controller/netlist_simulator_controller.h"
#include "hal_core/plugin_system/plugin_manager.h"

namespace hal
{
    extern std::unique_ptr<BasePluginInterface> create_plugin_instance()
    {
        return std::make_unique<GenerateHreTaskPlugin>();
    }

    FacExtensionHreTask::FacExtensionHreTask()
        : FacExtensionInterface(FacExtensionInterface::FacNetlistParser)
    {
        m_description = "Generate Netlist for HRE Task";
        m_supported_file_extensions.push_back(".hre");
        FacFactoryProvider<NetlistParser>* fac = new FacFactoryProvider<NetlistParser>;
        fac->m_factory = []() { return std::make_unique<GenerateHreTask>(); };
        factory_provider = fac;
    }

    GenerateHreTaskPlugin::GenerateHreTaskPlugin()
        : m_fac_extension(nullptr), m_gui_extension(nullptr)
    {;}

    std::string GenerateHreTaskPlugin::get_name() const
    {
        return std::string("generate_hre_task");
    }

    std::string GenerateHreTaskPlugin::get_version() const
    {
        return std::string("0.1");
    }

    void GenerateHreTaskPlugin::on_load()
    {
        m_fac_extension = new FacExtensionHreTask;
        m_gui_extension = new GuiExtensionHreTask;
        m_gui_extension->m_parent = this;
        m_extensions.push_back(m_fac_extension);
        m_extensions.push_back(m_gui_extension);
    }

    void GenerateHreTaskPlugin::on_unload()
    {
        delete_extension(m_fac_extension);
        delete_extension(m_gui_extension);
    }

    /**
      * Returns plugin dependencies. Depends on GUI, simulation controller, verilator, waveform viewer
      * @return
      */
     std::set<std::string> GenerateHreTaskPlugin::get_dependencies() const
     {
         std::set<std::string> retval;
         retval.insert("netlist_simulator_controller");
         retval.insert("hal_gui");
         retval.insert("waveform_viewer");
         retval.insert("verilator");
         return retval;
     }

     bool GenerateHreTaskPlugin::simulate(std::filesystem::path sim_input)
     {
         enum {VCD, CSV, SAL} input_file_format;
         if (sim_input.extension() == ".vcd")
             input_file_format = VCD;
         else if (sim_input.extension() == ".csv")
             input_file_format = CSV;
         else if (sim_input.extension() == ".json")
             input_file_format = SAL;
         else
         {
             log_warning("hre_task", "Simulation input file '{}’ has unknown extension.", sim_input.string());
             return false;
         }
         NetlistSimulatorControllerPlugin* ctrlPlug = static_cast<NetlistSimulatorControllerPlugin*>(plugin_manager::get_plugin_instance("netlist_simulator_controller"));
         if (!ctrlPlug)
         {
             log_warning("hre_task", "Plugin 'netlist_simulator_controller' not found");
             return false;
         }
         m_simul_controller = ctrlPlug->create_simulator_controller();
         m_simul_controller.get()->add_gates(m_original_netlist.get()->get_gates());
         m_simul_controller.get()->set_no_clock_used();
         m_simul_controller.get()->create_simulation_engine("verilator");

         switch (input_file_format)
         {
         case VCD:
             if (!m_simul_controller.get()->import_vcd(sim_input.string(),NetlistSimulatorController::FilterInputFlag::CompleteNetlist))
             {
                 log_warning("hre_task", "Cannot parse simulation input file '{}'.", sim_input.string());
                 return false;
             }
             break;
         case CSV:
             m_simul_controller.get()->import_csv(sim_input.string(),NetlistSimulatorController::FilterInputFlag::CompleteNetlist);
             break;
         case SAL:
             m_simul_controller.get()->import_simulation(sim_input.parent_path().string(),NetlistSimulatorController::FilterInputFlag::CompleteNetlist);
             break;
         }

         std::vector<const Net*> probes;
         for (u32 id : {3, 4, 5, 14, 18, 15}) // input, output, internal probe 15
             probes.push_back(m_original_netlist.get()->get_net_by_id(id));
         m_simul_controller.get()->simulate_only_probes(probes);

         m_simul_controller.get()->emit_run_simulation();
         return true;
     }

     //----------------------

         GuiExtensionHreTask::GuiExtensionHreTask()
         {
             m_parameter.push_back(PluginParameter(PluginParameter::ExistingFile,"sim_input","Simulation input file"));
             m_parameter.push_back(PluginParameter(PluginParameter::PushButton,"simulate","Start Simulation"));
         }

         GuiExtensionHreTask::~GuiExtensionHreTask()
         {
         }

         std::vector<PluginParameter> GuiExtensionHreTask::get_parameter() const
         {
             return m_parameter;
         }

         void GuiExtensionHreTask::set_parameter(const std::vector<PluginParameter>& params)
         {
             m_parameter = params;
             bool simulate = false;
             std::filesystem::path sim_input;
             for (PluginParameter par : m_parameter)
             {
                 if(par.get_tagname()=="simulate" && par.get_value() == "clicked")
                     simulate = true;
                 else if (par.get_tagname()=="sim_input")
                     sim_input = par.get_value();
             }
             if (simulate && m_parent )
             {
                m_parent->simulate(sim_input);
             }
         }


}    // namespace hal
