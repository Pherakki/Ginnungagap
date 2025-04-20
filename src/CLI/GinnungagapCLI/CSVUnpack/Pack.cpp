#include "Pack.hpp"
#include "CSVFile.hpp"
#include <memory>
#include <vector>
#include <exbip/descriptors/streammarker.hpp>
#include <GinnungagapCore/structs/Valkyria/Containers/Database/MXEN/MXEC/StringBank.hpp>
#include "MXEInfo.hpp"

typedef std::unordered_map<decltype(AssetRef::ID), decltype(AssetRef::filetype)> AssetLookup_t;

template<typename T>
class ShiftedOffsetReference : public exbip::OffsetReference
{
public:
    T& offset;
    int64_t shift;

    ShiftedOffsetReference(T& ref, int64_t shift)
        : offset{ref}
        , shift{shift}
    {}

    void receive(int64_t offset) override 
    { 
        this->offset = static_cast<T>(offset) + shift; 
    }
    
    virtual int64_t getOffset() override { return static_cast<int64_t>(this->offset); };
};


static std::string errorline(size_t column, size_t row, const std::string& filename, const std::string& msg)
{
    return "Column " + std::to_string(column) + " of row " + std::to_string(row) + " in '" + filename + "' " + msg;
}


static std::vector<std::pair<size_t, size_t>> findFilenameOrder(const std::vector<std::filesystem::path>& filepaths)
{
    std::vector<std::pair<size_t, size_t>> ordering;
    for (int filepath_idx=0; filepath_idx < filepaths.size(); ++filepath_idx)
    {
        const auto& filepath = filepaths[filepath_idx];
        const std::string fname = filepath.filename().stem().string();
        size_t csv_order_param;
        try
        {
            csv_order_param = std::stoull(fname);
        }
        catch(const std::runtime_error& err)
        {
            std::cout << "Error: " << filepath.string() << " does not have a filename convertable to an unsigned integer. Aborting." << std::endl;
            exit(1);
        }

        ordering.emplace_back(csv_order_param, filepath_idx);
    }

    std::sort(ordering.begin(), ordering.end(), [](const std::pair<size_t,size_t>& a, const std::pair<size_t,size_t>& b) { return a.first < b.first; });
    return ordering;
}


int32_t extToAssetType(const std::string& file_ext)
{
    if      (file_ext == "hmd") return 1;
    else if (file_ext == "htx") return 2;
    else if (file_ext == "hmt") return 3;
    else if (file_ext == "mcl") return 6;
    else if (file_ext == "mlx") return 8;
    else if (file_ext == "abr") return 9;
    else if (file_ext == "abd") return 10;
    else if (file_ext == "cvd") return 12;
    else if (file_ext == "hst") return 12;
    else if (file_ext == "bhv") return 12;
    else if (file_ext == "hmm") return 13;
    else if (file_ext == "pvs") return 20;
    else if (file_ext == "htf") return 21;
    else if (file_ext == "htr") return 22;
    else if (file_ext == "mmf") return 24;
    else if (file_ext == "mmr") return 25;
    // htf
    // hmm
    else return -1;
}




template<typename T>
T interpretString(const std::string& s, const std::string& err_msg="")
{
    try
    {
        if constexpr(std::is_same_v<T, int8_t>)
        {
            return std::stoi(s);
        }
        else if constexpr(std::is_same_v<T, int16_t>)
            return std::stoi(s);
        else if constexpr(std::is_same_v<T, int32_t>)
            return std::stol(s);
        else if constexpr(std::is_same_v<T, int64_t>)
            return std::stoll(s);
        else if constexpr(std::is_same_v<T, uint8_t>)
            return std::stoul(s);
        else if constexpr(std::is_same_v<T, uint16_t>)
            return std::stoul(s);
        else if constexpr(std::is_same_v<T, uint32_t>)
            return std::stoul(s);
        else if constexpr(std::is_same_v<T, uint64_t>)
            return std::stoull(s);
        else if constexpr(std::is_same_v<T, float>)
        {
            return std::stof(s);
        }
        else if constexpr(std::is_same_v<T, double>)
            return std::stod(s);
        else if constexpr(std::is_same_v<T, std::string>)
            return s;
    }
    catch (std::exception& e)
    {
        if (err_msg.size() > 0)
        {
            std::cout << err_msg << std::endl;
            exit(1);
        }
    }
    return 0;
}

template<typename T>
void interpretString(const std::string& s, T& out, const std::string& err_msg="")
{
    out = interpretString<T>(s, err_msg);
}

std::vector<std::string> splitString(const std::string& s)  
{
    std::vector<std::string> elems;
    std::istringstream       stream(s);
    std::string              tmp;

    while(getline(stream, tmp, ' ')) 
    {
        if (!tmp.empty())
            elems.push_back(tmp);
    }

    return elems;
} 

void setParameterData(const std::string& s, Parameter& elem, const std::string& errmsg)
{
    switch(elem.type)
    {
        case ElementType::Float32:
        {
            elem.data.m_float = interpretString<float>(s, errmsg + "cannot be interpreted as a 32-bit float");
            break;
        }
        case ElementType::Float64:
        {
            elem.data.m_double = interpretString<double>(s, errmsg + "cannot be interpreted as a 64-bit float");
            break;
        }
        case ElementType::Int8:
        {
            elem.data.m_int8 = interpretString<int8_t>(s, errmsg + "cannot be interpreted as an 8-bit signed integer");
            break;
        }
        case ElementType::Int16:
        {
            elem.data.m_int16 = interpretString<int16_t>(s, errmsg + "cannot be interpreted as a 16-bit signed integer");
            break;
        }
        case ElementType::Int32:
        {
            elem.data.m_int32 = interpretString<int32_t>(s, errmsg + "cannot be interpreted as a 32-bit signed integer");
        }
        case ElementType::Int64:
        case ElementType::Asset:
        {
            elem.data.m_int64 = interpretString<int64_t>(s, errmsg + "cannot be interpreted as a 64-bit signed integer");
            break;
        }
        case ElementType::UInt8:
        case ElementType::Pad8:
        {
            elem.data.m_uint8 = interpretString<uint8_t>(s, errmsg + "cannot be interpreted as an 8-bit unsigned integer");
            break;
        }
        case ElementType::UInt16:
        case ElementType::Pad16:
        {
            elem.data.m_uint16 = interpretString<uint16_t>(s, errmsg + "cannot be interpreted as a 16-bit unsigned integer");
            break;
        }
        case ElementType::UInt32:
        case ElementType::Pad32:
        case ElementType::Offset32:
        {
            elem.data.m_uint32 = interpretString<uint32_t>(s, errmsg + "cannot be interpreted as a 32-bit unsigned integer");
            break;
        }
        case ElementType::UInt64:
        case ElementType::Pad64:
        case ElementType::Offset64:
        {
            elem.data.m_uint64 = interpretString<uint64_t>(s, errmsg + "cannot be interpreted as a 64-bit unsigned integer");
            break;
        }
        case ElementType::SJISString:
        {
            elem.string = stringmanip::utf8ToSjis(s);
            break;
        }
        case ElementType::UTF8String:
        {
            elem.string = s;
            break;
        }
        case ElementType::CharColor:
        {
            auto& c = elem.data.m_char4;
            auto vec = splitString(s);
            if (vec.size() != 4)
            {
                std::cout << "Incorrect number of elements in byte color vector: expected 4, found " << vec.size() << " '" << s << "'" << std::endl;
                exit(1);
            }
            else
            {
                for (int idx=0; idx < 4; ++idx)
                    c[idx] = interpretString<uint8_t>(vec[idx], errmsg + "cannot be interpreted as a vector of 4 uint8s");
            }
            break;
        }
        case ElementType::FloatColor:
        {
            auto& c = elem.data.m_float4;
            auto vec = splitString(s);
            if (vec.size() != 4)
            {
                std::cout << "Incorrect number of elements in float color vector: expected 4, found " << vec.size() << " '" << s << "'" << std::endl;
                exit(1);
            }
            else
            {
                for (int idx=0; idx < 4; ++idx)
                    c[idx] = interpretString<float>(vec[idx], errmsg + "cannot be interpreted as a vector of 4 floats");
            }
            break;
        }
        case ElementType::Bit8:
        {
            if (s.substr(0,2) != "0b")
            {
                std::cout << errmsg << "is a bit-string, but does not start with '0b'" << std::endl;
                exit(1);
            }
            elem.data.m_uint8 = stringmanip::fromBitString8(s);
            break;
        }
        case ElementType::Bit16:
        {
            if (s.substr(0,2) != "0b")
            {
                std::cout << errmsg << "is a bit-string, but does not start with '0b'" << std::endl;
                exit(1);
            }
            elem.data.m_uint16 = stringmanip::fromBitString16(s);
            break;
        }
        case ElementType::Bit32:
        {
            if (s.substr(0,2) != "0b")
            {
                std::cout << errmsg << "is a bit-string, but does not start with '0b'" << std::endl;
                exit(1);
            }
            elem.data.m_uint32 = stringmanip::fromBitString32(s);
            break;
        }
        case ElementType::Bit64:
        {
            if (s.substr(0,2) != "0b")
            {
                std::cout << errmsg << "is a bit-string, but does not start with '0b'" << std::endl;
                exit(1);
            }
            elem.data.m_uint64 = stringmanip::fromBitString64(s);
            break;
        }
        case ElementType::Hex8:
        {
            if (s.substr(0,2) != "0x")
            {
                std::cout << errmsg << "is a hex-string, but does not start with '0x'" << std::endl;
                exit(1);
            }
            elem.data.m_uint8 = stringmanip::fromHexString8(s);
            break;
        }
        case ElementType::Hex16:
        {
            if (s.substr(0,2) != "0x")
            {
                std::cout << errmsg << "is a hex-string, but does not start with '0x'" << std::endl;
                exit(1);
            }
            elem.data.m_uint16 = stringmanip::fromHexString16(s);
            break;
        }
        case ElementType::Hex32:
        {
            if (s.substr(0,2) != "0x")
            {
                std::cout << errmsg << "is a hex-string, but does not start with '0x'" << std::endl;
                exit(1);
            }
            elem.data.m_uint32 = stringmanip::fromHexString32(s);
            break;
        }
        case ElementType::Hex64:
        {
            if (s.substr(0,2) != "0x")
            {
                std::cout << errmsg << "is a hex-string, but does not start with '0x'" << std::endl;
                exit(1);
            }
            elem.data.m_uint64 = stringmanip::fromHexString64(s);
            break;
        }
    }
}


static void markAssetRefs(const AssetLookup_t& asset_lookup, const MXEParameterSetDefinition* const param_def, const ParameterSet& pset, std::vector<std::pair<uint32_t, int64_t>>& asset_offsets)
{


    for (int asset_elem_idx=0; asset_elem_idx < param_def->asset_vars.size(); ++asset_elem_idx)
    {
        const auto asset_idx    = param_def->asset_vars   [asset_elem_idx];
        const auto asset_offset = param_def->asset_offsets[asset_elem_idx];

        int64_t asset_id = pset.dataset[asset_idx].data.m_int64;
        //if (asset_id >= 0)
        {
            asset_offsets.emplace_back(pset.ID, asset_offset);
        }

        // Validation
        if (asset_id > -1)
        {
            if (asset_lookup.count(asset_id) == 0)
            {
                std::cout << "Error: Parameter ID " << pset.ID << " (" << stringmanip::sjisToUtf8(pset.name) << ") uses asset ID " << asset_id << ", which is not defined in the assets.csv. Aborting." << std::endl;
                exit(1);
            }

            const auto asset_type = param_def->assets.at(asset_idx);
            auto asset_type_id = extToAssetType(asset_type);
            if (asset_type_id == -1) // Unknown asset type
                return;
            
            if (asset_type_id != asset_lookup.at(asset_id))
            {
                std::cout << "Error: Parameter ID " << pset.ID << " (" << stringmanip::sjisToUtf8(pset.name) << ") uses asset ID " << asset_id << ", which is not a " << asset_type << " file as required by this parameter set. Aborting." << std::endl;
                exit(1);
            }
        }
    }
}

static void markAssetRefsSlgEnGrassParam(const AssetLookup_t& asset_lookup, const MXEParameterSetDefinition* const param_def, const ParameterSet& pset, std::vector<std::pair<uint32_t, int64_t>>& asset_offsets)
{


    for (int asset_elem_idx=0; asset_elem_idx < param_def->asset_vars.size()-1; ++asset_elem_idx)
    {
        const auto asset_idx    = param_def->asset_vars   [asset_elem_idx];
        const auto asset_offset = param_def->asset_offsets[asset_elem_idx];

        int64_t asset_id = pset.dataset[asset_idx].data.m_int64;
        //if (asset_id >= 0)
        {
            asset_offsets.emplace_back(pset.ID, asset_offset);
        }

        // Validation
        if (asset_id > -1)
        {
            if (asset_lookup.count(asset_id) == 0)
            {
                std::cout << "Error: Parameter ID " << pset.ID << " (" << stringmanip::sjisToUtf8(pset.name) << ") uses asset ID " << asset_id << ", which is not defined in the assets.csv. Aborting." << std::endl;
                exit(1);
            }

            const auto asset_type = param_def->assets.at(asset_idx);
            auto asset_type_id = extToAssetType(asset_type);
            if (asset_type_id == -1) // Unknown asset type
                return;
            
            if (asset_type_id != asset_lookup.at(asset_id))
            {
                std::cout << "Error: Parameter ID " << pset.ID << " (" << stringmanip::sjisToUtf8(pset.name) << ") uses asset ID " << asset_id << ", which is not a " << asset_type << " file as required by this parameter set. Aborting." << std::endl;
                exit(1);
            }
        }
    }
    size_t asset_idx    = param_def->asset_vars.back();
    size_t asset_offset = param_def->asset_offsets.back();
    int64_t asset_id = pset.dataset[asset_idx].data.m_int64;

    if (asset_id == -1)
    {
        asset_offsets.emplace_back(pset.ID, asset_offset);
    }

}


void packMetadata(MXE& mxe, const std::filesystem::path& root_path)
{
    const std::filesystem::path metadata_file = root_path/"metadata.txt";
    if (!doesFileExist(metadata_file))
        return;
    
    std::string name_0 = "mxe_flag=";
    std::string name_1 = "unknown_index=";
    std::string name_2 = "has_unknown_table_2=";
    std::string name_3 = "assets_unknown_0x00=";
    std::ifstream stream(metadata_file);
    std::string buf; 
    while (std::getline(stream, buf))
    {
        if (buf.starts_with(name_0))
        {
            std::string unknown_idx_str = buf.substr(name_0.size());
            bool has_flag;
            if (unknown_idx_str == "0")
                has_flag = false;
            else if (unknown_idx_str == "1")
                has_flag = true;
            else
            {
                std::cout << "Unrecognised value for 'mxe_flag' [" + unknown_idx_str + "]: must be 0 or 1" << std::endl;
                exit(1);
            }

            mxe.mxen.header.flags.flag_0(has_flag);
            mxe.mxen.mxec.header.flags.flag_0(has_flag);
        }

        else if (buf.starts_with(name_1))
        {
            std::string unknown_idx_str = buf.substr(name_1.size());
            mxe.mxen.mxec.toc.unknown_count_1 = std::stoi(unknown_idx_str);
        }

        else if (buf.starts_with(name_2))
        {
            bool has_unk_table_2;
            std::string has_unk_table_2_str = buf.substr(name_2.size());
            if (has_unk_table_2_str == "0")
                has_unk_table_2 = false;
            else if (has_unk_table_2_str == "1")
                has_unk_table_2 = true;
            else
            {
                std::cout << "Unrecognised value for 'has_unk_table_2' [" + has_unk_table_2_str + "]: must be 0 or 1" << std::endl;
                exit(1);
            }

            mxe.mxen.mxec.utoc2.exists = has_unk_table_2;
        }

        else if (buf.starts_with(name_3))
        {
            std::string asset_unk0x00 = buf.substr(name_3.size());
            mxe.mxen.mxec.atable.toc.unknown_0x00 = std::stoi(asset_unk0x00);
        }
    }
}

void packParametersTable(MXE& mxe, const ParameterDefMap& params_defmap, const std::filesystem::path& root_path)
{
    AssetLookup_t asset_lookup = {};
    for (const auto& asset : mxe.mxen.mxec.atable.arefs)
    {
        asset_lookup[asset.ID] = asset.filetype;
    }

    auto& ptable = mxe.mxen.mxec.ptable;
    ptable.psets.clear();
    mxe.mxen.mxec.atable.asset_uses.clear();

    const std::string mxe_name = root_path.stem().string();
    const std::filesystem::path params_root = root_path/"parameters";

    // If there are no parameters, do an early return.
    if (!doesDirectoryExist(params_root))
        return;
    
    // ************************************************************************ //
    // ****** Find all parameter IDs and verify that they are contiguous ****** //
    // ************************************************************************ //
    // This can all be skipped if global IDs can be replaced by per-param-type IDs.
    // First get all parameter IDs
    std::vector<ParameterErrorData> param_IDs = checkParameterTableCSVErrors(params_root);
    ptable.psets.resize(param_IDs.size());

    // ***************************************************** //
    // ****** Read and deserialize each parameter set ****** //
    // ***************************************************** //
    std::vector<uint32_t> texmerge_ids;
    int64_t mergefile_id = -1;
    std::vector<std::pair<uint32_t, int64_t>> asset_offsets;
    for (const auto& f : CSVFile::getCSVFiles(params_root))
    {
        auto csv = CSVFile::fromFile(f);
        std::string param_type = f.filename().stem().string();
        if (!params_defmap.contains(param_type))
        {
            std::cout << "Error: Unknown Parameter type '" << param_type << "'" << std::endl;
            exit(1);
        }

        std::vector<uint32_t> this_type_param_ids;
        const auto& param_definition = params_defmap.at(param_type);
        for (uint32_t row_idx=1; row_idx < csv.rowCount(); ++row_idx)
        {
            uint32_t start_idx;
            const auto& row = csv[row_idx];

            if (row.size() == 0)
                continue;

            const MXEParameterSetDefinition* param_def;
            if (param_definition.hasMultipleDefinitions())
            {
                start_idx = 4;
                // Get variant from CSV, assign variant
                uint64_t variant_size = interpretString<uint64_t>(row[3], "Column 3 of row " + std::to_string(row_idx) + " cannot be interpreted as an unsigned integer");
                param_def = &param_definition.getDefinition(variant_size);
            }
            else
            {
                start_idx = 3;
                // Assign default variant
                param_def = &param_definition.getDefinition(0);
            }

            if (row.size() < start_idx + param_def->data.size())
            {
                std::cout<<"Not enough columns in row " + std::to_string(row_idx) + " for parameter table " + param_type + ", expected " + std::to_string(start_idx + param_def->data.size()) + " values" << std::endl;
                exit(1);
            }
            
            uint32_t param_ID = interpretString<uint32_t>(row[0]);
            this_type_param_ids.push_back(param_ID);
            auto& pset = ptable.psets[param_ID];
            pset.type_name = param_type;

            if (param_type == "MxParameterTextureMerge")
            {
                texmerge_ids.emplace_back(param_ID);
            }

            if (param_type == "MxParameterMergeFile")
            {
                if (mergefile_id == -1)
                {
                    mergefile_id = param_ID;
                }
                else
                {
                    std::cout << "Error: More than 1 MxParameterMergeFile defined. Aborting." << std::endl;
                    exit(1);
                }
            }

            pset.ID    = param_ID;
            pset.name  = param_type + ":" + stringmanip::utf8ToSjis(row[1]);
            for (int idx=0; idx < param_def->data.size(); ++idx)
            {
                const auto& pdef = param_def->data[idx];
                auto& elem = pset.dataset.emplace_back();
                elem.type = pdef.type;
                setParameterData(row[start_idx + idx], elem, errorline(start_idx+idx, row_idx, f.string(), ""));
            }
            pset.size = param_def->size;

            // Mark assets while we're here
            if (param_type == "SlgEnGrassParam")
                markAssetRefsSlgEnGrassParam(asset_lookup, param_def, pset, asset_offsets);
            else
                markAssetRefs(asset_lookup, param_def, pset, asset_offsets);
        }

        // Deal with subparameter packing...
        const std::filesystem::path subparams_dir = params_root / param_type;
        for (const auto& subparam_info : param_definition.fullDefinition().subparams)
        {
            for (uint32_t param_id : this_type_param_ids)
            {
                auto& param = ptable.psets[param_id];
                param.subparameters.emplace(subparam_info.name, std::vector<ParameterSet::Parameters>{});
                // Should have a less messy way to set this, but in practice it only gets called 
                // for a MxParameterStaticLight...
                setParameterData("0", param.dataset[subparam_info.count],  "");
                setParameterData("0", param.dataset[subparam_info.offset], "");
            }

            const std::filesystem::path subparam_filename = subparams_dir / (subparam_info.name + ".csv");
            if (!doesFileExist(subparam_filename))
                continue;
            
            const std::string subparam_type = subparam_info.type;
            if (!params_defmap.contains(subparam_type))
            {
                std::cout << "Error: Unknown Subparameter type '" << param_type << "'. Aborting." << std::endl;
                exit(1);
            }

            const auto& subparam_definition = params_defmap.at(subparam_type);
            auto subparam_csv = CSVFile::fromFile(subparam_filename);
            for (uint32_t row_idx=1; row_idx < subparam_csv.rowCount(); ++row_idx)
            {
                uint32_t start_idx=1;
                const auto& row = subparam_csv[row_idx];

                if (row.size() == 0)
                    continue;

                const MXEParameterSetDefinition& subparam_def = subparam_definition.fullDefinition();

                if (row.size() < start_idx + subparam_def.data.size())
                {
                    std::cout << "Not enough columns in row " + std::to_string(row_idx) + " for parameter table " + param_type + "/" + subparam_info.name + ", expected " + std::to_string(start_idx + subparam_def.data.size()) + " values, found " << row.size() << std::endl;
                    exit(1);
                }
                
                uint32_t parent_id = interpretString<uint32_t>(row[0]);
                if (parent_id >= ptable.psets.size())
                {
                    std::cout << "Parent parameter ID " << parent_id << " found in " << subparam_filename << " exceeds the number of defined parameters" << std::endl;
                }
                auto& pset = ptable.psets[parent_id];

                
                // Check that the ID is the right type
                {
                    bool bad_id = true;
                    for (auto located_id : this_type_param_ids)
                    {
                        if (located_id == parent_id)
                        {
                            bad_id = false;
                            break;
                        }
                    }

                    if (bad_id)
                    {
                        std::cout << "Parent parameter ID " << parent_id << " found in " << subparam_filename << " does not correspond to an ID in " << f << std::endl;
                    }
                }

                auto& subpset = pset.subparameters.at(subparam_info.name).emplace_back();

                for (int idx=0; idx < subparam_def.data.size(); ++idx)
                {
                    const auto& subpdef = subparam_def.data[idx];
                    auto& elem = subpset.emplace_back();
                    elem.type = subpdef.type;
                    setParameterData(row[start_idx + idx], elem, errorline(start_idx+idx, row_idx, subparam_filename.string(), ""));
                }

                // Will need a redesign if we hit a case where assets are referenced inside subparams...
            }
            
            // Now update the subparam counts
            for (uint32_t param_id : this_type_param_ids)
            {
                auto& param = ptable.psets[param_id];
                const auto& subparam_vec = param.subparameters.at(subparam_info.name);
                // Eww...
                setParameterData(std::to_string(subparam_vec.size()), param.dataset[subparam_info.count],  "");
            }
        }
    }

    // Set up texmerge offset calcs
    auto& txo = mxe.mxen.mxec.texmerge_offsets;
    txo.resize(texmerge_ids.size());
    for (int id_idx=0; id_idx < texmerge_ids.size(); ++id_idx)
    {
        auto pset_id = texmerge_ids[id_idx];
        ptable.psets[pset_id].data_marker.subscribe(txo[id_idx]);
    }
    mxe.mxen.mxec.toc.texmerge_count = mxe.mxen.mxec.texmerge_offsets.size();
    if (txo.size() > 0)
        mxe.mxen.mxec.toc.content_flags |= 0x00000400;
                
    // Set up mergefile offset calc
    if (mergefile_id > -1)
    {
        ptable.psets[mergefile_id].data_marker.subscribe(mxe.mxen.mxec.toc.mergefile_offset);
        mxe.mxen.mxec.toc.content_flags |= 0x01000000;
    }
    else
    {
        mxe.mxen.mxec.toc.mergefile_offset = 0;
    }

    // Set up asset use offset calc
    auto& asset_uses = mxe.mxen.mxec.atable.asset_uses;
    asset_uses.resize(asset_offsets.size());
    for (int id_idx=0; id_idx < asset_offsets.size(); ++id_idx)
    {
        ptable.psets[asset_offsets[id_idx].first].data_marker.subscribe<ShiftedOffsetReference<uint32_t>>(asset_uses[id_idx], asset_offsets[id_idx].second + 0x20);
    }
    mxe.mxen.mxec.atable.toc.asset_uses_count = mxe.mxen.mxec.atable.asset_uses.size();

    ptable.toc.unknown_0x00  = 0;
    ptable.toc.unknown_0x04  = 0;
    ptable.toc.entry_count   = ptable.psets.size();
    ptable.toc.unknown_0x0C  = 0;

    ptable.toc.unused_offset = 0;
}


void packEntitiesTable(MXE& mxe, const FlatEntityDefMap& entity_defmap, const std::filesystem::path& root_path)
{
    auto& etable = mxe.mxen.mxec.etable;
    etable.entities.clear();

    const std::filesystem::path entities_root = root_path/"entities";

    // If there are no entities, do an early return.
    if (!doesDirectoryExist(entities_root))
        return;

    // ************************************* //
    // ****** Check for duplicate IDs ****** //
    // ************************************* //
    std::map<uint32_t, uint32_t> entity_IDs;
    auto csv_files = CSVFile::getCSVFiles(entities_root);
    std::vector<std::string> csv_names;
    for (const auto& f : csv_files)
    {
        auto csv = CSVFile::fromFile(f);
        csv_names.push_back(f.filename().stem().string());

        for (int row_idx=1; row_idx < csv.rowCount(); ++row_idx)
        {
            const auto& row = csv[row_idx];

            if (row.size() == 0)
                continue;
            
            entity_IDs[interpretString<uint32_t>(row[0])] += 1;
        }
    }

    std::set<uint32_t> bad_IDs;
    for (auto [ID, count] : entity_IDs)
    {
        if (count > 1)
            bad_IDs.emplace(ID);
    }

    if (!bad_IDs.empty())
    {
        std::map<uint32_t, std::vector<std::pair<uint32_t, uint32_t>>> errors;
        for (int i=0; i < csv_files.size(); ++i)
        {
            auto csv = CSVFile::fromFile(csv_files[i]);

            for (int row_idx=1; row_idx < csv.rowCount(); ++row_idx)
            {
                const auto& row = csv[row_idx];

                if (row.size() == 0)
                    continue;
                
                uint32_t entity_ID = interpretString<uint32_t>(row[0]);
                if (bad_IDs.contains(entity_ID))
                {
                    errors[entity_ID].emplace_back(i, row_idx);
                }
            }
        }

        for (const auto& [ID, offending_entries] : errors)
        {
            std::cout << "Error: Multiple entities have the ID '" << ID << "'\n";
            for (const auto& [csv_idx, row_idx] : offending_entries)
            {
                std::cout << ">> CSV '" << csv_names[csv_idx] << ", row " << row_idx << "\n";
            }
        }

        exit(1);
    }
    
    // ********************************************** //
    // ****** Read and deserialize each entity ****** //
    // ********************************************** //
    for (const auto& f : csv_files)
    {
        auto csv = CSVFile::fromFile(f);
        std::string entity_type = f.filename().stem().string();
        
        if (!entity_defmap.contains(entity_type))
        {
            std::cout << "Error: Unknown Entity type '" << entity_type << "'" << std::endl;
            exit(1);
        }

        const auto& entity_def = entity_defmap.at(entity_type);
        size_t n_components = 0;
        for (const auto& c : entity_def.components)
            n_components += c.parameters.size();

        for (int row_idx=1; row_idx < csv.rowCount(); ++row_idx)
        {
            const auto& row = csv[row_idx];

            if (row.size() == 0)
                continue;

            if (row.size() < 3)
                throw std::runtime_error("Not enough columns in row " + std::to_string(row_idx) + " for entity table " + entity_def.name + " for file " + f.string() + ", expected at least a value for ID, Name, and Controller ID");

            auto& entity = etable.entities.emplace_back();
            uint32_t entity_ID = interpretString<uint32_t>(row[0]);
            entity.ID = entity_ID;
            entity.unknown_0x08 = 0;
            entity.controller_ID = interpretString<uint32_t>(row[2]);
            entity.unknown_0x10 = 0;
            entity.unknown_0x14 = 0;
            entity.unknown_offset_3 = 0;
            entity.unknown_offset_4 = 0;
            entity.unknown_offset_5 = 0;
            entity.name = stringmanip::utf8ToSjis(row[1]);

            const size_t param_ids_begin = 3;
            uint32_t total_param_idx=0;
            std::vector<EntityComponent> tmp_components;
            for (int comp_idx=0; comp_idx < entity_def.components.size(); ++comp_idx)
            {
                const auto& comp_def = entity_def.components[comp_idx];
                auto& component = tmp_components.emplace_back();
                component.name = comp_def.name;

                if (comp_def.parameters.size() > 0)
                {
                    bool is_null_component = true;
                    for (int param_idx=0; param_idx < comp_def.parameters.size(); ++param_idx)
                    {
                        const auto& param_def = comp_def.parameters[param_idx];
                        int32_t param_id = interpretString<int32_t>(row[param_ids_begin + total_param_idx]);
                        if (param_id >= 0)
                        {
                            component.parameter_set_ids.push_back(param_id);
                            if (is_null_component && param_idx > 0)
                            {
                                std::cout << "Error: In column " << param_ids_begin + total_param_idx << ", row " << row_idx << " of " << f.string() << ", encountered a non-negative parameter ID in a component that contains null IDs. Aborting." << std::endl;
                                exit(1);
                            }
                            is_null_component = false;
                        }
                        else
                        {
                            if (!is_null_component)
                            {
                                std::cout << "Error: In column " << param_ids_begin + total_param_idx << ", row " << row_idx << " of " << f.string() << ", encountered a non-negative parameter ID in a component that contains null IDs. Aborting." << std::endl;
                                exit(1);
                            }
                        }
                        total_param_idx++;
                    }

                    if   (is_null_component) tmp_components.pop_back();
                    else                     component.parameter_sets_count = component.parameter_set_ids.size();
                }
                else
                {
                    component.parameter_sets_count = 0;
                }


            }

            // Reserve the component order when saving
            for (auto it=tmp_components.rbegin(); it != tmp_components.rend(); ++it)
                entity.components.push_back(*it);
            entity.component_count = entity.components.size();
        }
    }


    // Entities are sorted by controller ID.
    std::stable_sort(etable.entities.begin(), etable.entities.end(), [](const EntityRecord& a, const EntityRecord&b) { return a.controller_ID < b.controller_ID; });

    etable.toc.unknown_0x00     = 0;
    etable.toc.entry_count      = etable.entities.size();
    etable.toc.unknown_0x08     = 0;
    etable.toc.unknown_0x0C     = 0;
    etable.toc.unknown_offset_2 = 0;
}


void packPathsTable(MXE& mxe, const std::filesystem::path& root_path)
{
    const std::filesystem::path paths_root = root_path/"paths";
    if (!doesDirectoryExist(paths_root))
        return;
    
    const std::filesystem::path names_filepath = paths_root/"names.txt";
    if (!doesFileExist(names_filepath))
    {
        std::cout << "Error: No 'names.txt' file exists in the directory " << names_filepath << std::endl;
        exit(1);
    }

    std::vector<std::string> path_names;
    {
        std::ifstream stream(names_filepath);
        std::string buf;
        while (std::getline(stream, buf))
        {
            path_names.push_back(stringmanip::utf8ToSjis(buf));
        }
    }

    auto& paths_table = mxe.mxen.mxec.gtable;
    paths_table.entries.clear();

    const auto csv_files = CSVFile::getCSVFiles(paths_root);
    const std::vector<std::pair<size_t, size_t>> ordering = findFilenameOrder(csv_files);

    for (const auto& [order_idx, csv_idx] : ordering)
    {
        const auto& f = csv_files[csv_idx];
        auto csv = CSVFile::fromFile(f);
        auto& graph = paths_table.entries.emplace_back();
        
        if (order_idx >= path_names.size())
        {
            std::cout << "Tried to pack the name for graph " << order_idx << " (" << f << "), but there are only " << path_names.size() << " names defined in " << names_filepath << std::endl;
            exit(1);
        }
        
        // Pack graph name and set up node vector.
        graph.name = path_names[order_idx];
        graph.nodes.resize(csv.rowCount() - 1);
        graph.node_count = graph.nodes.size();


        // Identify how many subgraphs exist for this graph.
        // In this process, create lookup tables for node/edge/subgraph IDs to contiguous indices.
        // This is done in case the user doesn't define contiguous IDs themself.
        std::unordered_map<int, int> subgraph_id_lut;
        std::unordered_map<uint32_t, uint32_t> node_param_to_idx;
        std::unordered_map<int, int> edge_id_to_idx;
        {
            std::set<int> edge_ids_set;
            std::set<int> subgraph_ids_set;
            uint32_t node_idx=0;
            for (int row_idx=1; row_idx < csv.rowCount(); ++row_idx)
            {
                auto& row = csv[row_idx];
                if (row.empty())
                    continue;

                if (row.size() < 2)
                {
                    std::cout << "Error: row " << row_idx << " in " << f.string() << " has only 1 column, so a subgraph index for this row cannot be identified. Aborting." << std::endl;
                    exit(1);
                }
                
                uint32_t node_param_ID = interpretString<uint32_t>(row[0], "Unable to interpret the first column of row " + std::to_string(row_idx) + "in " + f.string() + " as an integer node parameter index");
                int subgraph_ID        = interpretString<int>(row[1], "Unable to interpret the second column of row " + std::to_string(row_idx) + "in " + f.string() + " as an integer subgraph index");   
                if (subgraph_ID >= 0)
                    subgraph_ids_set.emplace(subgraph_ID);
                
                if (node_param_to_idx.contains(node_param_ID))
                {
                    std::cout << "Error: row " << row_idx << " in " << f.string() << " defines a node parameter ID (" << row[0] << ") that is defined by a previous now. All parameter IDs must be unique. Aborting." << std::endl;
                    exit(1);
                }
                node_param_to_idx.emplace(node_param_ID, node_idx);


                int n_pairs = (row.size() - 2) / 3; // -2 for skipping the first two columns
                int incomplete_edge = (row.size()-2) % 3;
                while (incomplete_edge)
                {
                    row.push_back("");
                    incomplete_edge--;
                }

                for (int local_edge_idx=0; local_edge_idx < n_pairs; ++local_edge_idx)
                {
                    int col_idx = 2 + local_edge_idx*3 + 1;
                    if (row[col_idx-1].empty() || row[col_idx].empty() || row[col_idx+1].empty())
                    {
                        // Skip empty triplets
                        if (row[col_idx-1].empty() && row[col_idx].empty() && row[col_idx+1].empty())
                            continue;
                        
                        std::cout << "Error: Column " << col_idx-1 << " of row " << row_idx << " in graph CSV " << f.string() << " is the start of a partially-defined edge. Either make this definition completely empty, or fill in all three components of the edge." << std::endl;
                        exit(1); 
                    }

                    int edge_id = interpretString<int>(row[col_idx], "Unable to interpret column " + std::to_string(col_idx) + " of row " + std::to_string(row_idx) + " of " + f.string() + " as an integer");
                    if (edge_ids_set.contains(edge_id))
                    {
                        std::cout << "Error: column " << col_idx << " of row " << row_idx << " in " << f.string() << " defines a edge ID that is already defined (" << edge_id << ")" << std::endl;
                        exit(1);
                    }
                    edge_ids_set.emplace(edge_id);
                }

                node_idx++;
            }

            // Create the edge ID -> idx lookup
            graph.edges.resize(edge_ids_set.size());
            graph.edge_count = graph.edges.size();
            {
                int edge_idx = 0;
                for (int edge_id : edge_ids_set)
                {
                    edge_id_to_idx.emplace(edge_id, edge_idx);
                    edge_idx++;
                }
            }

            // Create the subgraph ID -> idx lookup
            graph.subgraphs.resize(subgraph_ids_set.size());
            graph.subgraph_count = graph.subgraphs.size();
            {
                int subgraph_idx = 0;
                for (int sg_id : subgraph_ids_set)
                {
                    subgraph_id_lut.emplace(sg_id, subgraph_idx);
                    subgraph_idx++;
                }    
            }
        }

        // Now construct the graph data.
        {
            uint32_t node_idx=0;
            for (int row_idx=1; row_idx < csv.rowCount(); ++row_idx)
            {
                const auto& row = csv[row_idx];
                if (row.empty())
                    continue;
                
                // Cyclicity test is, apparently, just whether there are no source and sink nodes.
                auto& node = graph.nodes[node_idx];
                
                // We've already checked these conversions work in the previous loop.
                int n_pairs = (row.size() - 2) / 3; // -2 for skipping the first two columns
                uint32_t node_param_ID = interpretString<uint32_t>(row[0]);
                int      subgraph_ID   = interpretString<int>(row[1]);
                node.param_ID = node_param_ID;
                
                if (subgraph_ID < 0)
                {
                    graph.unused_nodes.push_back(node_idx);
                    graph.unused_node_count += 1;
                    node_idx++;
                    continue;
                }
                
                // Add node to subgraph + update edge properties.
                uint32_t subgraph_idx = subgraph_id_lut.at(subgraph_ID);
                auto& subgraph = graph.subgraphs.at(subgraph_idx);
                subgraph.node_indices.push_back(node_idx);
                for (int local_edge_idx=0; local_edge_idx < n_pairs; ++local_edge_idx)
                {
                    // Extract target node ID + edge ID
                    int col_idx = 2 + local_edge_idx*3;
                    // Already errored out if an edge definition is partially defined, so we can do a quick check now.
                    if (row[col_idx].empty())
                        continue;
                    
                    auto target_node_param_id  = interpretString<uint32_t>(row[col_idx+0], "Unable to interpret column " + std::to_string(col_idx) + " of row " + std::to_string(row_idx) + "in " + f.string() + " as an integer node index");
                    auto edge_id               = interpretString<uint32_t>(row[col_idx+1], "Unable to interpret column " + std::to_string(col_idx) + " of row " + std::to_string(row_idx) + "in " + f.string() + " as an integer node index");
                    
                    // Convert Node + Edge IDs to indices
                    uint32_t target_node_idx = node_param_to_idx.at(target_node_param_id);
                    uint32_t edge_index      = edge_id_to_idx.at(edge_id);
                    subgraph.edge_indices.push_back(edge_index);

                    // Push edge index onto the nodes
                    node.outgoing_edge_idxs.push_back(edge_index);
                    node.outgoing_edges_count++;
                    graph.nodes[target_node_idx].incoming_edge_idxs.push_back(edge_index);
                    graph.nodes[target_node_idx].incoming_edges_count++;

                    // Now get the start and end nodes + parameter indices for the edge.
                    // >> Start + end nodes
                    auto& edge = graph.edges[edge_index];
                    edge.start_node_idx = node_idx;
                    edge.end_node_idx   = target_node_idx;
                    
                    // >> Param indices
                    auto pindex_vec = splitString(row[col_idx+2]);
                    edge.param_ids.resize(pindex_vec.size());
                    for (int edge_param_idx=0; edge_param_idx < pindex_vec.size(); ++edge_param_idx)
                    {
                        edge.param_ids[edge_param_idx] = interpretString<uint32_t>(pindex_vec[edge_param_idx], "Unable to interpret column " + std::to_string(col_idx+1) + " of row " + std::to_string(row_idx) + "in " + f.string() + " as a space-separated list of integers, representing parameter indices for an edge");
                    }
                    edge.param_count = edge.param_ids.size();
                }
                node_idx++;
            }
            graph.unused_node_count = graph.unused_nodes.size();
        }

        // Now figure out source + sink nodes + cyclicity for each subgraph
        for (auto& subgraph : graph.subgraphs)
        {
            for (auto& node_idx : subgraph.node_indices)
            {
                if (graph.nodes[node_idx].incoming_edges_count == 0)
                    subgraph.source_node_indices.push_back(node_idx);
                
                if (graph.nodes[node_idx].outgoing_edges_count == 0)
                    subgraph.sink_node_indices.push_back(node_idx);
            
            }

            std::sort(subgraph.node_indices       .begin(), subgraph.node_indices       .end());
            std::sort(subgraph.edge_indices       .begin(), subgraph.edge_indices       .end());
            std::sort(subgraph.source_node_indices.begin(), subgraph.source_node_indices.end());
            std::sort(subgraph.sink_node_indices  .begin(), subgraph.sink_node_indices  .end());
            subgraph.node_idx_count    = subgraph.node_indices.size();
            subgraph.edge_idx_count    = subgraph.edge_indices.size();
            subgraph.source_node_count = subgraph.source_node_indices.size();
            subgraph.sink_node_count   = subgraph.sink_node_indices.size();

            // This isn't true cyclicity, just how the bool is defined.
            subgraph.is_cyclic = (subgraph.source_node_count + subgraph.sink_node_count) == 0;
        }
    }

    paths_table.toc.unknown_0x00 = 0;
    paths_table.toc.entry_count  = paths_table.entries.size();

}



void packAssetsTable(MXE& mxe, const std::filesystem::path& root_path)
{
    auto& assets_table = mxe.mxen.mxec.atable;
    const std::filesystem::path assets_file = root_path/"assets.csv";
    if (!doesFileExist(assets_file))
    {
        assets_table.toc.asset_uses_count = 0;
        assets_table.asset_uses.clear();
        return;
    }

    auto csv = CSVFile::fromFile(assets_file);
    assets_table.arefs.clear();

    uint32_t asset_idx=0;
    for (int row_idx=1; row_idx < csv.rowCount(); ++row_idx)
    {
        const auto& row = csv[row_idx];
        if (row.empty())
            continue;
        
        if (row.size() != 6)
        {
            std::cout << "Error: row " << row_idx << " of assets CSV file " << assets_file << " does not have 6 columns. Aborting." << std::endl;
            exit(1);
        }


        auto& aref = assets_table.arefs.emplace_back();
        aref.ID = interpretString<int>(row[0], "Error: column 0, row " + std::to_string(row_idx) + " of " + assets_file.string() + " cannot be interpreted as an integer");
        aref.asset_filepath = stringmanip::utf8ToSjis(row[1]);
        aref.asset_filename = stringmanip::utf8ToSjis(row[2]);
        aref.mergefile_index = interpretString<int>(row[3], "Error: column 3, row " + std::to_string(row_idx) + " of " + assets_file.string() + " cannot be interpreted as an integer");
        aref.texmerge_file   = interpretString<int>(row[4], "Error: column 4, row " + std::to_string(row_idx) + " of " + assets_file.string() + " cannot be interpreted as an integer");
        aref.texmerge_index  = interpretString<int>(row[5], "Error: column 5, row " + std::to_string(row_idx) + " of " + assets_file.string() + " cannot be interpreted as an integer");
        aref.unknown_offset = 0;
        aref.flags =  0x100 * (aref.texmerge_index > -1) +  0x200 * (aref.mergefile_index > -1);

        // Figure out file type
        std::string file_ext = stringmanip::rsection<stringmanip::SectionSide::Right>(row[2], ".");
        std::transform(file_ext.begin(), file_ext.end(), file_ext.data(), [](char c){ return std::tolower(c); });
        int32_t filetype = extToAssetType(file_ext);

        if(filetype == -1)
        {
            std::cout << "Error: Unknown asset file extension '" << file_ext << "'. Aborting." << std::endl;
            exit(1);
        }
        else
        {
            aref.filetype = static_cast<uint32_t>(filetype);
        }
    }
}


void cleanupAssetsTable(MXE& mxe)
{
    auto& assets_table = mxe.mxen.mxec.atable;
    // The asset uses have been handled in the parameter set table generator.    
    assets_table.toc.asset_refs_count = assets_table.arefs.size();
    if (assets_table.toc.asset_refs_count == 0)
    {
        assets_table.toc.asset_uses_count = 0;
        assets_table.asset_uses.clear();
    }
}

void packGroupsTable(MXE& mxe, const std::filesystem::path& root_path, bool has_table_2)
{
    auto& unktable1 = mxe.mxen.mxec.unknowntable1;
    unktable1.entries.clear();

    unktable1.toc.unknown_0x00 = 0;
    unktable1.toc.entry_count  = 0;
    unktable1.toc.entries_offset = 0;

    const std::filesystem::path groups_root = root_path/"groups";
    if (!doesDirectoryExist(groups_root))
        return;
    

    const auto csv_files = CSVFile::getCSVFiles(groups_root);
    const std::vector<std::pair<size_t, size_t>> ordering = findFilenameOrder(csv_files);

    size_t count = 0;
    for (const auto& [_, csv_idx] : ordering)
    {
        const auto& f = csv_files[csv_idx];
        auto csv = CSVFile::fromFile(f);
        auto& param_group = unktable1.entries.emplace_back();
        
        // Find param IDs
        std::vector<uint32_t> IDs;
        for (int row_idx=1; row_idx < csv.rowCount(); ++row_idx)
        {
            const auto& row = csv[row_idx];
            if (row.empty())
                continue;
            
            uint32_t param_ID = interpretString<uint32_t>(row[0]);
            IDs.push_back(param_ID);
        }
        
        // Now resize + set up offsets
        param_group.data_1.resize(IDs.size());
        for (int id_idx=0; id_idx < IDs.size(); ++id_idx)
        {
            const auto param_ID = IDs[id_idx];
            auto& offset = param_group.data_1[id_idx];
            mxe.mxen.mxec.ptable.psets[param_ID].data_marker.subscribe(offset);
        }

        if (has_table_2)
            param_group.data_2.resize(param_group.data_1.size());

        param_group.unknown_0x00 = 0;
        param_group.unknown_0x04 = 0;
        //param_group.group_id = csv_idx;
        param_group.group_id = count;
        param_group.count_1 = param_group.data_1.size();
        param_group.count_2 = param_group.data_2.size();
        count++;
    }

    unktable1.toc.entry_count = unktable1.entries.size();
}
