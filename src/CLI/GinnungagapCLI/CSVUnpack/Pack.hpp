#pragma once

#include <exbip/exbip.hpp>
#include <GinnungagapCore/structs/Valkyria/Filetypes/MXE/BinaryVC4.hpp>
#include <filesystem>

//void packCSVtoMXE(MXE& mxe, const ParameterDefMap& params_defmap, const FlatEntityDefMap& entity_defmap, const std::string& path);
void packMetadata(MXE& mxe, const std::filesystem::path& root_path);
void packParametersTable(MXE& mxe, const ParameterDefMap&  params_defmap, const std::filesystem::path& root_path);
void packEntitiesTable  (MXE& mxe, const FlatEntityDefMap& entity_defmap, const std::filesystem::path& root_path);
void packPathsTable     (MXE& mxe, const std::filesystem::path& root_path);
void packAssetsTable    (MXE& mxe, const std::filesystem::path& root_path);
void cleanupAssetsTable (MXE& mxe);
void packGroupsTable    (MXE& mxe, const std::filesystem::path& root_path, bool has_unknown_table_2);
