#pragma once

#include <cinttypes>


struct UnknownTable2TableOfContents
{
    uint32_t entry_count; // 0
    uint64_t entries_offset; // 0
    bool exists=false;

    template<typename Parser>
    void exbip_rw(Parser& rw)
    {
        rw.rw_uint32(this->entry_count);
        rw.template rw_padding<0x1C>();

        rw.rw_offset64(this->entries_offset);
        rw.template rw_padding<0xF8>();
    }
};

