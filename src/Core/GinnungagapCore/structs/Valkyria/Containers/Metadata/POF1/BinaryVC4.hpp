#pragma once

#include <GinnungagapCore/structs/Valkyria/serialization/Structs/Container.hpp>
#include <GinnungagapCore/structs/Valkyria/serialization/CanvasParsers.hpp>


template<typename T>
std::vector<T> decompressPOF(const std::string& data_s)
{
    T offset = 0;
    std::vector<T> offsets;
    constexpr T PTR_SIZE = sizeof(T);

    const uint8_t* const data = reinterpret_cast<const uint8_t*>(data_s.c_str());

    for (int i=0; i < data_s.size(); ++i)
    {
        uint8_t elem = data[i];
        uint8_t power = (elem & 0xC0) >> 6;
        T       value = elem & 0x3F;

        if (power == 1)
        {
            offset += PTR_SIZE * value;
        }
        else if (power == 2)
        {
            offset += PTR_SIZE * ((value << 0x08) | (data[i+1] << 0x00) );
            i += 1;
        }
        else if (power == 3)
        {
            offset += PTR_SIZE * ((value << 0x18) | (data[i+1] << 0x10) | (data[i+2] << 0x08) | (data[i+3] << 0x00) );
            i += 3;
        }
        else
        {
            continue;
        }
        
        offsets.push_back(offset);
    }
    return offsets;
}

inline std::vector<uint64_t> decompressPOF1(const std::string& data)
{
    return decompressPOF<uint64_t>(data);
}

template<typename T>
consteval uint8_t powerOf2()
{
    if constexpr(sizeof(T) == 1)
        return 0;
    else if constexpr(sizeof(T) == 2)
        return 1;
    else if constexpr(sizeof(T) == 4)
        return 2;
    else if constexpr(sizeof(T) == 8)
        return 3;
    else
        static_assert(!sizeof(T), "Type width is not a power of 2");
}

inline char toChar(uint8_t v)
{
    char res = *reinterpret_cast<char*>(&v);
    return res;
}

template<typename T>
std::string compressPOF(const std::vector<T>& offsets)
{
    constexpr T PTR_SIZE = sizeof(T);
    constexpr T MASK     = PTR_SIZE - 1;
    constexpr T POWER    = powerOf2<T>();

    // Reserve 2 bytes on the first element for the
    // width of the delta; remaining bytes are the
    // data index when the data is interpreted as a T*.
    constexpr T LIM_1B_WIDTH = 1 << 6; // OK for all T
    constexpr T LIM_2B_WIDTH = 1 << (6 + 8*1); // Will not work for 8-bit T
    constexpr T LIM_4B_WIDTH = 1 << (6 + 8*3); // Will not work for 8-bit, 16-bit T

    T previous_offset = 0;
    std::string data;

    for (int i=0; i < offsets.size(); ++i)
    {
        T delta = (offsets[i] - previous_offset);
        
        if ((delta & MASK) != 0)
            throw std::runtime_error("Attempted to compress offsets that are not aligned to the pointer width");
        
        delta >>= POWER;
        if (delta < LIM_1B_WIDTH)
        {
            data.push_back(toChar(0x40 | delta));
        }
        else if (delta < LIM_2B_WIDTH)
        {
            data.push_back(toChar(0x80 | ((delta & 0x3F00) >> 8)));
            data.push_back(toChar(delta & 0xFF));
        }
        else if (delta < LIM_4B_WIDTH)
        {
            data.push_back(toChar(0xC0 | ((delta & 0x3F000000) >> 0x18)));
            data.push_back(toChar((delta & 0x00FF0000) >> 0x10));
            data.push_back(toChar((delta & 0x0000FF00) >> 0x08));
            data.push_back(toChar((delta & 0x000000FF) >> 0x00));
        }
        else
        {
            throw std::runtime_error("Attempted to compress an offset delta larger than " + std::to_string(LIM_4B_WIDTH*PTR_SIZE));
        }
        previous_offset = offsets[i];
        
    }
    return data;
}

inline std::string compressPOF1(const std::vector<uint64_t>& offsets)
{
    return compressPOF(offsets);
}


struct POF1 : public BaseContainerVC4<POF1>
{
public:
    uint32_t data_size;
    std::string data;
public:
    template<typename Parser>
    void valk_rw(Parser& rw)
    {
        rw.rw_uint32_le(data_size);
        rw.rw_bytestring(data, data_size - 4);
        rw.align(rw.tell(), 0x10);
    }

    template<typename Parser>
    void rw_subcontainers(Parser& rw)
    {
    }

    auto getDecompressed()
    {
        return decompressPOF1(this->data);
    }

    void setData(const std::vector<uint64_t>& offsets)
    {
        this->data = compressPOF1(offsets);
        this->data_size = this->data.size() + 4;
        this->header.body_size = roundToAlignment(this->data_size, 0x10);
        this->header.size      = this->header.body_size;
    }

    template<typename T, typename... Args>
    void from_container(T& ctr, Args&&... args)
    {
        header.container_type = '1FOP';
        header.header_size = 0x20;
        header.flags.EXTENDED_HEADER(true);
        header.depth = ctr.header.depth + 1;

        POF1Calculator pof_rw;
        pof_rw.asLittleEndian().rw_obj(ctr, args...);
        this->setData(pof_rw.offsets);
    }
};
