#pragma once

#include <exbip/framework/CRTP/Base.hpp>


BEGIN_EXBIP_NAMESPACE

struct ObjectDescriptor
{
    template<typename Parser, typename T, typename... Args>
    static decltype(auto) deserialize(Parser& rw, T& obj, Args&&... args)
    {
        return obj.exbip_rw(rw, std::forward<Args>(args)...);
    }

    template<typename Parser, typename T, typename... Args>
    static decltype(auto) serialize(Parser& rw, T& obj, Args&&... args)
    {
        return obj.exbip_rw(rw, std::forward<Args>(args)...);
    }

    template<typename Parser, typename T, typename... Args>
    static decltype(auto) count(Parser& rw, T& obj, Args&&... args)
    {
        return obj.exbip_rw(rw, std::forward<Args>(args)...);
    }
};

template<typename Implementation>
struct IObjectMixin : public CRTPBase<Implementation, IObjectMixin<Implementation>>
{
    template<typename T, typename... Args>
    decltype(auto) rw_obj(T& obj, Args&&... args) 
    {
        this->downcast()->template rw_descriptor<ObjectDescriptor>(obj, args...);
    }
};


struct ObjectVectorDescriptor
{
    template<typename Parser, typename T, typename... Args>
    static decltype(auto) deserialize(Parser& rw, std::vector<T>& vec, size_t count, Args&&... args)
    {
        vec.resize(count);
        for (int i=0; i < count; ++i)
            rw.rw_obj(vec[i], std::forward<Args>(args)...);
    }

    template<typename Parser, typename T, typename... Args>
    static decltype(auto) serialize(Parser& rw, std::vector<T>& vec, size_t count, Args&&... args)
    {
        for (int i=0; i < vec.size(); ++i)
            rw.rw_obj(vec[i], std::forward<Args>(args)...);
    }

    template<typename Parser, typename T, typename... Args>
    static decltype(auto) count(Parser& rw, std::vector<T>& vec, size_t count, Args&&... args)
    {
        for (int i=0; i < vec.size(); ++i)
            rw.rw_obj(vec[i], std::forward<Args>(args)...);
    }
};


template<typename Implementation>
struct IObjectVectorMixin : public CRTPBase<Implementation, IObjectVectorMixin<Implementation>>
{

    template<typename T, typename... Args>
    decltype(auto) rw_objs(std::vector<T>& vec, size_t count, Args&&... args) 
    {
         this->downcast()->template rw_descriptor<ObjectVectorDescriptor>(vec, count, std::forward<Args>(args)...); 
    }

    template<typename T, typename... Args>
    decltype(auto) rw_objs(const std::vector<T>& vec, size_t count, Args&&... args) 
    {
         this->downcast()->template rw_descriptor<ObjectVectorDescriptor>(vec, count, std::forward<Args>(args)...); 
    }

};

END_EXBIP_NAMESPACE
