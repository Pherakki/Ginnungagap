#pragma once

#include <exbip/framework/CRTP/Base.hpp>
#include <vector>

BEGIN_EXBIP_NAMESPACE

struct BytestringDescriptor
{
    template<typename Parser>
    static void deserialize(Parser& rw, std::string& s, size_t size)
    {
        s.resize(size);
        rw.getBytestream().read(&s[0], size);
    }

    template<typename Parser>
    static void serialize(Parser& rw, const std::string& s, size_t size)
    {
        rw.getBytestream().write(s.c_str(), size);
    }

    template<typename Parser>
    static void count(Parser& rw, const std::string& s, size_t size)
    {
        rw.getBytestream().advance(size);
    }
};

template <typename Implementation>
struct IBytestringMixin : public CRTPBase<Implementation, IBytestringMixin<Implementation>>
{
    void rw_bytestring(std::string& s, size_t size) { this->downcast()->template rw_descriptor<BytestringDescriptor>(s, size); }
};


struct PeekBytestringDescriptor
{
    template<typename Parser>
    static std::string deserialize(Parser& rw, size_t size)
    {
        auto pos = rw.globalTell();

        std::string buf;
        buf.resize(size);
        rw.getBytestream().read(&buf[0], size);

        std::stringstream ss;
        ss << "0x" << std::hex;
        for (const auto& e : buf)
        {
            ss << static_cast<unsigned int>(e);
        }

        rw.globalSeek(pos);
        return ss.str();
    }

    template<typename Parser>
    static std::string serialize(Parser& rw, size_t size)
    {
        return std::string();
    }

    template<typename Parser>
    static std::string count(Parser& rw, size_t size)
    {
        return std::string();
    }
};

template <typename Implementation>
struct IPeekBytestringMixin : public CRTPBase<Implementation, IPeekBytestringMixin<Implementation>>
{
    std::string peek_bytestring(size_t size) { return this->downcast()->template rw_descriptor<PeekBytestringDescriptor>(size); }
};

struct CBytestringDescriptor
{
    template<typename Parser>
    static void deserialize(Parser& rw, std::string& s)
    {
        // TODO: Fix shit implementation 
        std::string out;
        auto& stream = rw.getBytestream();
        char b;
        stream.read(&b, 1);
        while (b != 0)
        {
            out.push_back(b);
            stream.read(&b, 1);
        }
        s = out;
    }

    template<typename Parser>
    static void serialize(Parser& rw, const std::string& s)
    {
        rw.getBytestream().write(&s[0], s.size());
        char zero = 0;
        rw.getBytestream().write(&zero, 1);
    }

    template<typename Parser>
    static void count(Parser& rw, const std::string& s)
    {
        rw.getBytestream().advance(s.size() + 1);
    }
};


template<typename Implementation>
struct ICBytestringMixin : public CRTPBase<Implementation, ICBytestringMixin<Implementation>>
{
    void rw_cbytestring(std::string& s) { this->downcast()->template rw_descriptor<CBytestringDescriptor>(s); }
};

END_EXBIP_NAMESPACE
