#pragma once

#include <fstream>
#include <sstream>

#include <exbip/_detail/DefineDetailMacros.hpp>
#include "../CRTP/Base.hpp"
#include <exbip/utils/Endian.hpp>


BEGIN_EXBIP_NAMESPACE


template<typename T>
class OriginLock
{
public:
    OriginLock(T& rw)
      : rw{rw}
      , origin{rw.origin}
    {
        rw.origin = rw.globalTell();
    }
    ~OriginLock()
    {
        rw.origin = this->origin;
    }

    size_t getOrigin()
    {
        return this->origin;
    }
protected:
    T& rw;
    size_t origin;
};


template<class Held, typename Implementation, template<class> typename... Descriptors>
class IStreamParserBaseView : public Descriptors<Implementation>...
{
public:
    IStreamParserBaseView(Held& held)
      : held{held}
    {}

    typename Held::Stream_T&       getBytestream()       noexcept { return this->held.getBytestream(); }
    const typename Held::Stream_T& getBytestream() const noexcept { return this->held.getBytestream(); }
    const size_t                   getOrigin() const noexcept { return this->held.getOrigin(); }

    size_t globalTell()              { return this->held.globalTell() ;}
    size_t tell      ()              { return this->held.tell(); }
    void   globalSeek(size_t offset) { this->held.globalSeek(offset); }
    void   seek      (size_t offset) { this->held.seek(offset); }
    OriginLock<Held> newOrigin() { return this->held.newOrigin(); }
    auto   asLittleEndian() { return this->held.asLittleEndian(); }
    auto   asBigEndian   () { return this->held.asBigEndian();    }
    template<exbip::endian endianness>
    auto   asEndian      () { return this->held.template asEndian<endianness>();    }
    template<class Descriptor, typename... Args>
    decltype(auto) staticDispatchDescriptor(Args&&... args)
    {
        return this->held.staticDispatchDescriptor(args...);
    }
protected:
    Held& held;
};

template<class StreamType, typename Implementation, typename LittleEndianView, typename BigEndianView>
class IStreamParserBase : public CRTPBase<Implementation>
{
public:
    friend OriginLock<Implementation>;
    typedef StreamType Stream_T;

    StreamType&       getBytestream()       noexcept { return this->stream; }
    const StreamType& getBytestream() const noexcept { return this->stream; }
    const size_t      getOrigin()     const noexcept { return this->origin; }
public:
    template<typename... Args>
    IStreamParserBase(Args... args)
      : stream{std::forward<Args>(args)...}
    {}

    IStreamParserBase(StreamType& stream)
        :stream{stream}
    {}
    
    size_t globalTell()              
    {
        static_assert(decltype(this)::globalTell != decltype(this->downcast())::globalTell);
        return this->downcast()->globalTell(); 
    }
    void   globalSeek(size_t offset) 
    { 
        static_assert(decltype(this)::globalSeek != decltype(this->downcast())::globalSeek);
        return this->downcast()->globalSeek(offset);
    }
    size_t tell      ()              { return this->downcast()->globalTell() - this->origin; }
    void   seek      (size_t offset) { this->downcast()->globalSeek(offset + this->origin); }
public:
    OriginLock<Implementation> newOrigin()
    {
        return OriginLock<Implementation>(*this->downcast());
    }

    auto asLittleEndian()
    {
        LittleEndianView rw(*this->downcast());
        return rw;
    }

    auto asBigEndian()
    {
        BigEndianView rw(*this->downcast());
        return rw;
    }

    template<exbip::endian endianness>
    auto asEndian()
    {
        if constexpr (endianness == exbip::endian::little)
            return this->asLittleEndian();
        else if constexpr (endianness == exbip::endian::big)
            return this->asBigEndian();
        else
            return this->asLittleEndian(); // TODO: FIX
            //static_assert(!std::is_same<exbip::endian, exbip::endian>::value, "invalid endianness for asEndian");
    }
protected:
    StreamType stream;
    size_t     origin=0;
};


END_EXBIP_NAMESPACE
