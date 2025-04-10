#pragma once

#include <GinnungagapCore/structs/Valkyria/serialization/Structs/Container.hpp>


struct EOFC : public BaseContainerVC4<EOFC>
{
public:
    std::string payload;
public:
    template<typename T>
    void valk_rw(T& rw)
    {
        rw.rw_bytestring(payload, header.body_size);
    }

    template<typename T>
    void rw_subcontainers(T& rw)
    {}

    void from_container()
    {
        header.container_type = 'CFOE';
        header.header_size = 0x10;
        header.flags.EXTENDED_HEADER(false);
        header.depth = 0;

        this->header.body_size = 0;
        this->header.size      = 0;
        this->payload          = "";
    }

    template<typename T, typename... Args>
    void from_container(T& ctr, Args&&... args)
    {
        header.container_type = 'CFOE';
        header.header_size = 0x20;
        header.flags.EXTENDED_HEADER(true);
        header.depth = ctr.header.depth + 1;

        this->header.body_size = 0;
        this->header.size      = 0;
        this->payload          = "";
    }
};
