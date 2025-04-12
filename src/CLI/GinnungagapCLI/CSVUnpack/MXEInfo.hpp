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

std::vector<ParameterErrorData> checkParameterTableCSVErrors(const std::string& params_root);
std::vector<ParameterErrorData> checkParameterTableCSVErrors(const std::filesystem::path& params_root);

uint32_t nextAvailableID(const std::filesystem::path& root_path);