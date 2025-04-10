#pragma once

#include <bit>
#include <type_traits>
#include <exbip/_detail/DefineDetailMacros.hpp>


BEGIN_EXBIP_NAMESPACE

template<typename T>
requires std::is_arithmetic_v<T>
T byteswap(T value)
{
    if constexpr (sizeof(T) == 1)
        return value;
    else if constexpr (sizeof(T) == 2)
    {        
        uint16_t tmp = *std::bit_cast<uint16_t*, T*>(&value);
        #if defined(__clang__) || defined(__GNUG__)
            tmp = __builtin_bswap16(tmp);
        #elif defined(_MSC_VER)
            tmp = _byteswap_ushort(tmp);
        #else
            tmp = (tmp & 0xFF00 >> 8) | (tmp & 0x00FF << 8);
        #endif
        return *std::bit_cast<T*, uint16_t*>(&tmp);
    }
    else if constexpr(sizeof(T) == 4)
    {        
        uint32_t tmp = *std::bit_cast<uint32_t*, T*>(&value);
        #if defined(__clang__) || defined(__GNUG__)
            tmp = __builtin_bswap32(tmp);
        #elif defined(_MSC_VER)
            tmp = _byteswap_ulong(tmp);
        #else
            tmp = (tmp & 0xFF000000 >> 24) | 
                  (tmp & 0x00FF0000 >>  8) | 
                  (tmp & 0x0000FF00 <<  8) | 
                  (tmp & 0x000000FF << 24);
        #endif
        return *std::bit_cast<T*, uint32_t*>(&tmp);
    }
    else if constexpr(sizeof(T) == 8)
    {        
        uint64_t tmp = *std::bit_cast<uint64_t*, T*>(&value);
        #if defined(__clang__) || defined(__GNUG__)
            tmp = __builtin_bswap64(tmp);
        #elif defined(_MSC_VER)
            tmp = _byteswap_uint64(tmp);
        #else
            tmp = (tmp & 0xFF00000000000000 >> 56) |
                  (tmp & 0x00FF000000000000 >> 40) |
                  (tmp & 0x0000FF0000000000 >> 24) |
                  (tmp & 0x000000FF00000000 >>  8) |
                  (tmp & 0x00000000FF000000 <<  8) |
                  (tmp & 0x0000000000FF0000 << 24) |
                  (tmp & 0x000000000000FF00 << 40) |
                  (tmp & 0x00000000000000FF << 56);
        #endif
        return *std::bit_cast<T*, uint64_t*>(&tmp);
    }
    else
    {
        static_assert(sizeof(T) != 1 && sizeof(T) != 2 && sizeof(T) != 4 && sizeof(T) != 8, "Bad template parameter width");
    }
}

END_EXBIP_NAMESPACE
