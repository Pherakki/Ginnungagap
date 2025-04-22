#include "CSVUnpack/Unpack.hpp"
#include "CSVUnpack/Pack.hpp"
#include "CSVUnpack/MXEInfo.hpp"
#include "CSVUnpack/MXEUtil.hpp"
#include "GinnungagapCore/utils/Paths.hpp"
#include "GinnungagapCore/structs/Valkyria/Filetypes/MXE/BinaryVC4.hpp"
#include "GinnungagapCore/structs/Valkyria/Containers/Database/MXEN/MXEC/ParametersTable/ParamDefinitions/Load.hpp"
#include <iostream>


class CmdParser
{
public:
    CmdParser(int argc, char** argv)
    {
        // Mode
        {
            if (argc < 2)
            {
                std::cout << "Error: no arguments passed." << std::endl;
                CmdParser::printUsage();
                exit(1);
            }

            std::string mode = std::string(argv[1]);
            if      (mode == "-u")       this->unpack = true;
            else if (mode == "-p")       this->pack   = true;
            else if (mode == "--unpack") this->unpack = true;
            else if (mode == "--pack")   this->pack   = true;
            else if (mode == "--next-id")
            {
                this->next_id = true;
                if (argc < 3)
                {
                    std::cout << "Error: Missing input directory" << std::endl;
                    CmdParser::printUsage();
                    exit(1);
                }
                this->src_dir = std::filesystem::path(argv[2]);
                return;
            }
            else if (mode == "--next-entity-id")
            {
                this->next_entity_id = true;
                if (argc < 3)
                {
                    std::cout << "Error: Missing input directory" << std::endl;
                    CmdParser::printUsage();
                    exit(1);
                }
                this->src_dir = std::filesystem::path(argv[2]);
                return;
            }
            else if (mode == "--add-entity")
            {
                this->add_entity = true;
                if (argc < 4)
                {
                    std::cout << "Error: Missing input directory or entity type" << std::endl;
                    CmdParser::printUsage();
                    exit(1);
                }
                this->src_dir = std::filesystem::path(argv[2]);
                this->ent_type = std::string(argv[3]);
                return;
            }
            else
            {
                std::cout << "Error: invalid mode '" << mode << "'" << std::endl;
                CmdParser::printUsage();
                exit(1);
            }
        }

        // Recursive + Terse
        int arg_offset = 0;
        {
            if (argc < 3)
            {
                if (this->unpack)
                    std::cout << "Error: Missing input file" << std::endl;
                else
                    std::cout << "Error: Missing input directory" << std::endl;
                CmdParser::printUsage();
                exit(1);
            }

            std::string next_arg = std::string(argv[2]);
            if (next_arg == "-r")
            {
                this->recursive = true;
                next_arg = std::string(argv[3]);
                if (next_arg == "-t")
                {
                    this->terse = true;
                    arg_offset = 2;
                }
                else
                {
                    arg_offset = 1;
                }
            }
            else if (next_arg == "-t")
            {
                this->terse = true;
                next_arg = std::string(argv[3]);
                if (next_arg == "-r")
                {
                    this->recursive = true;
                    arg_offset = 2;
                }
                else
                {
                    arg_offset = 1;
                }
            }
        }

        // Input location
        {
            if (argc < (3 + arg_offset))
            {
                if (this->unpack)
                    std::cout << "Error: Missing input file" << std::endl;
                else
                    std::cout << "Error: Missing input directory" << std::endl;
                CmdParser::printUsage();
                exit(1);
            }
            this->src_dir = std::filesystem::path(argv[2 + arg_offset]);
        }

        // Output directory
        {
            if (argc < (4 + arg_offset))
            {
                std::cout << "Error: Missing output directory" << std::endl;
                CmdParser::printUsage();
                exit(1);
            }
            this->dest_dir = std::filesystem::path(argv[3 + arg_offset]);
        }
    }

    static void printUsage()
    {
        std::cout << "Usage: <-u/-p/--unpack/--pack/--next-id/--next-entity-id/--add-entity> [-r] [-t] <input file/directory> <output file/directory>\n" << std::endl;
        std::cout << "    -u <input file> <output directory>              : Unpacks a single MXE file to a directory of CSVs, written to <output directory>" << std::endl;
        std::cout << "    --unpack <input file> <output directory>        : Unpacks a single MXE file to a directory of CSVs, written to <output directory>" << std::endl;
        std::cout << "    -u -r <input directory> <output directory>      : Unpacks a directory of MXE files to CSVs, written to <output directory>" << std::endl;
        std::cout << "    --unpack -r <input directory> <output directory>: Unpacks a directory of MXE files to CSVs, written to <output directory>" << std::endl;
        std::cout << "    -p <input directory> <output directory>         : Packs a directory containing CSV files to a single MXE file, written to <output directory>" << std::endl;
        std::cout << "    --pack <input directory> <output directory>     : Packs a directory containing CSV files to a single MXE file, written to <output directory>" << std::endl;
        std::cout << "    -p -r <input directory> <output directory>      : Packs multiple directories containing CSV files to corresponding MXE files, written to <output directory>" << std::endl;
        std::cout << "    --pack -r <input directory> <output directory>  : Packs multiple directories containing CSV files to corresponding MXE files, written to <output directory>" << std::endl;
        std::cout << "    --next-id <input directory>                     : Returns the next available parameter set ID." << std::endl;
        std::cout << "    --next-entity-id <input directory>              : Returns the next available entity ID." << std::endl;
        std::cout << "    --add-entity <input directory> entity-type-name : Add dummy entity and its params to CSVs." << std::endl;
        std::cout << "    Add -t for 'terse mode'; where types are written to CSV as single-character keys." << std::endl;
    }

public:
    bool pack=false;
    bool unpack=false;
    bool recursive=false;
    bool terse=false;
    bool next_id=false;
    bool next_entity_id=false;
    bool add_entity=false;
    std::string ent_type;
    std::filesystem::path src_dir;
    std::filesystem::path dest_dir;
};

void readMXE(MXE& mxe, const std::filesystem::path& filepath, const ParameterDefMap& params_defmap)
{
    
    CanvasReader fr(filepath.string(), std::ios_base::binary);
    auto rw = fr.asLittleEndian();
    std::string peek = rw.peek_bytestring(4);
    if (peek != "0x4d58454e")
    {
        std::cout << "Error: not an MXE file, is " << peek << ". << Aborting." << std::endl;
    }
    rw.rw_obj(mxe, params_defmap);
}

void writeMXE(MXE& mxe, const std::filesystem::path& filepath, const ParameterDefMap& params_defmap)
{
    safe_create_dir(filepath.parent_path().string());
    CanvasWriter writer(filepath, std::ios_base::binary);
    auto writer_rw = writer.asLittleEndian();
    mxe.exbip_rw(writer_rw, params_defmap);
}

void packMXE(MXE& mxe, const std::filesystem::path& filepath, const ParameterDefMap& params_defmap, const FlatEntityDefMap& flat_entity_defmap)
{
    mxe.mxen.mxec.ptable.toc.entry_count          = 0;
    mxe.mxen.mxec.etable.toc.entry_count          = 0;
    mxe.mxen.mxec.ptable.toc.entry_count          = 0;
    mxe.mxen.mxec.atable.toc.asset_refs_count     = 0;
    mxe.mxen.mxec.atable.toc.asset_uses_count     = 0;
    mxe.mxen.mxec.unknowntable1.toc.entry_count   = 0;
    mxe.mxen.mxec.utoc2.entry_count               = 0;
    mxe.mxen.mxec.utoc2.entries_offset            = 0;
    mxe.mxen.mxec.toc.parameter_sets_table_offset = 0;
    mxe.mxen.mxec.toc.entity_table_offset         = 0;
    mxe.mxen.mxec.toc.paths_table_offset          = 0;
    mxe.mxen.mxec.toc.asset_table_offset          = 0;
    mxe.mxen.mxec.toc.texmerge_offsets_offset     = 0;
    mxe.mxen.mxec.toc.mergefile_offset            = 0;
    mxe.mxen.mxec.toc.unknown_table_1_offset      = 0;
    mxe.mxen.mxec.toc.unknown_table_2_offset      = 0;

    mxe.mxen.header.container_type = 'NEXM';
    mxe.mxen.mxec.header.container_type = 'CEXM';
    mxe.mxen.header.flags.EXTENDED_HEADER(true);
    mxe.mxen.mxec.header.flags.EXTENDED_HEADER(true);
    mxe.mxen.mxec.header.depth = 1;
    mxe.mxen.mxec.header.header_size = 0x20;
    mxe.mxen.mxec.toc.content_flags = 0x101;
    
    packMetadata(mxe, filepath);
    packAssetsTable    (mxe, filepath);
    packParametersTable(mxe, params_defmap, filepath); // This needs to calculate the TexMerge offset
    packEntitiesTable  (mxe, flat_entity_defmap, filepath); // This needs to ignore cells if the component is not there
    packPathsTable     (mxe, filepath);
    cleanupAssetsTable (mxe);
    packGroupsTable    (mxe, filepath, mxe.mxen.mxec.utoc2.exists);
    
    // Rebuild entity component parameter names
    for (const auto& entity : mxe.mxen.mxec.etable.entities)
    {
        const auto& entity_def = flat_entity_defmap.at(entity.components.back().name);
        for (int c_idx=0; c_idx < entity.components.size(); ++c_idx)
        {
            const auto& component_def = entity_def.components[c_idx];
            auto& component = entity.components[entity.components.size()-c_idx-1];
            for (int p_idx=0; p_idx < component_def.parameters.size(); ++p_idx)
            {
                const auto& param_def = component_def.parameters[p_idx];
                auto param_id = component.parameter_set_ids[p_idx];
                auto& param   = mxe.mxen.mxec.ptable.psets[param_id];
                
                param.name = component_def.full_name + "!@" + stringmanip::utf8ToSjis(param_def.member_type) + "@" + param_def.param_type + ":" + entity.name;
            }
        }
    }
    
    {
        CanvasOffsetCalculator oc;
        auto oc_rw = oc.asLittleEndian();
        mxe.exbip_rw(oc_rw, params_defmap);
    }

    mxe.mxen.mxec.pof1.from_container(mxe.mxen.mxec, params_defmap);
    mxe.mxen.mxec.enrs.from_container(mxe.mxen.mxec, params_defmap);
    mxe.mxen.mxec.ccrs.from_container(mxe.mxen.mxec, params_defmap);
    mxe.mxen.mxec.eofc.from_container(mxe.mxen.mxec, params_defmap);
    mxe.mxen.eofc.from_container(mxe.mxen, params_defmap);
    mxe.eofc.from_container(mxe.mxen); // This is a bit odd; doesn't the final EOFC usually have 0x10 header..?
    {
        CanvasOffsetCalculator oc;
        auto oc_rw = oc.asLittleEndian();
        mxe.exbip_rw(oc_rw, params_defmap);
    }
    std::sort(mxe.mxen.mxec.atable.asset_uses.begin(), mxe.mxen.mxec.atable.asset_uses.end());
    // EOFC depths are weird????
    mxe.eofc.header.depth = 0;
    mxe.mxen.eofc.header.depth = 2;
    mxe.mxen.mxec.eofc.header.depth = 0;
}


int main(int argc, char** argv)
{
    CmdParser config(argc, argv);


    ParameterDefMap params_defmap;
    EntityDefMap entity_defmap;
    //do not die silently if config jsons have errors
    try 
    {
        loadParameterDefinitions(params_defmap, getexepath().parent_path() / "assets" / "definitions" / "VC4" / "Parameters");
        loadEntityDefinitions(entity_defmap, getexepath().parent_path() / "assets" / "definitions" / "VC4" / "Entities");
        
    }
    catch (const nlohmann::json::parse_error& e) {
        std::cout << "assets/definitions/VC4 JSON parsing error: " << e.what() << std::endl
                  << "exception id: " << e.id << std::endl
                  << "byte position of error: " << e.byte << std::endl;
        return 1;
    }

    FlatEntityDefMap flat_entity_defmap = flattenEntityDefinitions(entity_defmap);

    if (config.next_id)
    {
        std::cout << "Next ID: " << nextAvailableParamID(config.src_dir) << std::endl;
        return 0;
    }
    if (config.next_entity_id)
    {
        std::cout << "Next entity ID: " << nextAvailableEntityID(config.src_dir) << std::endl;
        return 0;
    }
    if (config.add_entity)
    {
        addEntityToCSVUnpack(config.src_dir, config.ent_type, flat_entity_defmap);
        return 0;
    }

    if (config.unpack)
    {
        if (config.recursive)
        {
            if (!doesDirectoryExist(config.src_dir))
            {
                std::cout << config.src_dir << " is not a directory. Aborting." << std::endl;
                exit(1);
            }

            auto files = getFiles(config.src_dir);
            for (const auto& f : files)
            {
                if (!doesFileExist(f))
                    continue;
                
                std::string ext = f.extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c){ return std::tolower(c); });
                if (ext != ".mxe")
                {
                    std::cout << "Skipping file without .mxe extension " << f << std::endl;
                    continue;
                }
                
                std::filesystem::path inpath  = f;
                std::filesystem::path outpath = config.dest_dir / f.filename().stem().string();
                std::cout << "Unpacking " << f << " to " << outpath << "..." << std::endl;
                MXE mxe;
                readMXE(mxe, inpath, params_defmap);
                unpackMXEToCSV(mxe, params_defmap, flat_entity_defmap, outpath.string(), config.terse);
            }
        }
        else
        {
            if (!doesFileExist(config.src_dir))
            {
                std::cout << config.src_dir << " is not a file. Aborting." << std::endl;
                exit(1);
            }
            
            std::filesystem::path inpath  = config.src_dir;

            std::string ext = inpath.extension().string();
            std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return std::tolower(c); });
            if (ext != ".mxe")
            {
                std::cout << "Error: input file does not have an .mxe extension " << inpath << std::endl;
                exit(0);
            }

            std::filesystem::path outpath = config.dest_dir / config.src_dir.filename().stem().string();
            std::cout << "Unpacking " << inpath << " to " << outpath << "..." << std::endl;
            MXE mxe;
            readMXE(mxe, inpath, params_defmap);
            unpackMXEToCSV(mxe, params_defmap, flat_entity_defmap, outpath.string(), config.terse);
        }
    }
    else if (config.pack)
    {
        if (config.recursive)
        {
            if (!doesDirectoryExist(config.src_dir))
            {
                std::cout << config.src_dir << " is not a directory. Aborting." << std::endl;
                exit(1);
            }
            
            auto directories = getFiles(config.src_dir);
            for (const auto& f : directories)
            {
                if (!doesDirectoryExist(f))
                    continue;

                if (!doesFileExist(f / "metadata.txt"))
                    continue;

                std::filesystem::path inpath = f;
                std::filesystem::path outpath = config.dest_dir / (f.filename().stem().string() + ".mxe");
                std::cout << "Packing " << inpath << " to " << outpath << "..." << std::endl;
                MXE mxe;
                packMXE(mxe, inpath, params_defmap, flat_entity_defmap);
                writeMXE(mxe, outpath, params_defmap);
            }
        }
        else
        {
            if (!doesDirectoryExist(config.src_dir))
            {
                std::cout << config.src_dir << " is not a directory. Aborting." << std::endl;
                exit(1);
            }

            if (!doesFileExist(config.src_dir / "metadata.txt"))
            {
                std::cout << "Input directory does not contain a metadata.txt file. Aborting." << std::endl;
                exit(1);
            }
            
            std::filesystem::path inpath = config.src_dir;
            std::filesystem::path outpath = config.dest_dir / (config.src_dir.filename().stem().string() + ".mxe");
            std::cout << "Packing " << inpath << " to " << outpath << "..." << std::endl;
            MXE mxe;
            packMXE(mxe, inpath, params_defmap, flat_entity_defmap);
            writeMXE(mxe, outpath, params_defmap);
        }
    }

    return 0;
}
