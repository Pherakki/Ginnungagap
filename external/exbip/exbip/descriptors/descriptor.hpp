#pragma once

#include <exbip/framework/CRTP/Base.hpp>


BEGIN_EXBIP_NAMESPACE

template<typename Implementation, typename... Tags>
struct DescriptorDescriptor : public CRTPBase<Implementation, DescriptorDescriptor<Implementation, Tags...>, Tags...>
{
    template<typename Descriptor, typename... Args>
    decltype(auto) rw_descriptor(Args&&... args)
    {
        auto* dc = this->downcast();
        return dc->template callDescriptor<Descriptor>(*dc, std::forward<Args>(args)...);
    }
};

END_EXBIP_NAMESPACE
