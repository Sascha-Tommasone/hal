#include "hal_core/netlist/gate.h"
#include "hal_core/netlist/net.h"
#include "machine_learning/labels/gate_label.h"
#include "netlist_preprocessing/netlist_preprocessing.h"
#include "nlohmann/json.hpp"

namespace hal
{
    namespace machine_learning
    {
        namespace gate_label
        {
            Result<std::vector<u32>> GateNameKeyWord::calculate_label(Context& ctx, const Gate* g) const
            {
                UNUSED(ctx);

                for (const auto& gtp : m_applicable_to)
                {
                    if (g->get_type()->has_property(gtp))
                    {
                        const auto& gate_name = g->get_name();
                        if (gate_name.find(m_key_word) != std::string::npos)
                        {
                            return OK(MATCH);
                        }

                        return OK(MISMATCH);
                    }
                }

                return OK(NA);
            }

            Result<std::vector<std::vector<u32>>> GateNameKeyWord::calculate_labels(Context& ctx, const std::vector<Gate*>& gates) const
            {
                std::vector<std::vector<u32>> labels;

                for (const auto& g : gates)
                {
                    const auto new_label = calculate_label(ctx, g);
                    if (new_label.is_error())
                    {
                        return ERR_APPEND(new_label.get_error(), "Cannot caluclate label for gate  " + g->get_name() + " with ID " + std::to_string(g->get_id()));
                    }

                    labels.push_back(new_label.get());
                }

                return OK(labels);
            }

            Result<std::vector<std::vector<u32>>> GateNameKeyWord::calculate_labels(Context& ctx) const
            {
                const auto labels = calculate_labels(ctx, ctx.get_gates());
                if (labels.is_error())
                {
                    return ERR_APPEND(labels.get_error(), "Failed to calculate labels");
                }

                return OK(labels.get());
            }

            std::string GateNameKeyWord::to_string() const
            {
                std::string applicable_to_str = utils::join("_", m_applicable_to.begin(), m_applicable_to.end(), [](const GateTypeProperty& gtp) { return enum_to_string(gtp); });

                return "GateNameKeyWord_" + m_key_word + "_" + (m_applicable_to.empty() ? "ALL" : applicable_to_str);
            }

            Result<std::vector<u32>> NetNameKeyWord::calculate_label(Context& ctx, const Gate* g) const
            {
                UNUSED(ctx);

                for (const auto& gtp : m_applicable_to)
                {
                    if (g->get_type()->has_property(gtp))
                    {
                        if (!g->has_data("preprocessing_information", "multi_bit_indexed_identifiers"))
                        {
                            log_error("machine_learning", "unable to find indexed identifiers for gate with ID {}", g->get_id());
                            continue;
                        }

                        const std::string json_string = std::get<1>(g->get_data("preprocessing_information", "multi_bit_indexed_identifiers"));

                        nlohmann::json j                                                         = nlohmann::json::parse(json_string);
                        std::vector<netlist_preprocessing::indexed_identifier> index_information = j.get<std::vector<netlist_preprocessing::indexed_identifier>>();

                        // for each pin, only consider the index information with the least distance
                        std::map<std::string, u32> pin_to_min_distance;
                        for (const auto& [_name, _index, _origin, pin, _direction, distance] : index_information)
                        {
                            if (const auto it = pin_to_min_distance.find(pin); it == pin_to_min_distance.end())
                            {
                                pin_to_min_distance.insert({pin, distance});
                            }
                            else
                            {
                                pin_to_min_distance.at(pin) = std::min(it->second, distance);
                            }
                        }

                        std::map<std::string, std::string> pin_to_net_name;
                        for (const auto& [name, _index, _origin, pin, _direction, distance] : index_information)
                        {
                            if (pin_to_min_distance.at(pin) == distance)
                            {
                                pin_to_net_name.insert({pin, name});
                            }
                        }

                        for (const auto& pt : m_pin_types)
                        {
                            const auto& pins = g->get_type()->get_pins([&pt](const auto& gt_p) { return gt_p->get_type() == pt; });
                            for (const auto* p : pins)
                            {
                                std::string net_name;
                                if (const auto it = pin_to_net_name.find(p->get_name()); it != pin_to_net_name.end())
                                {
                                    net_name = pin_to_net_name.at(p->get_name());
                                }
                                else
                                {
                                    const auto* ep = (p->get_direction() == PinDirection::input) ? g->get_fan_in_endpoint(p) : g->get_fan_out_endpoint(p);
                                    net_name       = ep->get_net()->get_name();
                                }

                                if (net_name.find(m_key_word) != std::string::npos)
                                {
                                    return OK(MATCH);
                                }
                            }
                        }

                        return OK(MISMATCH);
                    }
                }

                return OK(NA);
            }

            Result<std::vector<std::vector<u32>>> NetNameKeyWord::calculate_labels(Context& ctx, const std::vector<Gate*>& gates) const
            {
                std::vector<std::vector<u32>> labels;

                for (const auto& g : gates)
                {
                    const auto new_label = calculate_label(ctx, g);
                    if (new_label.is_error())
                    {
                        return ERR_APPEND(new_label.get_error(), "Cannot caluclate label for gate  " + g->get_name() + " with ID " + std::to_string(g->get_id()));
                    }

                    labels.push_back(new_label.get());
                }

                return OK(labels);
            }

            Result<std::vector<std::vector<u32>>> NetNameKeyWord::calculate_labels(Context& ctx) const
            {
                const auto labels = calculate_labels(ctx, ctx.get_gates());
                if (labels.is_error())
                {
                    return ERR_APPEND(labels.get_error(), "Failed to calculate labels");
                }

                return OK(labels.get());
            }

            std::string NetNameKeyWord::to_string() const
            {
                std::string pin_types_str     = utils::join("_", m_pin_types.begin(), m_pin_types.end(), [](const PinType& pt) { return enum_to_string(pt); });
                std::string applicable_to_str = utils::join("_", m_applicable_to.begin(), m_applicable_to.end(), [](const GateTypeProperty& gtp) { return enum_to_string(gtp); });

                return "NetNameKeyWord_" + m_key_word + "_" + pin_types_str + "_" + (m_applicable_to.empty() ? "ALL" : applicable_to_str);
            }

        }    // namespace gate_label
    }        // namespace machine_learning
}    // namespace hal