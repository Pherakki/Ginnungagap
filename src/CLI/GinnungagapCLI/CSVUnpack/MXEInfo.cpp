#include "MXEInfo.hpp"
#include "CSVFile.hpp"

#include <iostream>


std::vector<ParameterErrorData> checkParameterTableCSVErrors(const std::string& params_root)
{
    std::vector<ParameterErrorData> param_IDs;
    std::vector<std::string> csv_names;
    for (const auto& f : CSVFile::getCSVFiles(params_root))
    {
        auto csv = CSVFile::fromFile(f);

        for (uint32_t row_idx=1; row_idx < csv.rowCount(); ++row_idx)
        {
            const auto& row = csv[row_idx];
            uint32_t val;

            if (!row.size())
                continue;
            
            try
            {
                val = std::stoul(row[0]);
            }
            catch (const std::exception& e)
            {
                std::cout << "ID value '" + row[0] + "' for row " + std::to_string(row_idx) + " of CSV file '" + f.string() + "' cannot be interpreted as an unsigned integer" << std::endl;
                exit(1);
            }
            param_IDs.emplace_back(val, row_idx, (uint32_t)csv_names.size());
        }
        csv_names.push_back(f.filename().string());
    }

    // Sort into ascending order
    std::sort(param_IDs.begin(), param_IDs.end(), [](const ParameterErrorData& a, const ParameterErrorData& b) { return a.ID < b.ID; });

    // Now check if the list contains a contiguous range of IDs:
    // Check for both duplicates (same number consecutively) and gaps (missing int)
    std::map<uint32_t, std::set<uint32_t>> duplicate_ids;
    std::vector<uint32_t> missing_ids;
    for (int i=1; i < param_IDs.size(); ++i)
    {
        auto idx = param_IDs[i-1].ID;
        auto next_idx = param_IDs[i].ID;

        // Check for duplicates
        if (idx == next_idx)
        {
            duplicate_ids[idx].emplace(i-1);
            duplicate_ids[idx].emplace(i);
        }

        // Check for missing
        idx++;
        while (idx < next_idx)
        {
            missing_ids.push_back(idx);
            idx++;
        }
    }

    // Report errors
    if (duplicate_ids.size())
    {
        for (const auto& [id, indices] : duplicate_ids)
        {
            std::cout << "Error: Parameter ID '" << id << "' is used by multiple parameter sets:\n";
            for (auto idx : indices)
            {
                const auto& param_ref = param_IDs[idx];
                std::cout << "[" << csv_names[param_ref.csv_idx] << "], row " << param_ref.row << std::endl;
            }
        }
    }
    if (missing_ids.size())
    {
        for (auto& id : missing_ids)
        {
            std::cout << "Error: Parameter IDs are not a contiguous series. Missing ID: " << id << std::endl;
        }
    }
    if (!duplicate_ids.empty() || !missing_ids.empty())
        exit(1);
    
    return param_IDs;
}

std::vector<ParameterErrorData> checkParameterTableCSVErrors(const std::filesystem::path& params_root)
{
    return checkParameterTableCSVErrors(params_root.string());
}


std::vector<EntityErrorData> checkEntityTableCSVErrors(const std::string& entities_root)
{
    std::vector<EntityErrorData> entity_IDs;
    std::vector<std::string> csv_names;
    for (const auto& f : CSVFile::getCSVFiles(entities_root))
    {
        auto csv = CSVFile::fromFile(f);

        for (uint32_t row_idx = 1; row_idx < csv.rowCount(); ++row_idx)
        {
            const auto& row = csv[row_idx];
            uint32_t val;

            if (!row.size())
                continue;

            try
            {
                val = std::stoul(row[0]);
            }
            catch (const std::exception& e)
            {
                std::cout << "ID value '" + row[0] + "' for row " + std::to_string(row_idx) + " of CSV file '" + f.string() + "' cannot be interpreted as an unsigned integer" << std::endl;
                exit(1);
            }
            entity_IDs.emplace_back(val, row_idx, (uint32_t)csv_names.size());
        }
        csv_names.push_back(f.filename().string());
    }

    // Sort into ascending order
    std::sort(entity_IDs.begin(), entity_IDs.end(), [](const EntityErrorData& a, const EntityErrorData& b) { return a.ID < b.ID; });

    // Now check if the list contains duplicate IDs:
    std::map<uint32_t, std::set<uint32_t>> duplicate_ids;
    for (int i = 1; i < entity_IDs.size(); ++i)
    {
        auto idx = entity_IDs[i - 1].ID;
        auto next_idx = entity_IDs[i].ID;

        // Check for duplicates
        if (idx == next_idx)
        {
            duplicate_ids[idx].emplace(i - 1);
            duplicate_ids[idx].emplace(i);
        }
    }

    // Report errors
    if (duplicate_ids.size())
    {
        for (const auto& [id, indices] : duplicate_ids)
        {
            std::cout << "Error: Parameter ID '" << id << "' is used by multiple parameter sets:\n";
            for (auto idx : indices)
            {
                const auto& param_ref = entity_IDs[idx];
                std::cout << "[" << csv_names[param_ref.csv_idx] << "], row " << param_ref.row << std::endl;
            }
        }
    }
    
    if (!duplicate_ids.empty())
        exit(1);

    return entity_IDs;
}

std::vector<EntityErrorData> checkEntityTableCSVErrors(const std::filesystem::path& params_root)
{
    return checkEntityTableCSVErrors(params_root.string());
}


uint32_t nextAvailableParamID(const std::filesystem::path& root_path)
{
    const std::filesystem::path params_root   = root_path/"parameters";
    const std::filesystem::path metadata_path = root_path/"metadata.txt";

    if (!doesFileExist(metadata_path))
    {
        std::cout << "Error: Not an MXE unpack. File " << metadata_path.string() << " does not exist." << std::endl;
        exit(1);
    }

    // If there are no parameters, do an early return.
    if (!doesDirectoryExist(params_root))
    {
        return 0;
    }
    
    return checkParameterTableCSVErrors(params_root).size();
}

uint32_t nextAvailableEntityID(const std::filesystem::path& root_path)
{
    const std::filesystem::path entities_root = root_path / "entities";
    const std::filesystem::path metadata_path = root_path / "metadata.txt";

    if (!doesFileExist(metadata_path))
    {
        std::cout << "Error: Not an MXE unpack. File " << metadata_path.string() << " does not exist." << std::endl;
        exit(1);
    }

    // Entities may not exist, e.g. game_info.mxe
    if (!doesDirectoryExist(entities_root))
    {
        return 0;
    }

    return checkEntityTableCSVErrors(entities_root).back().ID+1;
}
