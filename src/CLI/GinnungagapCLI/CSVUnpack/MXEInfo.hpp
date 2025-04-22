#pragma once

#include <GinnungagapCore/structs/Valkyria/Filetypes/MXE/BinaryVC4.hpp>
#include <filesystem>

struct ParameterErrorData
{
    uint32_t ID;
    uint32_t row;
    uint32_t csv_idx;

    ParameterErrorData(uint32_t ID, uint32_t row, uint32_t csv_idx)
        : ID{ID}
        , row{row}
        , csv_idx{csv_idx}
    {}
};

struct EntityErrorData
{
    uint32_t ID;
    uint32_t row;
    uint32_t csv_idx;

    EntityErrorData(uint32_t ID, uint32_t row, uint32_t csv_idx)
        : ID{ ID }
        , row{ row }
        , csv_idx{ csv_idx }
    {}
};

std::vector<ParameterErrorData> checkParameterTableCSVErrors(const std::string& params_root);
std::vector<ParameterErrorData> checkParameterTableCSVErrors(const std::filesystem::path& params_root);

std::vector<EntityErrorData> checkEntityTableCSVErrors(const std::string& entities_root);
std::vector<EntityErrorData> checkEntityTableCSVErrors(const std::filesystem::path& entities_root);

uint32_t nextAvailableParamID(const std::filesystem::path& root_path);
uint32_t nextAvailableEntityID(const std::filesystem::path& root_path);