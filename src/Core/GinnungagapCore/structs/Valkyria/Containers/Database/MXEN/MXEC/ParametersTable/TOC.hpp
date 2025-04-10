#pragma once

#include <exbip/exbip.hpp>


struct ParametersTableTableOfContents
{
    uint32_t unknown_0x00; // 0
    uint32_t unknown_0x04; // 0
    uint32_t entry_count;
    uint32_t unknown_0x0C; // 0
        
    uint64_t entries_offset;
    uint64_t unused_offset; // Unused

    template<typename Parser>
    void exbip_rw(Parser& rw)
    {
        rw.rw_uint32(this->unknown_0x00);
        rw.rw_uint32(this->unknown_0x04);
        rw.rw_uint32(this->entry_count);
        rw.rw_uint32(this->unknown_0x0C);

        // The next 0x10 bytes are unused
        rw.template rw_padding<0x10>();
        rw.rw_offset64(this->entries_offset);
        rw.rw_offset64(this->unused_offset);

        // The next 0x30 bytes are unused
        rw.template rw_padding<0x30>();
    }
};
