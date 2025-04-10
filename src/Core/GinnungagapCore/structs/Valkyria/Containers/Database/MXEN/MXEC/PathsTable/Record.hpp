#pragma once

#include <cinttypes>
#include <string>

#include "../StringBank.hpp"
#include "Node.hpp"
#include "Edge.hpp"
#include "Subgraph.hpp"


struct PathRecord
{
public:
    uint32_t    node_count;
    uint32_t    edge_count;
    uint32_t    subgraph_count;
    uint32_t    unused_node_count;
    uint64_t    name_offset;
    uint64_t    nodes_offset;
    uint64_t    edges_offset;
    uint64_t    subgraphs_offset;
    uint64_t    unused_nodes_offset;
    std::string name;

    std::vector<PathNode> nodes;
    std::vector<PathEdge> edges;
    std::vector<PathSubgraph> subgraphs;
    std::vector<uint32_t> unused_nodes;
public:
    template<typename Parser>
    void exbip_rw(Parser& rw, MXECStringBank& sbank)
    {
        rw.rw_uint32(this->node_count);
        rw.rw_uint32(this->edge_count);
        rw.rw_uint32(this->subgraph_count);
        rw.rw_uint32(this->unused_node_count);

        rw.rw_memoized_string64(this->name_offset, this->name, sbank);
        rw.rw_offset64(this->nodes_offset);

        rw.rw_offset64(this->edges_offset);
        rw.rw_offset64(this->subgraphs_offset);

        rw.rw_offset64(this->unused_nodes_offset);
        rw.template rw_padding<0x18>();
    }

    template<typename Parser>
    void rw_nodes(Parser& rw)
    {
        if (rw.section_exists(this->nodes_offset, this->node_count))
        {
            rw.verify_and_set_stream_offset(this->nodes_offset, "Nodes");
            rw.rw_objs(this->nodes, this->node_count);
            for (auto& node : this->nodes)
            {
                node.rw_outgoing_edges(rw);
                node.rw_incoming_edges(rw);
            }
            rw.align(rw.tell(), 0x08);
        }
    }

    template<typename Parser>
    void rw_edges(Parser& rw)
    {
        if (rw.section_exists(this->edges_offset, this->edge_count))
        {
            rw.verify_and_set_stream_offset(this->edges_offset, "Edges");
            rw.rw_objs(this->edges, this->edge_count);
            for (auto& edge : this->edges)
            {
                edge.rw_param_ids(rw);
            }
            rw.align(rw.tell(), 0x08);
        }
    }

    template<typename Parser>
    void rw_subgraphs(Parser& rw)
    {
        if (rw.section_exists(this->subgraphs_offset, this->subgraph_count))
        {
            rw.verify_and_set_stream_offset(this->subgraphs_offset, "Subgraphs");
            rw.rw_objs(this->subgraphs, this->subgraph_count);

            for (auto& subgraph : this->subgraphs)
            {
                subgraph.rw_edge_indices(rw);
                subgraph.rw_node_indices(rw);
                subgraph.rw_sink_node_indices(rw);
                subgraph.rw_source_node_indices(rw);
            }

        }
    }

    template<typename Parser>
    void rw_unused_nodes(Parser& rw)
    {
        if (rw.section_exists(this->unused_nodes_offset, this->unused_node_count))
        {
            rw.verify_and_set_stream_offset(this->unused_nodes_offset, "Unused nodes");
            rw.rw_uint32s(this->unused_nodes, this->unused_node_count);
            rw.align(rw.tell(), 0x08);
        }
    }
};
