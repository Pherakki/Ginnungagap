#pragma once

#include <bitset>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#ifdef __linux__
#include <unicode/unistr.h>
#endif



namespace stringmanip
{
    enum class SectionSide
    {
        Left=0,
        Right=1
    };


    namespace __Impl
    {
        template<SectionSide side, typename FindFunctor>
        std::string __section_impl(const std::string& s, const std::string& delimiter, FindFunctor Find) 
        {
            const size_t split_point = Find(s, delimiter);
            size_t start;
            size_t count;
            if constexpr (side == SectionSide::Left)
            {
                start = 0;
                count = split_point == std::string::npos? s.size() : split_point;
            }
            else if constexpr (side == SectionSide::Right)
            {
                start  = split_point == std::string::npos? 0 : split_point + delimiter.size();
                count  = s.size() - start;
            }
            else
                static_assert(side < SectionSide::Left || side > SectionSide::Right, "Unhandled SectionSide direction");
            return s.substr(start, count);
        }

        template<size_t width>
        consteval auto bitmask()
        {
            uint64_t mask=0;
            for (int i=0; i < width; ++i)
                mask |= (1 << i);
            return mask;
        }

        template<typename T>
        std::string toHexString(T v)
        {
            std::stringstream stream;
            stream << "0x" << std::hex << std::setw(sizeof(T)*2) << std::setfill('0') << v;
            return stream.str();
        }

        template<typename T>
        T fromHexString(const std::string& v)
        {
            return std::stoull(v.c_str(), nullptr, 16);
        }

        template<typename T>
        std::string toBitString(T v)
        {
            return "0b" + std::bitset<sizeof(T)*8>(v).to_string();
        }

        template<typename T>
        T fromBitString(const std::string& v)
        {
            return std::stoull(&v.c_str()[2], nullptr, 2);
        }
    }

    inline std::string toBitString(uint8_t v)  { return __Impl::toBitString(v); }
    inline std::string toBitString(uint16_t v) { return __Impl::toBitString(v); }
    inline std::string toBitString(uint32_t v) { return __Impl::toBitString(v); }
    inline std::string toBitString(uint64_t v) { return __Impl::toBitString(v); }
    inline std::string toHexString(uint8_t v)  { return __Impl::toHexString(v); }
    inline std::string toHexString(uint16_t v) { return __Impl::toHexString(v); }
    inline std::string toHexString(uint32_t v) { return __Impl::toHexString(v); }
    inline std::string toHexString(uint64_t v) { return __Impl::toHexString(v); }
    inline uint8_t  fromBitString8 (const std::string& v) { return __Impl::fromBitString<uint8_t >(v); }
    inline uint16_t fromBitString16(const std::string& v) { return __Impl::fromBitString<uint16_t>(v); }
    inline uint32_t fromBitString32(const std::string& v) { return __Impl::fromBitString<uint32_t>(v); }
    inline uint64_t fromBitString64(const std::string& v) { return __Impl::fromBitString<uint64_t>(v); }
    inline uint8_t  fromHexString8 (const std::string& v) { return __Impl::fromHexString<uint8_t >(v); }
    inline uint16_t fromHexString16(const std::string& v) { return __Impl::fromHexString<uint16_t>(v); }
    inline uint32_t fromHexString32(const std::string& v) { return __Impl::fromHexString<uint32_t>(v); }
    inline uint64_t fromHexString64(const std::string& v) { return __Impl::fromHexString<uint64_t>(v); }

    template<SectionSide side>
    std::string section(const std::string& s, const std::string& delimiter) 
    {
        return __Impl::__section_impl<side>(s, delimiter, [](const std::string& v_s, const std::string& v_delimiter){return v_s.find(v_delimiter); });
    }

    template<SectionSide side>
    std::string rsection(const std::string& s, const std::string& delimiter) 
    {
        return __Impl::__section_impl<side>(s, delimiter, [](const std::string& v_s, const std::string& v_delimiter){return v_s.rfind(v_delimiter); });
    }

    std::string segment  (const std::string& s, const std::string& delimiter_1, const std::string& delimiter_2);
    std::string lrsegment(const std::string& s, const std::string& delimiter_1, const std::string& delimiter_2);
    std::string rlsegment(const std::string& s, const std::string& delimiter_1, const std::string& delimiter_2);
    std::string rsegment (const std::string& s, const std::string& delimiter_1, const std::string& delimiter_2);
    
    std::string MXEParameterEntityType(const std::string& s);
    std::string MXEParameterSetName(const std::string& s);
    std::string MXEParameterSetType(const std::string& s);

    std::string convertEncoding(const std::string& s, const std::string& from_enc, const std::string& to_enc);
    std::string sjisToUtf8(const std::string& s);
    std::string utf8ToSjis(const std::string& s);

    std::string toLower(const std::string&& s);

    template<typename T>
    void getLineCrossPlat(T& stream, std::string& out)
    {
        out.clear();

        for(;;)
        {
            if (stream.eof())
                return;
            
            char c;
            stream.read(&c, 1);
            switch (c)
            {
                case '\n':
                    return;
                case '\r':
                {
                    if (stream.peek() == '\n')
                        stream.read(&c, 1);
                    return;
                }
                default:
                    out.push_back(c);
            }
        }
    }
}


