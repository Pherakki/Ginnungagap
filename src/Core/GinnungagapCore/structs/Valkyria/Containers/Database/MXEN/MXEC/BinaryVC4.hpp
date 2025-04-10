#pragma once

#include <GinnungagapCore/structs/Valkyria/serialization/Structs/Container.hpp>
#include <GinnungagapCore/structs/Valkyria/Containers/Metadata/POF1/BinaryVC4.hpp>
#include <GinnungagapCore/structs/Valkyria/Containers/Metadata/ENRS/BinaryVC4.hpp>
#include <GinnungagapCore/structs/Valkyria/Containers/Metadata/CCRS/BinaryVC4.hpp>
#include <GinnungagapCore/structs/Valkyria/Containers/Metadata/EOFC/BinaryVC4.hpp>
#include "ParametersTable/ParametersTable.hpp"
#include "ParametersTable/ParamDefinitions/Definition.hpp"
#include "EntityTable/EntityTable.hpp"
#include "PathsTable/PathsTable.hpp"
#include "AssetsTable/AssetsTable.hpp"
#include "UnknownTable1/UnknownTable1.hpp"
#include "UnknownTable2/UnknownTable2.hpp"
#include "StringBank.hpp"

struct MXECTableOfContents
{
    uint32_t content_flags;
    uint32_t unknown_count_1;
    uint32_t texmerge_count;
    uint32_t padding_0x2C;
    
    uint32_t padding_0x30;
    uint32_t padding_0x34;
    uint32_t padding_0x38;
    uint32_t padding_0x3C;
    
    uint64_t parameter_sets_table_offset;
    uint64_t entity_table_offset;
    
    uint64_t asset_table_offset;
    uint64_t paths_table_offset;
    
    uint64_t texmerge_offsets_offset;
    uint64_t padding_0x68;
    
    uint64_t mergefile_offset;
    uint64_t unknown_table_1_offset;
    
    uint64_t unknown_table_2_offset;

    template<typename Parser>
    void exbip_rw(Parser& rw)
    {
        rw.rw_uint32(this->content_flags);
        rw.rw_uint32(this->unknown_count_1);
        rw.rw_uint32(this->texmerge_count);
        rw.rw_uint32(this->padding_0x2C);
        rw.rw_uint32(this->padding_0x30);
        rw.rw_uint32(this->padding_0x34);
        rw.rw_uint32(this->padding_0x38);
        rw.rw_uint32(this->padding_0x3C);
        rw.rw_offset64(this->parameter_sets_table_offset);
        rw.rw_offset64(this->entity_table_offset);
        rw.rw_offset64(this->asset_table_offset);
        rw.rw_offset64(this->paths_table_offset);
        rw.rw_offset64(this->texmerge_offsets_offset);
        rw.rw_offset64(this->padding_0x68);
        rw.rw_offset64(this->mergefile_offset);
        rw.rw_offset64(this->unknown_table_1_offset);
        rw.rw_offset64(this->unknown_table_2_offset);
    }
};


struct StringBankDescriptor
{
    template<typename Parser>
    static void deserialize(Parser& rw, ContainerHeader& header, MXECStringBank& sbank)
    {
        // Skip the string bank because it has already been parsed during the file read
        rw.seek(header.body_size);
    }
    
    template<typename Parser>
    static void serialize(Parser& rw, ContainerHeader& header, MXECStringBank& sbank)
    {
        // Sequentially write each string in the bank
        for (auto& [s, marker] : sbank)
            rw.rw_cbytestring(*const_cast<std::string*>(&s));
    }

    template<typename Parser>
    static void count(Parser& rw, ContainerHeader& header, MXECStringBank& sbank)
    {
        // Skip ahead by the size of the string bank
        for (auto& [s, marker] : sbank)
            rw.rw_cbytestring(*const_cast<std::string*>(&s));
    }

    template<typename Parser>
    static void calculateOffsets(Parser& rw, ContainerHeader& header, MXECStringBank& sbank)
    {
        // Record the simulated offset of each string in the bank
        for (const auto& [s, marker] : sbank)
        {
            rw.dispatch_marker(marker);
            rw.rw_cbytestring(*const_cast<std::string*>(&s));
        }
    }
};

struct MXEC : public BaseContainerVC4<MXEC>
{
public:
    MXECTableOfContents toc;
    MXECStringBank sbank;

    ParametersTable ptable;
    EntityTable etable;
    PathsTable gtable;
    AssetsTable atable;
    std::vector<uint64_t> texmerge_offsets;
    UnknownTable1 unknowntable1;
    UnknownTable2TableOfContents utoc2;

    POF1 pof1;
    ENRS enrs;
    CCRS ccrs;
    EOFC eofc;
public:
    template<typename Parser>
    void valk_rw(Parser& rw, const ParameterDefMap& params_defmap)
    {
        rw.rw_obj(toc);
        rw.align(rw.tell(), 0x80);

        rw.rw_obj(ptable, toc.parameter_sets_table_offset, sbank, params_defmap);
        rw.rw_obj(etable, toc.entity_table_offset, sbank);
        rw.rw_obj(gtable, toc.paths_table_offset, sbank);
        rw.align(rw.tell(), 0x08);
        rw.rw_obj(atable, toc.asset_table_offset, sbank);
        
        // Texmerges
        if (rw.section_exists(toc.texmerge_offsets_offset, toc.texmerge_count))
        {
            rw.verify_and_set_stream_offset(toc.texmerge_offsets_offset, "Texmerge");
            rw.rw_offset64s(texmerge_offsets, toc.texmerge_count);
        }
        rw.align(rw.tell(), 0x08);
        
        // Unknown Table 1
        if (rw.section_exists(toc.unknown_table_1_offset, unknowntable1.toc.entry_count) || rw.section_exists(toc.unknown_table_2_offset, utoc2.exists))
        {
            if (rw.section_exists(toc.unknown_table_1_offset, unknowntable1.toc.entry_count))
                rw.verify_and_set_stream_offset(toc.unknown_table_1_offset, "Unknown Table 1");
            rw.rw_obj(unknowntable1, toc.unknown_table_1_offset);
        }

        // Unknown Table 2
        if (rw.section_exists(toc.unknown_table_2_offset, utoc2.exists))
        {
            rw.verify_and_set_stream_offset(toc.unknown_table_2_offset, "Unknown Table 2");
            rw.rw_obj(utoc2);
            utoc2.exists = true;
        }
        rw.align(rw.tell(), 0x10);
        
        // String bank
        rw.template rw_descriptor<StringBankDescriptor>(header, sbank);
        rw.align(rw.tell(), 0x10);
    }

    template<typename Parser>
    void rw_subcontainers(Parser& rw, const ParameterDefMap& params_defmap)
    {
        rw.rw_obj(pof1);
        rw.rw_obj(enrs);
        rw.rw_obj(ccrs);
        rw.rw_obj(eofc);
    }
};