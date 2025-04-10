#include "StringUtils.hpp"
#include <algorithm>
#include <vector>

#ifdef __linux__
    #include <unicode/unistr.h>
#endif
#ifdef _WIN32
#include <Windows.h>
#include <stringapiset.h>
#endif


namespace stringmanip
{
    namespace __Impl
    {
        template<SectionSide SearchDir1, SectionSide SearchDir2>
        std::string __segment_impl(const std::string& s, const std::string& delimiter_1, const std::string& delimiter_2)
        {
            static_assert(!((SearchDir1 < SectionSide::Left) || (SearchDir1 > SectionSide::Right)), "Unhandled SearchDir1 direction");
            static_assert(!((SearchDir2 < SectionSide::Left) || (SearchDir2 > SectionSide::Right)), "Unhandled SearchDir2 direction");

            size_t start;
            size_t end;
            size_t pt1;
            if constexpr (SearchDir1 == SectionSide::Left)
            {
                pt1 = s.find(delimiter_1, 0);
                if (pt1 == std::string::npos) return "";
            }
            else if constexpr (SearchDir1 == SectionSide::Right)
            {
                pt1   = s.rfind(delimiter_1, std::string::npos);
                if (pt1 == std::string::npos) return "";
            }

            // Search from pt1 from the left
            if constexpr (SearchDir2 == SectionSide::Left)
            {
                size_t pend;
                start = pt1 + delimiter_1.size();
                pend  = s.find(delimiter_2, start);
                end = (pend == std::string::npos)? s.size() : pend;
            }
            // Search from pt1 from the right
            else if constexpr (SearchDir2 == SectionSide::Right)
            {
                size_t pstart;
                end = pt1;
                pstart = s.rfind(delimiter_2, end-delimiter_1.size());
                start = (pstart == std::string::npos)? 0 : pstart + delimiter_2.size();
            }
            
            return s.substr(start, end-start);
        }
    }

    std::string segment (const std::string& s, const std::string& delimiter_1, const std::string& delimiter_2)
    {
        return __Impl::__segment_impl<SectionSide::Left, SectionSide::Left>(s, delimiter_1, delimiter_2);
    }

    std::string lrsegment(const std::string& s, const std::string& delimiter_1, const std::string& delimiter_2)
    {
        return __Impl::__segment_impl<SectionSide::Left, SectionSide::Right>(s, delimiter_1, delimiter_2);
    }

    std::string rlsegment(const std::string& s, const std::string& delimiter_1, const std::string& delimiter_2)
    {
        return __Impl::__segment_impl<SectionSide::Right, SectionSide::Left>(s, delimiter_1, delimiter_2);
    }

    std::string rsegment(const std::string& s, const std::string& delimiter_1, const std::string& delimiter_2)
    {
        return __Impl::__segment_impl<SectionSide::Right, SectionSide::Right>(s, delimiter_1, delimiter_2);
    }


    std::string MXEParameterSetType(const std::string& s)
    {
        return lrsegment(s, ":", "@");
    }

    std::string MXEParameterSetName(const std::string& s)
    {
        return section<SectionSide::Right>(s, ":");
    }

#ifdef __linux__
    std::string convertEncoding(const std::string& s, const std::string& from_enc, const std::string& to_enc)
    {
        const char* to_enc_   = to_enc.c_str();
        const char* from_enc_ = from_enc.c_str();

        icu::UnicodeString from_buffer(&s[0], from_enc_);
        int length = from_buffer.extract(0, from_buffer.length(), NULL, to_enc_);

        std::vector<char> to_buffer(length+1);
        from_buffer.extract(0, from_buffer.length(), &to_buffer[0], to_enc_);

        return std::string(to_buffer.begin(), to_buffer.end() -1 );
    }
#endif

#ifdef _WIN32
    std::string convertEncoding(const std::string& s, int from_enc, int to_enc)
    {
        int utf16_size = MultiByteToWideChar(from_enc, 0, s.c_str(), (int)s.size(), NULL, 0);
        std::wstring wbuf(utf16_size, 0);
        MultiByteToWideChar(from_enc, 0, s.c_str(), (int)s.size(), &wbuf[0], utf16_size);

        int result_size = WideCharToMultiByte(to_enc, 0, wbuf.c_str(), (int)wbuf.size(), NULL, 0, NULL, NULL);
        std::string result(result_size, 0);
        WideCharToMultiByte(to_enc, 0, wbuf.c_str(), (int)wbuf.size(), &result[0], result_size, NULL, NULL);

        return result;
    }
#endif


    std::string sjisToUtf8(const std::string& s)
    {
#if defined(__linux__)
        return convertEncoding(s, "shift_jis", "utf8");
#elif defined(_WIN32)
        return convertEncoding(s, 932, CP_UTF8);
#else
        static_assert(0);
#endif
    }

    std::string utf8ToSjis(const std::string& s)
    {
#if defined(__linux__)
        return convertEncoding(s, "utf8", "shift_jis");
#elif defined(_WIN32)
        return convertEncoding(s, CP_UTF8, 932);
#else
        static_assert(0);
#endif
    }

    std::string toLower(const std::string&& s)
    {
        std::string out;
        out.resize(s.size());

        std::transform(s.begin(), s.end(), out.begin(), [](char c){ return std::tolower(c); });
        
        return out;
    }
}

