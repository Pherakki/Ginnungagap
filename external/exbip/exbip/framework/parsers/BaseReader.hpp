#pragma once

#include <fstream>
#include <sstream>

#include "BaseStreamParser.hpp"


BEGIN_EXBIP_NAMESPACE

template<class Held, template<class> typename... Descriptors>
class IReaderBaseView : public IStreamParserBaseView<Held, IReaderBaseView<Held, Descriptors...>, Descriptors...>
{
public:
    using IStreamParserBaseView<Held, IReaderBaseView<Held, Descriptors...>, Descriptors...>::IStreamParserBaseView;
public:
    template<class Descriptor, typename... Args>
    static decltype(auto) callDescriptor(Args&&... args)
    {
        return Descriptor::deserialize(std::forward<Args>(args)...);
    }
};


template<class StreamType, class Implementation, template<class> typename LEMixins, template<class> typename BEMixins>
class IReaderBase : public IStreamParserBase<StreamType, Implementation, 
                                             IReaderBaseView<Implementation, LEMixins>, 
                                             IReaderBaseView<Implementation, BEMixins>>
{
public:
    using LittleEndianView = IReaderBaseView<Implementation, LEMixins>;
    using BigEndianView    = IReaderBaseView<Implementation, BEMixins>;
    using IStreamParserBase<StreamType, Implementation, LittleEndianView, BigEndianView>::IStreamParserBase;
    size_t globalTell()              { return this->stream.tellg() ;}
    void   globalSeek(size_t offset) { this->stream.seekg(offset); }
    
};

template<class Implementation, template<class> typename LEMixins, template<class> typename BEMixins>
using IMemReaderBase = IReaderBase<std::stringstream, Implementation, LEMixins, BEMixins>;

template<class Implementation, template<class> typename LEMixins, template<class> typename BEMixins>
using IFileReaderBase = IReaderBase<std::ifstream, Implementation, LEMixins, BEMixins>;

END_EXBIP_NAMESPACE
