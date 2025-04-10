#pragma once

#include <fstream>
#include <sstream>

#include "BaseStreamParser.hpp"


BEGIN_EXBIP_NAMESPACE

template<class Held, template<class> typename... Descriptors>
class IWriterBaseView : public IStreamParserBaseView<Held, IWriterBaseView<Held, Descriptors...>, Descriptors...>
{
public:
    using IStreamParserBaseView<Held, IWriterBaseView<Held, Descriptors...>, Descriptors...>::IStreamParserBaseView;
public:
    template<class Descriptor, typename... Args>
    static decltype(auto) callDescriptor(Args&&... args)
    {
        return Descriptor::serialize(std::forward<Args>(args)...);
    }
};

template<class StreamType, class Implementation, template<class> typename LEMixins, template<class> typename BEMixins>
class IWriterBase : public IStreamParserBase<StreamType, Implementation, 
                                             IWriterBaseView<Implementation, LEMixins>, 
                                             IWriterBaseView<Implementation, BEMixins>>
{
public:
    using LittleEndianView = IWriterBaseView<Implementation, LEMixins>;
    using BigEndianView    = IWriterBaseView<Implementation, BEMixins>;
    using IStreamParserBase<StreamType, Implementation, LittleEndianView, BigEndianView>::IStreamParserBase;
    size_t globalTell()              { return this->stream.tellp() ;}
    void   globalSeek(size_t offset) { this->stream.seekp(offset); }
    
};

template<class Implementation, template<class> typename LEMixins, template<class> typename BEMixins>
using IMemWriterBase = IWriterBase<std::stringstream, Implementation, LEMixins, BEMixins>;

template<class Implementation, template<class> typename LEMixins, template<class> typename BEMixins>
using IFileWriterBase = IWriterBase<std::ofstream, Implementation, LEMixins, BEMixins>;

END_EXBIP_NAMESPACE
