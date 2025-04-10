#pragma once

#include <exbip/exbip.hpp>
#include "../Containers/Database/MXEN/MXEC/StringBank.hpp"


class Offset32Descriptor
{
public:
    template<typename Parser>
    static void deserialize(Parser& rw, uint32_t& offset)
    {
        rw.rw_uint32(offset);
    }

    template<typename Parser>
    static void serialize(Parser& rw, uint32_t& offset)
    {
        rw.rw_uint32(offset);
    }

    template<typename Parser>
    static void count(Parser& rw, uint32_t& offset)
    {
        rw.rw_uint32(offset);
    }

    template<typename Parser>
    static void pof0(Parser& rw, uint32_t& offset)
    {
        rw.register_offset(offset);
        rw.rw_uint32(offset);
    }
};

template <typename Implementation>
class IOffset32Mixin : public exbip::CRTPBase<Implementation, IOffset32Mixin<Implementation>>
{
public:
    void rw_offset32(uint32_t& offset) { this->downcast()->template rw_descriptor<Offset32Descriptor>(offset); }
};


class Offset64Descriptor
{
public:
    template<typename Parser>
    static void deserialize(Parser& rw, uint64_t& offset)
    {
        rw.rw_uint64(offset);
    }

    template<typename Parser>
    static void serialize(Parser& rw, uint64_t& offset)
    {
        rw.rw_uint64(offset);
    }

    template<typename Parser>
    static void count(Parser& rw, uint64_t& offset)
    {
        rw.rw_uint64(offset);
    }

    template<typename Parser>
    static void pof1(Parser& rw, uint64_t& offset)
    {
        rw.register_offset(offset);
        rw.rw_uint64(offset);
    }
};

template <typename Implementation>
class IOffset64Mixin : public exbip::CRTPBase<Implementation, IOffset64Mixin<Implementation>>
{
public:
    friend Implementation;
    void rw_offset64(uint64_t& offset) { this->downcast()->template rw_descriptor<Offset64Descriptor>(offset); }
};


class Offset64sDescriptor
{
public:
    template<typename Parser>
    static void deserialize(Parser& rw, std::vector<uint64_t>& offsets, size_t count)
    {
        rw.rw_uint64s(offsets, count);
    }
    
    template<typename Parser>
    static void serialize(Parser& rw, std::vector<uint64_t>& offsets, size_t count)
    {
        rw.rw_uint64s(offsets, count);
    }

    template<typename Parser>
    static void count(Parser& rw, std::vector<uint64_t>& offsets, size_t count)
    {
        rw.rw_uint64s(offsets, count);
    }

    template<typename Parser>
    static void pof1(Parser& rw, std::vector<uint64_t>& offsets, size_t count)
    {
        for (int i=0; i < count; ++i)
        {
            rw.register_offset(rw.tell());
            rw.rw_uint64(offsets[i]);
        }
    }
};

template <typename Implementation>
class IOffset64sMixin : public exbip::CRTPBase<Implementation, IOffset64sMixin<Implementation>>
{
public:
    void rw_offset64s(std::vector<uint64_t>& offsets, size_t count) { this->downcast()->template rw_descriptor<Offset64sDescriptor>(offsets, count); }
};


class MemoizedString32Descriptor
{
public:
    template<typename Parser>
    static void deserialize(Parser& rw, uint32_t& offset, std::string& s, MXECStringBank& sbank)
    {
        rw.rw_offset32(offset);
        if (offset == 0 || offset >= 0xFFFFFFFF)
            return;
        auto tmp_offset = rw.globalTell();
        rw.seek(offset);
        rw.rw_cbytestring(s);
        rw.globalSeek(tmp_offset);
    }

    template<typename Parser>
    static void serialize(Parser& rw, uint32_t& offset, std::string& s, MXECStringBank& sbank)
    {
        rw.rw_offset32(offset);
    }

    template<typename Parser>
    static void count(Parser& rw, uint32_t& offset, std::string& s, MXECStringBank& sbank)
    {
        rw.rw_offset32(offset);
        sbank.updateOffsets(s, offset);
    }

    template<typename Parser>
    static void calculateOffsets(Parser& rw, uint32_t& offset, std::string& s, MXECStringBank& sbank)
    {
        rw.rw_offset32(offset);
        sbank.updateOffsets(s, offset);
        sbank.addString(s, offset);
    }
};


template <typename Implementation>
class IMemoizedString32Mixin : public exbip::CRTPBase<Implementation, IMemoizedString32Mixin<Implementation>>
{
public:
    void rw_memoized_string32(uint32_t& offset, std::string& s, MXECStringBank& sbank)
    {
        this->downcast()->template rw_descriptor<MemoizedString32Descriptor>(offset, s, sbank); 
    }
};


class MemoizedString64Descriptor
{
public:
    template<typename Parser>
    static void deserialize(Parser& rw, uint64_t& offset, std::string& s, MXECStringBank& sbank)
    {
        rw.rw_offset64(offset);
        if (offset == 0 || offset >= 0xFFFFFFFF)
            return;
        sbank.updateOffsets(s, offset);
        auto tmp_offset = rw.globalTell();
        rw.seek(offset);
        rw.rw_cbytestring(s);
        rw.globalSeek(tmp_offset);
    }

    template<typename Parser>
    static void serialize(Parser& rw, uint64_t& offset, std::string& s, MXECStringBank& sbank)
    {
        rw.rw_offset64(offset);
    }

    template<typename Parser>
    static void count(Parser& rw, uint64_t& offset, std::string& s, MXECStringBank& sbank)
    {
        rw.rw_offset64(offset);
        sbank.updateOffsets(s, offset);
    }

    template<typename Parser>
    static void calculateOffsets(Parser& rw, uint64_t& offset, std::string& s, MXECStringBank& sbank)
    {
        rw.rw_offset64(offset);
        sbank.updateOffsets(s, offset);
        sbank.addString(s, offset);
    }
};

template <typename Implementation>
class IMemoizedString64Mixin : public exbip::CRTPBase<Implementation, IMemoizedString64Mixin<Implementation>>
{
public:
    void rw_memoized_string64(uint64_t& offset, std::string& s, MXECStringBank& sbank) 
    {
        this->downcast()->template rw_descriptor<MemoizedString64Descriptor>(offset, s, sbank); 
    }
};
