#pragma once

#include <fstream>
#include <sstream>

#include "BaseVirtualStreamParser.hpp"


BEGIN_EXBIP_NAMESPACE

template <typename Descriptor, typename... Args>
concept hasCalculateOffsetsCallable = requires(Descriptor t, Args... args) { { Descriptor::calculateOffsets(std::forward<Args>(args)...) }; };


template<class ParserImplementation, template<class> typename... Descriptors>
class IOffsetCalculatorBaseView : public IStreamParserBaseView<ParserImplementation, IOffsetCalculatorBaseView<ParserImplementation, Descriptors...>, Descriptors...>
{
public:
    using IStreamParserBaseView<ParserImplementation, IOffsetCalculatorBaseView<ParserImplementation, Descriptors...>, Descriptors...>::IStreamParserBaseView;
public:
    template<class Descriptor, typename... Args>
    static decltype(auto) callDescriptor(Args&&... args)
    {
        if constexpr (hasCalculateOffsetsCallable<Descriptor, Args...>)
            return Descriptor::calculateOffsets(std::forward<Args>(args)...);
        else
            return Descriptor::count(std::forward<Args>(args)...);
    }
};


template<class Implementation, template<class> typename LEMixins, template<class> typename BEMixins>
class IOffsetCalculatorBase : public IVirtualStreamParserBase<Implementation, IOffsetCalculatorBaseView<Implementation, LEMixins>, IOffsetCalculatorBaseView<Implementation, BEMixins>>
{
public:
    using IVirtualStreamParserBase<Implementation, IOffsetCalculatorBaseView<Implementation, LEMixins>, IOffsetCalculatorBaseView<Implementation, BEMixins>>::IVirtualStreamParserBase;
};

END_EXBIP_NAMESPACE
