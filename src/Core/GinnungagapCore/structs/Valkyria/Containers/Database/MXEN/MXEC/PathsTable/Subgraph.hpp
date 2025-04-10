#pragma once

#include <cinttypes>
#include <vector>


struct PathSubgraph
{
public:
    uint32_t is_cyclic;
    uint32_t node_idx_count;
    uint32_t edge_idx_count;
    uint32_t sink_node_count;
    uint32_t source_node_count;

    uint64_t sink_nodes_offset;
    uint64_t node_idxs_offset;
    uint64_t edge_idxs_offset;
    uint64_t source_nodes_offset;

    std::vector<uint32_t> source_node_indices;
    std::vector<uint32_t> sink_node_indices;
    std::vector<uint32_t> node_indices;
    std::vector<uint32_t> edge_indices;
public:
    template<typename Parser>
    void exbip_rw(Parser& rw)
    {
        rw.rw_uint32(this->is_cyclic);
        rw.rw_uint32(this->node_idx_count);
        rw.rw_uint32(this->edge_idx_count);
        rw.rw_uint32(this->sink_node_count);
        rw.rw_uint32(this->source_node_count);
        rw.template rw_padding<0x0C>();

        rw.rw_offset64(this->sink_nodes_offset);
        rw.rw_offset64(this->node_idxs_offset);
        rw.rw_offset64(this->edge_idxs_offset);
        rw.rw_offset64(this->source_nodes_offset);
    }

    template<typename Parser>
    void rw_sink_node_indices(Parser& rw)
    {
        if (rw.section_exists(this->sink_nodes_offset, this->sink_node_count))
        {
            rw.verify_and_set_stream_offset(this->sink_nodes_offset, "Subgraph Sink Nodes");
            rw.rw_uint32s(this->sink_node_indices, this->sink_node_count);
        }
    }

    template<typename Parser>
    void rw_node_indices(Parser& rw)
    {
        if (rw.section_exists(this->node_idxs_offset, this->node_idx_count))
        {
            rw.verify_and_set_stream_offset(this->node_idxs_offset, "Subgraph Node Indices");
            rw.rw_uint32s(this->node_indices, this->node_idx_count);
        }
    }

    template<typename Parser>
    void rw_edge_indices(Parser& rw)
    {
        if (rw.section_exists(this->edge_idxs_offset, this->edge_idx_count))
        {
            rw.verify_and_set_stream_offset(this->edge_idxs_offset, "Subgraph Edge Indices");
            rw.rw_uint32s(this->edge_indices, this->edge_idx_count);
        }
    }

    template<typename Parser>
    void rw_source_node_indices(Parser& rw)
    {
        if (rw.section_exists(this->source_nodes_offset, this->source_node_count))
        {
            rw.verify_and_set_stream_offset(this->source_nodes_offset, "Subgraph Source Nodes");
            rw.rw_uint32s(this->source_node_indices, this->source_node_count);
        }
    }
};
