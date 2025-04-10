#pragma once

#include <exbip/framework/CRTP/Base.hpp>
#include <exbip/utils/Offset.hpp>
#include <iostream>
BEGIN_EXBIP_NAMESPACE

struct StreamAlignmentDescriptor
{
    template<typename Parser>
    static void deserialize(Parser& rw, size_t offset, size_t alignment)
    {
        size_t to_read = bytesToAlignment(offset, alignment);

        std::string s;
        s.resize(to_read);
        rw.getBytestream().read(s.data(), to_read);
    }

    template<typename Parser>
    static void serialize(Parser& rw, size_t offset, size_t alignment)
    {
        size_t to_write = bytesToAlignment(offset, alignment);
        std::string s(to_write, 0);
        rw.getBytestream().write(s.data(), to_write);
    }

    template<typename Parser>
    static void count(Parser& rw, size_t offset, size_t alignment)
    {
        rw.getBytestream().advance(bytesToAlignment(offset, alignment));
    }
};


template<typename Implementation>
struct IStreamAlignmentMixin : public CRTPBase<Implementation, IStreamAlignmentMixin<Implementation>>
{
    void align(size_t offset, size_t alignment) { this->downcast()->template rw_descriptor<StreamAlignmentDescriptor>(offset, alignment); }
};

END_EXBIP_NAMESPACE
