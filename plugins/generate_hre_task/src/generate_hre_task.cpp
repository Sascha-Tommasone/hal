#include "generate_hre_task/generate_hre_task.h"
#include "generate_hre_task/plugin_generate_hre_task.h"

#include "hal_core/netlist/endpoint.h"
#include "hal_core/netlist/gate.h"
#include "hal_core/netlist/netlist.h"
#include "hal_core/netlist/netlist_factory.h"
#include "hal_core/netlist/persistent/netlist_serializer.h"
#include "hal_core/utilities/enums.h"
#include "hal_core/utilities/log.h"
#include "hal_core/utilities/utils.h"
#include "hal_core/plugin_system/plugin_manager.h"

#include <fstream>
#include <iomanip>
#include <queue>

namespace hal
{
    /**
     * This shared_ptr contains and owns the netlist after a file was opened. To access the Netlist data structure with
     * its functions, it is intended to use the raw pointer gNetlist instead.
     */
    extern std::shared_ptr<Netlist> gNetlistOwner;

    /**
     * The netlist that is currently loaded. It is a pointer to the netlist owned by gNetlistOwner.
     */
    extern Netlist* gNetlist;

    Result<std::monostate> GenerateHreTask::parse(const std::filesystem::path& file_path)
    {
        if (file_path.empty() || file_path.extension() != ".hre")
            return ERR("GenerateHreTask cannot handle file <" + file_path.string() + ">");
        m_path = file_path;
        if (!std::filesystem::exists(m_path))
            return ERR("File <" + m_path.string() + "> does not exist");

        return OK({});
    }

    Result<std::unique_ptr<Netlist>> GenerateHreTask::instantiate(const GateLibrary* gate_library)
    {
        // Load original .hre netlist with selected gate library
        // It is assumed that the .hre netlist is stored in HAL format and resides in a HAL project
        std::unique_ptr<Netlist> original_nl = netlist_serializer::deserialize_from_file(m_path, (GateLibrary*) gate_library);
        if (original_nl)
        {
            if (auto res = original_nl.get()->copy(); res.is_ok())
            {
                gNetlist = res.get().get();
                if (!manipulate())
                    return ERR("failed to do hard wired manipulations to netlist <" + m_path.string() + ">");

                GenerateHreTaskPlugin* hrePlug = static_cast<GenerateHreTaskPlugin*>(plugin_manager::get_plugin_instance("generate_hre_task"));
                if (!hrePlug)
                    return ERR("cannot access 'generate_hre_task' plugin, maybe the name changed");
                hrePlug->m_original_netlist = std::move(original_nl);
                return OK({std::move(res.get())});
            }
            else
                return ERR_APPEND(res.get_error(), "deep copy of netlist <" + m_path.string() + "> failed");

        }

        return ERR("could not load netlist <" + m_path.string() + ">");
    }

    bool GenerateHreTask::manipulate()
    {
        if (!gNetlist) return false;
        GateType* gt = gNetlist->get_gate_library()->get_gate_type_by_name("XNOR2_X1");
        if (!gt) return false;
        Gate* g = gNetlist->get_gate_by_id(2);
        if (!g) return false;
        std::string name = g->get_name();
        gNetlist->delete_gate(g);

        Net* na = gNetlist->get_net_by_id(3);
        if (!na) return false;
        Net* nb = gNetlist->get_net_by_id(4);
        if (!nb) return false;
        Net* nz = gNetlist->get_net_by_id(16);
        if (!nz) return false;
        g = gNetlist->create_gate(2, gt, name);
        if (!g) return false;
        na->add_destination(g,"B");
        nb->add_destination(g,"A");
        nz->add_source(g,"ZN");
        return true;
    }
}
