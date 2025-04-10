#pragma once

#include "TOC.hpp"
#include <GinnungagapCore/utils/StringUtils.hpp>

struct EntityTable
{
public:
    EntityTableTableOfContents toc;
    std::vector<EntityRecord> entities;
public:
    template<typename Parser>
    void exbip_rw(Parser& rw, uint64_t& offset, MXECStringBank& sbank)
    {
        // Entities table
        if (rw.section_exists(offset, entities.size()))
        {
            rw.verify_and_set_stream_offset(offset, "Entity Table");

            rw.rw_obj(toc);

            rw.verify_and_set_stream_offset(toc.entries_offset, "Entity Table entries");
            rw.rw_objs(entities, toc.entry_count, sbank);
            

            for (auto& e : entities)
            {
                rw.verify_and_set_stream_offset(e.data_offset, "Entity data offset");
                rw.rw_objs(e.components, e.component_count, sbank);

                for (auto& c : e.components)
                {
                    if (rw.section_exists(c.parameter_sets_offset, c.parameter_sets_count))
                    {
                        rw.verify_and_set_stream_offset(c.parameter_sets_offset, "Parameter Set IDs");
                        rw.rw_uint32s(c.parameter_set_ids, c.parameter_sets_count);
                    }
                }
                rw.align(rw.tell(), 0x08);
            }
        }
        rw.align(rw.tell(), 0x08);
    }
};
