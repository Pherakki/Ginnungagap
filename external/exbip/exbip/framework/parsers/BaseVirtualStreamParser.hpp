#pragma once

#include "BaseStreamParser.hpp"


BEGIN_EXBIP_NAMESPACE

struct VirtualStream
{
public:
    void advance(size_t delta)
    {
        this->offset += delta;
    }

    size_t tell() { return this->offset; }
    void seek(size_t offset) { this->offset = offset; }
protected:
    size_t offset = 0;
};


template<class Implementation, class LittleEndianView, class BigEndianView>
class IVirtualStreamParserBase : public IStreamParserBase<VirtualStream, Implementation, LittleEndianView, BigEndianView>
{
public:
    using IStreamParserBase<VirtualStream, Implementation, LittleEndianView, BigEndianView>::IStreamParserBase;
    size_t globalTell()              { return this->stream.tell() ;}
    void   globalSeek(size_t offset) { this->stream.seek(offset); }
    
};


END_EXBIP_NAMESPACE