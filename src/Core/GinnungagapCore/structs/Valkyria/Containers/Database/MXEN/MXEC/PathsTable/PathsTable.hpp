#pragma once

#include <vector>

#include "TOC.hpp"
#include "Record.hpp"


struct PathsTable
{
public:
    PathsTableTableOfContents toc;
    std::vector<PathRecord>   entries;

public:
    template<typename Parser>
    void exbip_rw(Parser& rw, uint64_t& offset, MXECStringBank& sbank)
    {
        if (rw.section_exists(offset, entries.size()))
        {
            rw.verify_and_set_stream_offset(offset);
            rw.rw_obj(toc);

            rw.verify_and_set_stream_offset(toc.entries_offset);
            rw.rw_objs(entries, toc.entry_count, sbank);

            for (auto& path : this->entries)
            {
                path.rw_nodes(rw);
                path.rw_edges(rw);
                path.rw_subgraphs(rw);
                path.rw_unused_nodes(rw);

                rw.align(rw.tell(), 0x08);
            }
        }
    }
};
