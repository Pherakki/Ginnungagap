#pragma once

#include <exbip/exbip.hpp>
#include "TOC.hpp"
#include "ParameterSet.hpp"


struct ParametersTable
{
public:
    ParametersTableTableOfContents toc;
    std::vector<ParameterSet> psets;
public:
    template<typename Parser>
    void exbip_rw(Parser& rw, uint64_t& table_offset, MXECStringBank& sbank, const ParameterDefMap& params_defmap)
    {
        if (rw.section_exists(table_offset, psets.size()))
        {
            rw.verify_and_set_stream_offset(table_offset, "Parameters Table");
            rw.rw_obj(toc);
            rw.verify_and_set_stream_offset(toc.entries_offset, "Parameters Table Entries");
            rw.rw_objs(psets, toc.entry_count, sbank);
            for (auto& p : psets)
            {
                p.rw_dataset(rw, params_defmap, sbank);
            }
            rw.align(rw.tell(), 0x08);
        }
    }
};