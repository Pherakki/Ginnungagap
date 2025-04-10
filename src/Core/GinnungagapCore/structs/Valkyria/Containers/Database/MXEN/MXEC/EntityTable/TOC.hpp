#pragma once

#include <cinttypes>
#include <string>
#include <vector>
#include "../StringBank.hpp"


struct EntityTableTableOfContents
{
    uint32_t unknown_0x00; // 0
    uint32_t entry_count;
    uint32_t unknown_0x08; // 0
    uint32_t unknown_0x0C; // 0
        
    uint64_t entries_offset;
    uint64_t unknown_offset_2; // Unused

    template<typename Parser>
    void exbip_rw(Parser& rw)
    {
        rw.rw_uint32(this->unknown_0x00);
        rw.rw_uint32(this->entry_count);
        rw.rw_uint32(this->unknown_0x08);
        rw.rw_uint32(this->unknown_0x0C);

        // The next 16 bytes are unused
        rw.template rw_padding<0x10>();
        rw.rw_offset64(this->entries_offset);
        rw.rw_offset64(this->unknown_offset_2);

        // The next 32 bytes are unused
        rw.template rw_padding<0x20>();
    }
};


struct EntityComponent
{
    uint32_t parameter_sets_count;
    uint32_t padding_0x04=0;
    uint64_t name_offset;
    uint64_t parameter_sets_offset;

    std::string name;
    std::vector<uint32_t> parameter_set_ids;

    template<typename Parser>
    void exbip_rw(Parser& rw, MXECStringBank& sbank)
    {
        rw.rw_uint32(this->parameter_sets_count);
        rw.rw_uint32(this->padding_0x04);
        rw.rw_memoized_string64(this->name_offset, this->name, sbank);
        rw.rw_offset64(this->parameter_sets_offset);
    }
};


struct EntityRecord
{
    uint32_t ID;
    uint32_t component_count;
    uint32_t unknown_0x08;
    uint32_t controller_ID;
    uint32_t unknown_0x10;
    uint32_t unknown_0x14;
    uint64_t name_offset;
    uint64_t data_offset;
    uint64_t unknown_offset_3;
    uint64_t unknown_offset_4;
    uint64_t unknown_offset_5;

    std::string name;
    std::vector<EntityComponent> components;

    template<typename Parser>
    void exbip_rw(Parser& rw, MXECStringBank& sbank)
    {
        rw.rw_uint32(this->ID);
        rw.rw_uint32(this->component_count);
        rw.rw_uint32(this->unknown_0x08);
        rw.rw_uint32(this->controller_ID);
        rw.rw_uint32(this->unknown_0x10);
        rw.rw_uint32(this->unknown_0x14);
        rw.rw_memoized_string64(this->name_offset, this->name, sbank);
        rw.rw_offset64(this->data_offset);
        rw.rw_offset64(this->unknown_offset_3);
        rw.rw_offset64(this->unknown_offset_4);
        rw.rw_offset64(this->unknown_offset_5);
 
        // Next 0x10 bytes are empty
        rw.template rw_padding<0x10>();
    }
};
