#pragma once

#include "TOC.hpp"
#include <GinnungagapCore/utils/StringUtils.hpp>

class AssetsTable
{
public:
    AssetsTableTableOfContents toc;
    std::vector<AssetRef> arefs;
    std::vector<uint32_t> asset_uses;
public:
    template<typename Parser>
    void exbip_rw(Parser& rw, uint64_t& asset_table_offset, MXECStringBank& sbank)
    {
        if (rw.section_exists(asset_table_offset, arefs.size() + asset_uses.size()))
        {
            rw.align(rw.tell(), 0x10);
            rw.verify_and_set_stream_offset(asset_table_offset, "Assets Table");

            rw.rw_obj(toc);

            rw.verify_and_set_stream_offset(toc.asset_refs_offset, "Assets Table Asset Refs");
            rw.rw_objs(arefs, toc.asset_refs_count, sbank);
            rw.verify_and_set_stream_offset(toc.asset_uses_offset, "Assets Table Asset Uses");
            rw.rw_uint32s(asset_uses, toc.asset_uses_count);

            rw.align(rw.tell(), 0x08);
        }
    }
};
