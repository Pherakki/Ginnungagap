#pragma once

#include <cinttypes>
#include <vector>


struct PathNode
{
public:
    uint32_t param_ID;
    uint32_t outgoing_edges_count;
    uint32_t incoming_edges_count;
    
    uint64_t outgoing_edges_offset;
    uint64_t incoming_edges_offset;
    
    std::vector<uint32_t> outgoing_edge_idxs;
    std::vector<uint32_t> incoming_edge_idxs;
public:
    template<typename Parser>
    void exbip_rw(Parser& rw)
    {
        rw.rw_uint32(this->param_ID);
        rw.rw_uint32(this->outgoing_edges_count);
        rw.rw_uint32(this->incoming_edges_count);
        rw.template rw_padding<0x04>();

        rw.rw_offset64(this->outgoing_edges_offset);
        rw.rw_offset64(this->incoming_edges_offset);
    }

    template<typename Parser>
    void rw_outgoing_edges(Parser& rw)
    {
        if (rw.section_exists(this->outgoing_edges_offset, this->outgoing_edges_count))
        {
            rw.verify_and_set_stream_offset(this->outgoing_edges_offset);
            rw.rw_uint32s(this->outgoing_edge_idxs, this->outgoing_edges_count);
        }
    }

    template<typename Parser>
    void rw_incoming_edges(Parser& rw)
    {
        if (rw.section_exists(this->incoming_edges_offset, this->incoming_edges_count))
        {
            rw.verify_and_set_stream_offset(this->incoming_edges_offset);
            rw.rw_uint32s(this->incoming_edge_idxs, this->incoming_edges_count);
        }
    }
};