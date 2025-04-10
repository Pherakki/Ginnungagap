#pragma once

#include <cinttypes>


struct UnknownTable1TableOfContents
{
public:
    uint32_t unknown_0x00;
    uint32_t entry_count;
    uint64_t entries_offset;
public:
    template<typename Parser>
    void exbip_rw(Parser& rw)
    {
        rw.rw_uint32(this->unknown_0x00);
        rw.template rw_padding<0x04>();
        rw.rw_uint32(this->entry_count);
        rw.template rw_padding<0x04>();

        rw.rw_offset64(this->entries_offset);
        rw.template rw_padding<0xA0>();
    }
};
