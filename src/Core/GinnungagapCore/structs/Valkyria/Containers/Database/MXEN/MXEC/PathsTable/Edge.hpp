#pragma once

#include <cinttypes>
#include <vector>

struct PathEdge
{
public:
    uint32_t start_node_idx;
    uint32_t end_node_idx;
    uint32_t param_count;
    
    uint64_t params_offset;
    
    std::vector<uint32_t> param_ids;
public:
    template<typename Parser>
    void exbip_rw(Parser& rw)
    {
        rw.rw_uint32(this->start_node_idx);
        rw.rw_uint32(this->end_node_idx);
        rw.rw_uint32(this->param_count);
        rw.template rw_padding<0x04>();

        rw.rw_offset64(this->params_offset);
    }

    template<typename Parser>
    void rw_param_ids(Parser& rw)
    {
        if (rw.section_exists(this->params_offset, this->param_count))
        {
            rw.verify_and_set_stream_offset(this->params_offset);
            rw.rw_uint32s(this->param_ids, this->param_count);
        }
    }
};
