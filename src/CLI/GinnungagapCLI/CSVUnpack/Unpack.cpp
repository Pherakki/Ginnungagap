#include "Unpack.hpp"

#include <unordered_map>
#include <iostream>
#include <fstream>
#include <GinnungagapCore/utils/Paths.hpp>

static void dumpParamData(std::ofstream& stream, const ParameterSet::Parameters& dataset, const MXEParameterSetGroupDefinition& pdef, uint32_t ID, const std::string& data_name, uint32_t size)
{
    stream << ID;
    if (!data_name.empty())
        stream << ",\"" << stringmanip::sjisToUtf8(data_name) << "\"";
    if (pdef.hasMultipleDefinitions())
        stream << "," << size;
    
    for (const auto& elem : dataset)
    {
        stream << ",";

        switch(elem.type)
        {
            case ElementType::Float32:
            {
                stream << std::setprecision(9) << elem.data.m_float;
                break;
            }
            case ElementType::Float64:
            {
                stream << std::setprecision(17) << elem.data.m_double;
                break;
            }
            case ElementType::Int8:
            {
                stream << (int32_t)elem.data.m_int8;
                break;
            }
            case ElementType::Int16:
            {
                stream << (int32_t)elem.data.m_int16;
                break;
            }
            case ElementType::Int32:
            {
                stream << elem.data.m_int32;
                break;
            }
            case ElementType::Int64:
            case ElementType::Asset:
            {
                stream << elem.data.m_int64;
                break;
            }
            case ElementType::UInt8:
            case ElementType::Pad8:
            {
                stream << (uint32_t)elem.data.m_uint8;
                break;
            }
            case ElementType::UInt16:
            case ElementType::Pad16:
            {
                stream << (uint32_t)elem.data.m_uint16;
                break;
            }
            case ElementType::UInt32:
            case ElementType::Pad32:
            case ElementType::Offset32:
            {
                stream << elem.data.m_uint32;
                break;
            }
            case ElementType::UInt64:
            case ElementType::Pad64:
            case ElementType::Offset64:
            {
                stream << elem.data.m_uint64;
                break;
            }
            case ElementType::SJISString:
            {
                stream << '\"' << stringmanip::sjisToUtf8(elem.string) << '\"';
                break;
            }
            case ElementType::UTF8String:
            {
                stream << '\"' << elem.string << '\"';
                break;
            }
            case ElementType::CharColor:
            {
                const auto& c = elem.data.m_char4;
                stream << '\"' << (uint32_t)c[0] << " " << (uint32_t)c[1] << " " << (uint32_t)c[2] << " " << (uint32_t)c[3] << '\"';
                break;
            }
            case ElementType::FloatColor:
            {
                const auto& c = elem.data.m_float4;
                stream << '\"' << c[0] << " " << c[1] << " " << c[2] << " " << c[3] << '\"';
                break;
            }
        }
    }
    stream << std::endl;
}

std::string terseifyTypeName(const ElementType& type)
{
    switch (type)
    {
        case ElementType::Int8:
            return "b";
        case ElementType::Int16:
            return "h";
        case ElementType::Int32:
            return "i";
        case ElementType::Int64:
            return "q";
        case ElementType::UInt8:
            return "B";
        case ElementType::UInt16:
            return "H";
        case ElementType::UInt32:
            return "I";
        case ElementType::UInt64:
            return "Q";
        case ElementType::Pad8:
            return "B";
        case ElementType::Pad16:
            return "H";
        case ElementType::Pad32:
            return "I";
        case ElementType::Pad64:
            return "Q";
        case ElementType::Asset:
            return "A";
        case ElementType::Offset32:
            return "O";
        case ElementType::Offset64:
            return "O";
        case ElementType::Float32:
            return "f";
        case ElementType::Float64:
            return "d";
        case ElementType::SJISString:
            return "s";
        case ElementType::UTF8String:
            return "s";
        case ElementType::CharColor:
            return "B4";
        case ElementType::FloatColor:
            return "f4";
        default:
        {
            std::cout << "Error: Unknown element type " << (uint32_t)type << ". Aborting." << std::endl;
            exit(1);
        }
    };
}

template<typename DumpFunc>
static void dumpParamsCSV(const std::filesystem::path& fpath, const MXEParameterSetGroupDefinition& pdef, DumpFunc dump_func, const std::string& idname="ID", const std::string& param_name="Name", bool terse=false)
{
    std::ofstream stream = safe_create_textfile(fpath);
    const auto& mdef = pdef.fullDefinition();

    // Write header
    stream << idname;
    if (!param_name.empty())
        stream << ",Name";
    if (pdef.hasMultipleDefinitions())
    {
        auto def_sizes = pdef.getDefinitionSizes();
        stream << ",\"Variant [" << std::accumulate(std::next(def_sizes.begin()), def_sizes.end(), std::to_string(def_sizes[0]), 
                                                    [](const std::string& a, size_t b){ return a + "/" + std::to_string(b); }) << "]\"";
    }
    
    if (!terse)
    {
        for (const auto& data_elem : mdef.data)
            stream << ",\"" << data_elem.name << " [" << data_elem.type_name << "]\""; // ASCII, no need to convert encodings
    }
    else
    {
        for (const auto& data_elem : mdef.data)
            stream << ",\"" << terseifyTypeName(data_elem.type) << ":" << data_elem.name << "\""; // ASCII, no need to convert encodings
    }
    stream << std::endl;

    dump_func(stream);
    stream.close();
}

void unpackMXEToCSV(const MXE& mxe, const ParameterDefMap& params_defmap, const FlatEntityDefMap& entity_defmap, const std::string& path, bool terse)
{
    std::filesystem::path root_path(path);
    
    safe_create_dir(path);

    auto& mxec = mxe.mxen.mxec;

    // METADATA DUMP
    {
        std::ofstream stream = safe_create_textfile(root_path/"metadata.txt");
        stream << "mxe_flag=" << mxec.header.flags.flag_0() << "\n";
        stream << "unknown_index=" << mxec.toc.unknown_count_1 << "\n";
        stream << "assets_unknown_0x00=" << mxec.atable.toc.unknown_0x00 << "\n";
        stream << "has_unknown_table_2=" << (mxec.toc.unknown_table_2_offset > 0) << "\n";
    }

    unpackParametersTable(mxe, params_defmap, root_path, terse);
    unpackEntitiesTable  (mxe, entity_defmap, root_path);
    unpackPathsTable     (mxe, root_path);
    unpackAssetsTable    (mxe, root_path);
    unpackGroupsTable    (mxe, root_path);
}

void unpackParametersTable(const MXE& mxe, const ParameterDefMap& params_defmap, std::filesystem::path& root_path, bool terse)
{
    auto& mxec = mxe.mxen.mxec;

    // Organise parameters of the same type
    std::unordered_map<std::string, std::vector<size_t>> categorised_parameters;
    for (int i=0; i < mxec.ptable.psets.size(); ++i)
    {
        const auto& p = mxec.ptable.psets[i];
        if (!categorised_parameters.count(p.type_name))
            categorised_parameters.emplace(p.type_name, std::vector<size_t>());
        
        categorised_parameters.at(p.type_name).push_back(i);
    }

    std::filesystem::path params_root = root_path/"parameters";
    safe_create_dir(params_root);

    for (const auto& [category, indices] : categorised_parameters)
    {    
        const auto& iref = indices; // Lambda fails for clang without this.

        // Dump main parameters
        const auto& pdef = params_defmap.at(category);
        dumpParamsCSV(params_root/(stringmanip::sjisToUtf8(category) + ".csv"),
                      pdef,
                      [&](std::ofstream& stream)
                      {
                        for (const auto& idx : iref)
                        {
                            const auto& pdata = mxec.ptable.psets[idx];
                            dumpParamData(stream, pdata.dataset, pdef, pdata.ID, pdata.data_name, pdata.size);
                        }
                      },
                      "ID",
                      "Name",
                      terse
                      );
        
        // Dump subparameters
        const auto& mdef = pdef.fullDefinition();
        if (!mdef.subparams.empty())
        {
            auto subparams_dir = params_root/category;
            safe_create_dir(subparams_dir);
            for (const auto& subparam_def : mdef.subparams)
            {
                std::string subparam_name = subparam_def.name;
                std::string subcat        = subparam_def.type;
                const auto& subpdef       = params_defmap.at(subcat);

                dumpParamsCSV(subparams_dir/(subparam_name + ".csv"),
                    subpdef,
                    [&](std::ofstream& stream)
                    {
                        for (const auto& idx : iref)
                        {
                            const auto& pdata = mxec.ptable.psets[idx];
                            for (const auto& subpdata : pdata.subparameters.at(subparam_name))
                            {
                                dumpParamData(stream, subpdata, subpdef, pdata.ID, "", subpdef.fullDefinition().size);
                            }
                            
                        }
                    },
                    "Parent ID",
                    "",
                    terse);
            }
        }
    }
}


void unpackEntitiesTable(const MXE& mxe, const FlatEntityDefMap& entity_defmap, std::filesystem::path& root_path)
{
    auto& mxec = mxe.mxen.mxec;

    if (mxec.etable.entities.size())
    {
        std::filesystem::path entities_path = root_path/"entities";
        safe_create_dir(entities_path);
        
        // Organise entities of the same type
        std::unordered_map<std::string, std::vector<size_t>> categorised_entities;
        for (int i=0; i < mxec.etable.entities.size(); ++i)
        {
            const auto& e = mxec.etable.entities[i];
            const auto& ename = e.components.back().name;
            if (!categorised_entities.count(ename))
                categorised_entities.emplace(ename, std::vector<size_t>());
            
            categorised_entities.at(ename).push_back(i);
        }

        // Dump Entities
        for (const auto& [en_type, en_indices] : categorised_entities)
        {
            if (!entity_defmap.contains(en_type))
            {
                std::string msg = "Unknown entity type: " + en_type;
                std::cout << msg << "\n";
                throw std::runtime_error(msg);
            }
            const auto& edef = entity_defmap.at(en_type);
            std::ofstream stream = safe_create_textfile(entities_path/(en_type + ".csv"));
            stream << "ID,Name,Controller ID";
            for (const auto& comp : edef.components)
            for (const auto& p    : comp.parameters)
                stream << "," << p.param_type;
            stream << "\n";

            for (const auto idx : en_indices)
            {
                const auto& en = mxec.etable.entities[idx];
                stream << en.ID << "," << stringmanip::sjisToUtf8(en.name) << "," << en.controller_ID;

                int comp_idx = en.components.size()-1;
                for (const auto& comp_def : edef.components)
                {
                    // Check we haven't exhausted the component count in the file...
                    if (comp_idx < 0)
                    {
                        for (const auto& param_id : comp_def.parameters)
                            stream << "," << "-1";
                        continue;
                    }

                    // Check that there isn't a missing component in the middle of the component array...
                    const auto& comp = en.components[comp_idx];
                    if (comp.name != comp_def.name)
                    {
                        for (const auto& param_id : comp_def.parameters)
                            stream << "," << "-1";
                        continue;
                    }

                    // Check that the component defines all the parameters associated with it...
                    if (comp.parameter_sets_count != comp_def.parameters.size())
                    {
                        std::cout << "Error: Corrupt entity definition for ID " << en.ID << ". Component parameter count does not match definition. Aborting." << std::endl;
                        exit(1);
                    }

                    // If all those conditions are met, finally dump it to file...
                    for (int param_idx=0; param_idx < comp.parameter_set_ids.size(); ++ param_idx)
                    {
                        auto param_id = comp.parameter_set_ids[param_idx];
                        if (param_id >= mxec.ptable.psets.size())
                        {
                            std::cout << "Error: Corrupt entity definition for ID " << en.ID << ". Component parameter has a parameter ID that exceeds the total parameter count. Aborting." << std::endl;
                            exit(1);
                        }

                        const auto& param = mxec.ptable.psets[param_id];
                        const std::string& expected_type_name = comp_def.parameters[param_idx].param_type;
                        if (param.type_name != expected_type_name)
                        {
                            std::cout << "Error: Corrupt entity definition for ID " << en.ID << ". Component parameter's ID [" << param_id << "] does not correspond to the correct parameter type [Expected " << expected_type_name << ", found " << param.type_name << "]. Aborting." << std::endl;
                            exit(1);
                        }

                        stream << "," << param_id;
                    }

                    comp_idx--;
                }
                stream << "\n";
            }
        }
    }
}


void unpackPathsTable(const MXE& mxe, std::filesystem::path& root_path)
{
    auto& mxec = mxe.mxen.mxec;

    // REVISIT THIS... PATHS OF THE SAME TYPE CONTAIN NODES FROM OTHER PATHS!!!
    // Currently, an entire graph is naively dumped to CSV.
    // However, really, we should dump by type. 
    if (mxec.gtable.entries.size())
    {
        std::filesystem::path paths_path = root_path/"paths";
        safe_create_dir(paths_path);

        // Create list of graph names
        {
            std::ofstream stream = safe_create_textfile(paths_path/"names.txt");
            for (int i=0; i < mxec.gtable.entries.size(); ++i)
            {
                const auto& entry = mxec.gtable.entries[i];
                stream << stringmanip::sjisToUtf8(entry.name) << "\n";
            }
        }

        for (int i=0; i < mxec.gtable.entries.size(); ++i)
        {
            const auto& path = mxec.gtable.entries[i];
            std::filesystem::path cur_path_path = paths_path/(std::to_string(i) + ".csv");
            std::ofstream stream = safe_create_textfile(cur_path_path);

            stream << "Node Param ID,Subgraph";
            if (path.edges.size())
            {
                uint32_t max_connections = std::max_element(path.edges.begin(), path.edges.end(), [](const PathEdge& a,const PathEdge& b) { return a.param_count < b.param_count; })->param_count;
                for (int conn_i=0; conn_i < max_connections; ++conn_i)
                {
                    stream << ",Next Node Param ID " << (conn_i+1);
                    stream << ",Edge ID " << (conn_i+1);
                    stream << ",Edge " << (conn_i+1) << " Param IDs";
                }
            }
            stream << "\n";

            std::vector<int> node_subgraph_ids(path.nodes.size(), -1);
            for (int subgraph_idx=0; subgraph_idx < path.subgraphs.size(); ++subgraph_idx)
            {
                const auto& subgraph = path.subgraphs[subgraph_idx];
                for (const auto& node_idx : subgraph.node_indices)
                    node_subgraph_ids[node_idx] = subgraph_idx;
            }

            for (int node_idx=0; node_idx < path.nodes.size(); ++node_idx)
            {
                const auto& node = path.nodes[node_idx];
                stream << node.param_ID;
                stream << "," << node_subgraph_ids[node_idx];
                for (const auto& edge_idx : node.outgoing_edge_idxs)
                {
                    const auto& edge = path.edges[edge_idx];
                    stream << "," << path.nodes[edge.end_node_idx].param_ID;
                    stream << "," << edge_idx;
                    stream << ",";
                    for (int edge_param_idx_idx=0; edge_param_idx_idx < edge.param_ids.size(); ++edge_param_idx_idx)
                    {
                        if (edge_param_idx_idx!=0)
                            stream << " ";
                        stream << edge.param_ids[edge_param_idx_idx];
                    }
                }
                stream << "\n";
            }
        }
    }
}


void unpackAssetsTable(const MXE& mxe, std::filesystem::path& root_path)
{
    auto& mxec = mxe.mxen.mxec;
    
    if (mxec.atable.arefs.size())
    {
        std::ofstream stream = safe_create_textfile(root_path/"assets.csv");
        stream << "ID;Directory;Filename;Mergefile Index;Texmerge File Index; Texmerge Index\n";
        for (const auto& asset : mxec.atable.arefs)
        {
            stream << asset.ID << ",";
            stream << stringmanip::sjisToUtf8(asset.asset_filepath) << ",";
            stream << stringmanip::sjisToUtf8(asset.asset_filename) << ",";
            stream << asset.mergefile_index << ",";
            stream << asset.texmerge_file << ",";
            stream << asset.texmerge_index << "\n";
        }
    }
}


void unpackGroupsTable(const MXE& mxe, std::filesystem::path& root_path)
{
    auto& mxec = mxe.mxen.mxec;
    if (mxec.unknowntable1.entries.size())
    {
        // Create offset -> parameter -> entity lookups
        std::unordered_map<uint64_t, size_t> o2p;
        for (int i=0; i < mxec.ptable.psets.size(); ++i)
            o2p.insert({mxec.ptable.psets[i].data_offset, i});
        // std::unordered_map<uint64_t, size_t> p2e;
        // for (int i=0; i < mxe.entities.size(); ++i)
        //     p2e.insert({mxe.entities[i].components.back().parameter_set_ids.front(), i});
        
        std::filesystem::path group_path = root_path/"groups";
        safe_create_dir(group_path);

        for (int i=0; i < mxec.unknowntable1.entries.size(); ++i)
        {
            const auto& unk = mxec.unknowntable1.entries[i];
            std::ofstream stream = safe_create_textfile(group_path/(std::to_string(i) + ".csv"));
            stream << "Parameter ID\n";
            for (const auto& o : unk.data_1)
            {
                if (!o2p.count(o))
                {
                    std::cout << "WARNING: Dropping offset " << o << " in group file " << i << ".csv because it does not point to a parameter set.\n";
                    exit(1);
                }
                else
                {
                    stream << o2p.at(o) << "\n";
                }
            }
        }
    }
}
