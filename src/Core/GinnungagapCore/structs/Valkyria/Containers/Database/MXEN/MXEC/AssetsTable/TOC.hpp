#pragma once

#include <exbip/exbip.hpp>
#include "../StringBank.hpp"


struct AssetsTableTableOfContents
{
    uint32_t unknown_0x00;
    uint32_t asset_refs_count;
    uint32_t asset_uses_count;

    uint64_t asset_refs_offset;
    uint64_t asset_uses_offset;

    template<typename Parser>
    void exbip_rw(Parser& rw)
    {
        rw.rw_uint32(this->unknown_0x00);
        rw.rw_uint32(this->asset_refs_count);
        rw.rw_uint32(this->asset_uses_count);
        rw.template rw_padding<0x14>();

        rw.rw_offset64(this->asset_refs_offset);
        rw.rw_offset64(this->asset_uses_offset);

        rw.template rw_padding<0x20>();
    }
};


struct AssetRef
{
    uint32_t flags;
    uint32_t ID;
    uint32_t filetype;

    int32_t texmerge_index;
    int32_t texmerge_file;
    int32_t mergefile_index;

    uint64_t asset_filepath_offset;
    uint64_t asset_filename_offset;
    uint64_t unknown_offset;

    std::string asset_filepath;
    std::string asset_filename;

    template<typename Parser>
    void exbip_rw(Parser& rw, MXECStringBank& sbank)
    {
        rw.rw_uint32(this->flags);
        rw.rw_uint32(this->ID);
        rw.rw_uint32(this->filetype);
        rw.rw_int32(this->texmerge_index);

        rw.rw_int32(this->texmerge_file);
        rw.rw_int32(this->mergefile_index);
        rw.rw_memoized_string64(this->asset_filepath_offset, this->asset_filepath, sbank);

        rw.rw_memoized_string64(this->asset_filename_offset, this->asset_filename, sbank);
        rw.rw_offset64(this->unknown_offset);

        rw.template rw_padding<0x10>();
    }
};
