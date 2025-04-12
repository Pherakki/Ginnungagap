#pragma once

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
    }

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


