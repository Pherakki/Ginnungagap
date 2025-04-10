#pragma once

#include "HeaderFlags.hpp"
#include <exbip/exbip.hpp>

class ContainerHeader
{
public:
    uint32_t    container_type;
    uint32_t    size; // body_size + subcontainer size
    uint32_t    header_size;
    HeaderFlags flags;

    uint32_t depth=0;
    uint32_t body_size=0;
    uint32_t unknown_0x18=0;
    uint32_t unknown_0x1C=0;

    std::string repr()
    {
        std::string res = "Container " + this->get_type() + " Size: " + std::to_string(this->size) + " Header Size: " + std::to_string(this->header_size);
        if (this->flags.EXTENDED_HEADER())
            res += " Depth: " + std::to_string(this->depth) + " Body Size: " + std::to_string(body_size) + " Unk0x18: " + std::to_string(unknown_0x18) + " Unk0x1C: " + std::to_string(unknown_0x1C);
        return res;
    }

    template<typename Parser>
    void exbip_rw(Parser& rw)
    {
        rw.rw_uint32(this->container_type);
        rw.rw_uint32(this->size);
        rw.rw_uint32(this->header_size);
        rw.rw_uint32(this->flags.getRef());

        if (this->flags.EXTENDED_HEADER())
        {
            rw.rw_uint32(this->depth);
            rw.rw_uint32(this->body_size);
            rw.rw_uint32(this->unknown_0x18);
            rw.rw_uint32(this->unknown_0x1C);
        }
    }

    const std::string get_type() const
    {
        char tmp[5];
        tmp[0] = (this->container_type & 0x000000FF) >> 0x00;
        tmp[1] = (this->container_type & 0x0000FF00) >> 0x08;
        tmp[2] = (this->container_type & 0x00FF0000) >> 0x10;
        tmp[3] = (this->container_type & 0xFF000000) >> 0x18;
        tmp[4] = 0;

        return std::string(tmp);
    }

    template<typename Parser>
    void check_header_size(Parser& rw)
    {
        rw.verify_and_set_stream_offset(this->header_size, this->get_type() + " header size");
    }

    template<typename Parser>
    void check_size(Parser& rw)
    {
        rw.verify_and_set_stream_offset(this->size, this->get_type() + " container size");
    }

    template<typename Parser>
    void check_body_size(Parser& rw)
    {
        rw.verify_and_set_stream_offset(this->body_size, this->get_type() + " contents size");
    }
};

using ContainerHeaderVC1 = ContainerHeader;
using ContainerHeaderVC4 = ContainerHeader;
