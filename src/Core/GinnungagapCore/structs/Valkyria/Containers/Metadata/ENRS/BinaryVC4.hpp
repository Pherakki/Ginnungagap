#pragma once

#include <GinnungagapCore/structs/Valkyria/serialization/Structs/Container.hpp>


struct ENRS : BaseContainerVC4<ENRS>
{
public:
    uint32_t unknown_0x20=0;
    uint32_t num_groups;
    std::string payload;
public:
    template<typename T>
    void valk_rw(T& rw)
    {
        rw.rw_uint32(this->unknown_0x20);
        rw.rw_uint32(this->num_groups);
        rw.template rw_padding<0x08>();
        rw.rw_bytestring(payload, header.body_size - 0x10);
    }

    template<typename T>
    void rw_subcontainers(T& rw)
    {}

    template<typename T, typename... Args>
    void from_container(T& ctr, Args&&... args)
    {
        header.container_type = 'SRNE';
        header.header_size = 0x20;
        header.flags.EXTENDED_HEADER(true);
        header.depth = ctr.header.depth + 1;

        // Replace with ENRS builder
        this->num_groups = 0;
        this->payload = "";
        this->header.body_size = 0x10;
        this->header.size      = 0x10;
    }
};
