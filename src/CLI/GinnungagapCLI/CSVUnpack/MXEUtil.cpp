#include "MXEInfo.hpp"
#include "CSVFile.hpp"
#include "MXEUtil.hpp"

void addEntityToCSVUnpack(const std::filesystem::path& root_path, const std::string& entity_name, const FlatEntityDefMap& entity_defmap)
{
	const std::filesystem::path params_root = root_path / "parameters";
	const std::filesystem::path metadata_path = root_path / "metadata.txt";
	const std::filesystem::path entities_root = root_path / "entities";
	const std::filesystem::path entity_csv = entities_root / (entity_name + ".csv");

	if (!doesFileExist(metadata_path))
	{
		std::cout << "Error: Not an MXE unpack. File " << metadata_path.string() << " does not exist." << std::endl;
		exit(1);
	}
	// Entities may not exist, e.g. game_info.mxe
	if (!doesDirectoryExist(entities_root))
	{
		std::cout << "'entities' directory does not exist in MXE unpack" 
			      << "Add it if you know what you're doing"
				  << std::endl;
		return;
	}
	// Params may not exist... in theory
	if (!doesDirectoryExist(entities_root))
	{
		std::cout << "''parameters' directory does not exist in MXE unpack."
			      << "Add it if you know what you're doing."
			      << std::endl;
		return;
	}
	// Given entity CSV may not exist
	if (!doesFileExist(entity_csv))
	{
		std::cout << "Entity CSV '"<< entity_csv <<"' does not exist in MXE unpack."
			      << "Add it if you know what you're doing."
			      << std::endl;
		return;
	}
	// Make a list of param CSVs and check they exist
	std::vector<std::pair<std::filesystem::path, uint32_t>> params;
	params.reserve(5);

	if (!entity_defmap.contains(entity_name))
	{
		std::cout << "Unknown entity type: " << entity_name << std::endl;
		return;
	}
	const auto& edef = entity_defmap.at(entity_name);
	uint32_t next_param_id = nextAvailableParamID(root_path);
	for (const auto& comp : edef.components)
		for (const auto& p : comp.parameters) {
			//build csv and ID list to use
			params.push_back(std::pair(params_root / (p.param_type + ".csv"), next_param_id));
			if (!doesFileExist(params.back().first))
			{
				std::cout << "Param CSV '" << params.back().first << "' does not exist in MXE unpack."
					<< "Add it if you know what you're doing."
					<< std::endl;
				return;
			}
			next_param_id++;
		}
	// Get new entity ID and write it out
	uint32_t entityID = nextAvailableEntityID(root_path);
	std::ofstream ent_file(entity_csv.string(), std::ios::app);
	try 
	{
		std::cout << "Writing new entity dummy to CSV: " << entity_csv;
		// Assume it is a proper csv and pray
		ent_file << entityID << "," << "new_" << entityID << ",0,";
		// We already iterated over parameters and generated IDs
		for (int i = 0; i < params.size(); ++i) {
			ent_file << params.at(i).second;
			if (i < params.size() - 1)
				ent_file << ",";
		}
		ent_file << "\n";
		ent_file.close();
		std::cout << " - Complete." << std::endl;
	}
	catch (std::exception e) {
		std::cout << std::endl << "Error writing CSV: " << entity_csv << std::endl;
		ent_file.close();
		return;
	}
	// Now add params
	for(const auto& v : params)
	{
		addParamToCSVUnpack(v.first, v.second);
	}
}

void addParamToCSVUnpack(const std::filesystem::path& param_csv, uint32_t param_id)
{
	// read CSV header and count commas :)
	std::ifstream param_file(param_csv, std::ios::in);
	std::string header;
	std::getline(param_file, header);
	int count = 0;
	for (int i = 0; i < header.size(); ++i)
		if (header.at(i) == ',')
			count++;
	param_file.close();
	
	//now print ID, Name, fill the rest with -1
	std::ofstream param_file_out(param_csv, std::ios::app);
	try
	{
		std::cout << "Writing new param dummy to CSV: " << param_csv;
		param_file_out << param_id << ",";
		param_file_out << "new_" << param_id << ",";
		param_file_out << "dummy Type string,";
		for (int i = 2; i < count; ++i)
		{
			param_file_out << "-1";
			if (i < count - 1)
				param_file_out << ",";
		}
		param_file_out << "\n";
		param_file_out.close();
		std::cout << " - Complete." << std::endl;
	}
	catch (std::exception e) {
		std::cout << std::endl << "Error writing CSV: " << param_csv << std::endl;
		param_file_out.close();
		return;
	}
}
