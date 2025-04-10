#pragma once

#include <cinttypes>
#include <exbip/framework/CRTP/Base.hpp>


BEGIN_EXBIP_NAMESPACE

template<size_t N>
struct StreamPaddingDescriptor
{
    template <typename Parser>
    static void deserialize(Parser& parser)
    {
        std::array<char, N> s;
        parser.getBytestream().read(s.data(), N);
    }

    template <typename Parser>
    static void serialize(Parser& parser)
    {
        std::array<char, N> s{0};
        parser.getBytestream().write(s.data(), N);
    }

    template <typename Parser>
    static void count(Parser& parser)
    {
        parser.getBytestream().advance(N);
    }
};


template <typename Implementation>
struct IStreamPaddingMixin : public CRTPBase<Implementation, IStreamPaddingMixin<Implementation>>
{
    template <size_t N>
    void rw_padding()
    {
        this->downcast()->template rw_descriptor<StreamPaddingDescriptor<N>>();
    }
};


END_EXBIP_NAMESPACE
