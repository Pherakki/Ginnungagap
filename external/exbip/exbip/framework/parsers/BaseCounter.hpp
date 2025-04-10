#pragma once
#include <iostream>
#include <sstream>

#include <exbip/_detail/DefineDetailMacros.hpp>
#include "BaseVirtualStreamParser.hpp"


BEGIN_EXBIP_NAMESPACE

template<class ParserImplementation, template<class> typename... Descriptors>
class ICounterBaseView : public IStreamParserBaseView<ParserImplementation, ICounterBaseView<ParserImplementation, Descriptors...>, Descriptors...>
{
public:
    using IStreamParserBaseView<ParserImplementation, ICounterBaseView<ParserImplementation, Descriptors...>, Descriptors...>::IStreamParserBaseView;
public:
    template<class Descriptor, typename... Args>
    static decltype(auto) callDescriptor(Args&&... args)
    {
        return Descriptor::count(std::forward<Args>(args)...);
    }
};


template<class Implementation, template<class> typename LEMixins, template<class> typename BEMixins>
class ICounterBase : public IVirtualStreamParserBase<Implementation, ICounterBaseView<Implementation, LEMixins>, ICounterBaseView<Implementation, BEMixins>>
{
public:
    using IVirtualStreamParserBase<Implementation, ICounterBaseView<Implementation, LEMixins>, ICounterBaseView<Implementation, BEMixins>>::IVirtualStreamParserBase;
};

END_EXBIP_NAMESPACE
