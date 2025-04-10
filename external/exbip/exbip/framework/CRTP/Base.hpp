#pragma once

#include <exbip/_detail/DefineDetailMacros.hpp>


BEGIN_EXBIP_NAMESPACE

// 'Implementation' is the concrete class that closes the CRTP loop.
// 'Descriptor' is a tag to disambiguate downcast calls in Descriptor Mixins.
template<typename Implementation, typename... Tags>
class CRTPBase
{
protected:
    const Implementation* downcast() const noexcept { return static_cast<const Implementation*>(this); }
          Implementation* downcast()       noexcept { return static_cast<      Implementation*>(this); }
    

    template<class Descriptor, typename... Args>
    decltype(auto) staticDispatchDescriptor(Args&&... args)
    {
        return this->downcast()->template callDescriptor<Descriptor>(*this->downcast(), args...);
    }
};

END_EXBIP_NAMESPACE
