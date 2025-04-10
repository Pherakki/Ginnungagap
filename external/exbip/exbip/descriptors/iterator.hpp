#pragma once

#include <exbip/framework/CRTP/Base.hpp>
#include <vector>

BEGIN_EXBIP_NAMESPACE

template<typename T>
struct VectorIteratorDescriptor
{
    template<typename Parser>
    static T& deserialize(Parser& rw, T& value, size_t count) 
    {
        value.resize(count);
        return value;
    }

    template<typename Parser>
    static T& serialize(Parser& rw, T& value, size_t count) 
    {
        return value;
    }

    template<typename Parser>
    static T& count(Parser& rw, T& value, size_t count) 
    {
        return value;
    }
};



template<typename Implementation>
struct IVectorIteratorMixin : public CRTPBase<Implementation, IVectorIteratorMixin<Implementation>>
{
    template<typename T>
    T& iterate_resizable(T& value, size_t count) { return this->downcast()->template rw_descriptor<VectorIteratorDescriptor<T>>(value, count); }
};


END_EXBIP_NAMESPACE