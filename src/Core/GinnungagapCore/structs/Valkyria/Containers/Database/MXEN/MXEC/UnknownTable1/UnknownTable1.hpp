#pragma once

#include <cinttypes>

#include "TOC.hpp"
#include "Record.hpp"


struct UnknownTable1
{
public:
    UnknownTable1TableOfContents toc;
    std::vector<UnknownRecord>   entries;
public:
    template<typename Parser>
    void exbip_rw(Parser& rw, uint64_t offset)
    {
        rw.rw_obj(toc);

        if (rw.section_exists(toc.entries_offset, toc.entry_count))
        {
            rw.verify_and_set_stream_offset(toc.entries_offset, "Unknown Table 1: Entries offset");
            rw.rw_objs(entries, toc.entry_count);

            int entry_idx=0;
            for (auto& e : entries)
            {
                e.rw_data1(rw);
                e.rw_data2(rw);
                entry_idx++;
            }
        }
    }
};
