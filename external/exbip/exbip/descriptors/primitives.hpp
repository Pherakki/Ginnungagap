#pragma once

#include <array>
#include <exbip/framework/CRTP/Base.hpp>
#include <exbip/utils/Byteswap.hpp>
#include <exbip/utils/FP16.hpp>
#include <exbip/utils/Endian.hpp>

BEGIN_EXBIP_NAMESPACE

template<typename T, exbip::endian endianness>
struct PrimitiveDescriptor
{
    template<typename Parser>
    static void deserialize(Parser& rw, T& value)
    {
        auto& stream = rw.getBytestream();

        std::array<char, sizeof(T)> buf;
        stream.read(&buf[0], sizeof(T));

        value = *std::bit_cast<T*, char*>(&buf[0]);
        if      constexpr (endianness == exbip::endian::little)
        {}
        else if constexpr (endianness == exbip::endian::big)
        {
            value = exbip::byteswap(value);
        }
        else
        {
            static_assert(!std::is_same<T, T>::value, "invalid endianness for primitive descriptor");
        }

    }

    template<typename Parser>
    static void serialize(Parser& rw, const T& value)
    {
        auto& stream = rw.getBytestream();

        char* c;
        T v = value;
        if      constexpr (endianness == exbip::endian::little)
        {
            c = std::bit_cast<char*, const T*>(&v);
        }
        else if constexpr (endianness == exbip::endian::big)
        {
            v = exbip::byteswap(v);
            c = std::bit_cast<char*, const T*>(&v);
        }
        else
        {
            static_assert(!std::is_same<T, T>::value, "invalid endianness for primitive descriptor");
        }
        stream.write(c, sizeof(T));
    }

    template<typename Parser>
    static void count(Parser& rw, const T& value)
    {
        auto& stream = rw.getBytestream();

        stream.advance(sizeof(T));
    }
};


template<exbip::endian endianness>
struct Float16Descriptor
{
    template<typename Parser>
    static void deserialize(Parser& rw, float& value)
    {
        auto& stream = rw.getBytestream();

        std::array<char, 2> buf;
        stream.read(&buf[0], 2);
        uint16_t tmp = *std::bit_cast<uint16_t*, char*>(&buf[0]);
        if constexpr (endianness == exbip::endian::big)
            tmp = exbip::byteswap(tmp);
        value = fp16_to_fp32(tmp);
    }

    template<typename Parser>
    static void serialize(Parser& rw, const float& value)
    {
        auto& stream = rw.getBytestream();

        uint16_t tmp = fp32_to_fp16(value);
        if constexpr (endianness == exbip::endian::big)
            tmp = exbip::byteswap(tmp);
        char* c = std::bit_cast<char*, uint16_t*>(&tmp);
        stream.write(c, 2);
    }

    template<typename Parser>
    static void count(Parser& rw, const float& value)
    {
        auto& stream = rw.getBytestream();
        stream.advance(2);
    }
};

/* ------------------ int8 ------------------ */

template<typename Implementation>
struct IInt8LEMixin : public CRTPBase<Implementation, IInt8LEMixin<Implementation>>
{
    void rw_int8_le(int8_t& value) { this->downcast()->template rw_descriptor<PrimitiveDescriptor<int8_t,  exbip::endian::little>>(value); }
};

template<typename Implementation>
struct IInt8BEMixin : public CRTPBase<Implementation, IInt8BEMixin<Implementation>>
{
    void rw_int8_be(int8_t& value) { this->downcast()->template rw_descriptor<PrimitiveDescriptor<int8_t,  exbip::endian::big>>(value); }
};

template<typename Implementation>
struct IInt8EMixin : public CRTPBase<Implementation, IInt8EMixin<Implementation>>
{
    void rw_int8_e(int8_t& value, exbip::endian endianness) 
    {
        if (endianness == exbip::endian::little)
            this->downcast()->rw_int8_le(value);
        else if (endianness == exbip::endian::big)
            this->downcast()->rw_int8_be(value);
        else
            throw std::runtime_error("invalid endianness for rw_int8_e");
    }
};

template<typename Implementation, exbip::endian endianness>
struct IInt8Mixin : public CRTPBase<Implementation, IInt8Mixin<Implementation, endianness>>
{
    void rw_int8(int8_t& value) { this->downcast()->template rw_descriptor<PrimitiveDescriptor<int8_t,  endianness>>(value); }
};

/* ------------------ int16 ------------------ */

template<typename Implementation>
struct IInt16LEMixin : public CRTPBase<Implementation, IInt16LEMixin<Implementation>>
{
    void rw_int16_le(int16_t& value) { this->downcast()->template rw_descriptor<PrimitiveDescriptor<int16_t, exbip::endian::little>>(value); }
};

template<typename Implementation>
struct IInt16BEMixin : public CRTPBase<Implementation, IInt16BEMixin<Implementation>>
{
    void rw_int16_be(int16_t& value) { this->downcast()->template rw_descriptor<PrimitiveDescriptor<int16_t, exbip::endian::big>>(value); }
};

template<typename Implementation>
struct IInt16EMixin : public CRTPBase<Implementation, IInt16EMixin<Implementation>>
{
    void rw_int16_e(int16_t& value, exbip::endian endianness) 
    {
        if (endianness == exbip::endian::little)
            this->downcast()->rw_int16_le(value);
        else if (endianness == exbip::endian::big)
            this->downcast()->rw_int16_be(value);
        else
            throw std::runtime_error("invalid endianness for rw_int16_e");
    }
};

template<typename Implementation, exbip::endian endianness>
struct IInt16Mixin : public CRTPBase<Implementation, IInt16Mixin<Implementation, endianness>>
{
    void rw_int16(int16_t& value) { this->downcast()->template rw_descriptor<PrimitiveDescriptor<int16_t, endianness>>(value); }
};

/* ------------------ int32 ------------------ */

template<typename Implementation>
struct IInt32LEMixin : public CRTPBase<Implementation, IInt32LEMixin<Implementation>>
{
    void rw_int32_le(int32_t& value) { this->downcast()->template rw_descriptor<PrimitiveDescriptor<int32_t, exbip::endian::little>>(value); }
};

template<typename Implementation>
struct IInt32BEMixin : public CRTPBase<Implementation, IInt32BEMixin<Implementation>>
{
    void rw_int32_be(int32_t& value) { this->downcast()->template rw_descriptor<PrimitiveDescriptor<int32_t, exbip::endian::big>>(value); }
};

template<typename Implementation>
struct IInt32EMixin : public CRTPBase<Implementation, IInt32EMixin<Implementation>>
{
    void rw_int32_e(int32_t& value, exbip::endian endianness) 
    {
        if (endianness == exbip::endian::little)
            this->downcast()->rw_int32_le(value);
        else if (endianness == exbip::endian::big)
            this->downcast()->rw_int32_be(value);
        else
            throw std::runtime_error("invalid endianness for rw_int32_e");
    }
};

template<typename Implementation, exbip::endian endianness>
struct IInt32Mixin : public CRTPBase<Implementation, IInt32Mixin<Implementation, endianness>>
{
    void rw_int32(int32_t& value) { this->downcast()->template rw_descriptor<PrimitiveDescriptor<int32_t, endianness>>(value); }
};

/* ------------------ int64 ------------------ */

template<typename Implementation>
struct IInt64LEMixin : public CRTPBase<Implementation, IInt64LEMixin<Implementation>>
{
    void rw_int64_le(int64_t& value) { this->downcast()->template rw_descriptor<PrimitiveDescriptor<int64_t, exbip::endian::little>>(value); }
};

template<typename Implementation>
struct IInt64BEMixin : public CRTPBase<Implementation, IInt64BEMixin<Implementation>>
{
    void rw_int64_be(int64_t& value) { this->downcast()->template rw_descriptor<PrimitiveDescriptor<int64_t, exbip::endian::big>>(value); }
};

template<typename Implementation>
struct IInt64EMixin : public CRTPBase<Implementation, IInt64EMixin<Implementation>>
{
    void rw_int64_e(int64_t& value, exbip::endian endianness) 
    {
        if (endianness == exbip::endian::little)
            this->downcast()->rw_int64_le(value);
        else if (endianness == exbip::endian::big)
            this->downcast()->rw_int64_be(value);
        else
            throw std::runtime_error("invalid endianness for rw_int64_e");
    }
};

template<typename Implementation, exbip::endian endianness>
struct IInt64Mixin : public CRTPBase<Implementation, IInt64Mixin<Implementation, endianness>>
{
    void rw_int64(int64_t& value) { this->downcast()->template rw_descriptor<PrimitiveDescriptor<int64_t, endianness>>(value); }
};

/* ------------------ uint8 ------------------ */

template<typename Implementation>
struct IUInt8LEMixin : public CRTPBase<Implementation, IUInt8LEMixin<Implementation>>
{
    void rw_uint8_le(uint8_t& value) { this->downcast()->template rw_descriptor<PrimitiveDescriptor<uint8_t,  exbip::endian::little>>(value); }
};

template<typename Implementation>
struct IUInt8BEMixin : public CRTPBase<Implementation, IUInt8BEMixin<Implementation>>
{
    void rw_uint8_be(uint8_t& value) { this->downcast()->template rw_descriptor<PrimitiveDescriptor<uint8_t,  exbip::endian::big>>(value); }
};

template<typename Implementation>
struct IUInt8EMixin : public CRTPBase<Implementation, IUInt8EMixin<Implementation>>
{
    void rw_uint8_e(uint8_t& value, exbip::endian endianness) 
    {
        if (endianness == exbip::endian::little)
            this->downcast()->rw_uint8_le(value);
        else if (endianness == exbip::endian::big)
            this->downcast()->rw_uint8_be(value);
        else
            throw std::runtime_error("invalid endianness for rw_uint8_e");
    }
};

template<typename Implementation, exbip::endian endianness>
struct IUInt8Mixin : public CRTPBase<Implementation, IUInt8Mixin<Implementation, endianness>>
{
    void rw_uint8(uint8_t& value) { this->downcast()->template rw_descriptor<PrimitiveDescriptor<uint8_t,  endianness>>(value); }
};

/* ------------------ uint16 ------------------ */

template<typename Implementation>
struct IUInt16LEMixin : public CRTPBase<Implementation, IUInt16LEMixin<Implementation>>
{
    void rw_uint16_le(uint16_t& value) { this->downcast()->template rw_descriptor<PrimitiveDescriptor<uint16_t, exbip::endian::little>>(value); }
};

template<typename Implementation>
struct IUInt16BEMixin : public CRTPBase<Implementation, IUInt16BEMixin<Implementation>>
{
    void rw_uint16_be(uint16_t& value) { this->downcast()->template rw_descriptor<PrimitiveDescriptor<uint16_t, exbip::endian::big>>(value); }
};

template<typename Implementation>
struct IUInt16EMixin : public CRTPBase<Implementation, IUInt16EMixin<Implementation>>
{
    void rw_uint16_e(uint16_t& value, exbip::endian endianness) 
    {
        if (endianness == exbip::endian::little)
            this->downcast()->rw_uint16_le(value);
        else if (endianness == exbip::endian::big)
            this->downcast()->rw_uint16_be(value);
        else
            throw std::runtime_error("invalid endianness for rw_uint16_e");
    }
};

template<typename Implementation, exbip::endian endianness>
struct IUInt16Mixin : public CRTPBase<Implementation, IUInt16Mixin<Implementation, endianness>>
{
    void rw_uint16(uint16_t& value) { this->downcast()->template rw_descriptor<PrimitiveDescriptor<uint16_t, endianness>>(value); }
};

/* ------------------ uint32 ------------------ */

template<typename Implementation>
struct IUInt32LEMixin : public CRTPBase<Implementation, IUInt32LEMixin<Implementation>>
{
    void rw_uint32_le(uint32_t& value) { this->downcast()->template rw_descriptor<PrimitiveDescriptor<uint32_t, exbip::endian::little>>(value); }
};

template<typename Implementation>
struct IUInt32BEMixin : public CRTPBase<Implementation, IUInt32BEMixin<Implementation>>
{
    void rw_uint32_be(uint32_t& value) { this->downcast()->template rw_descriptor<PrimitiveDescriptor<uint32_t, exbip::endian::big>>(value); }
};

template<typename Implementation>
struct IUInt32EMixin : public CRTPBase<Implementation, IUInt32EMixin<Implementation>>
{
    void rw_uint32_e(uint32_t& value, exbip::endian endianness) 
    {
        if (endianness == exbip::endian::little)
            this->downcast()->rw_uint32_le(value);
        else if (endianness == exbip::endian::big)
            this->downcast()->rw_uint32_be(value);
        else
            throw std::runtime_error("invalid endianness for rw_uint32_e");
    }
};

template<typename Implementation, exbip::endian endianness>
struct IUInt32Mixin : public CRTPBase<Implementation, IUInt32Mixin<Implementation, endianness>>
{
    void rw_uint32(uint32_t& value) { this->downcast()->template rw_descriptor<PrimitiveDescriptor<uint32_t, endianness>>(value); }
};

/* ------------------ uint64 ------------------ */

template<typename Implementation>
struct IUInt64LEMixin : public CRTPBase<Implementation, IUInt64LEMixin<Implementation>>
{
    void rw_uint64_le(uint64_t& value) { this->downcast()->template rw_descriptor<PrimitiveDescriptor<uint64_t, exbip::endian::little>>(value); }
};

template<typename Implementation>
struct IUInt64BEMixin : public CRTPBase<Implementation, IUInt64BEMixin<Implementation>>
{
    void rw_uint64_be(uint64_t& value) { this->downcast()->template rw_descriptor<PrimitiveDescriptor<uint64_t, exbip::endian::big>>(value); }
};

template<typename Implementation>
struct IUInt64EMixin : public CRTPBase<Implementation, IUInt64EMixin<Implementation>>
{
    void rw_uint64_e(uint64_t& value, exbip::endian endianness) 
    {
        if (endianness == exbip::endian::little)
            this->downcast()->rw_uint64_le(value);
        else if (endianness == exbip::endian::big)
            this->downcast()->rw_uint64_be(value);
        else
            throw std::runtime_error("invalid endianness for rw_uint64_e");
    }
};

template<typename Implementation, exbip::endian endianness>
struct IUInt64Mixin : public CRTPBase<Implementation, IUInt64Mixin<Implementation, endianness>>
{
    void rw_uint64(uint64_t& value) { this->downcast()->template rw_descriptor<PrimitiveDescriptor<uint64_t, endianness>>(value); }
};

/* ------------------ float32 ------------------ */

template<typename Implementation>
struct IFloat32LEMixin : public CRTPBase<Implementation, IFloat32LEMixin<Implementation>>
{
    void rw_float32_le(float& value) { this->downcast()->template rw_descriptor<PrimitiveDescriptor<float, exbip::endian::little>>(value); }
};

template<typename Implementation>
struct IFloat32BEMixin : public CRTPBase<Implementation, IFloat32BEMixin<Implementation>>
{
    void rw_float32_be(float& value) { this->downcast()->template rw_descriptor<PrimitiveDescriptor<float, exbip::endian::big>>(value); }
};

template<typename Implementation>
struct IFloat32EMixin : public CRTPBase<Implementation, IFloat32EMixin<Implementation>>
{
    void rw_float32_e(float& value, exbip::endian endianness) 
    {
        if (endianness == exbip::endian::little)
            this->downcast()->rw_float32_le(value);
        else if (endianness == exbip::endian::big)
            this->downcast()->rw_float32_be(value);
        else
            throw std::runtime_error("invalid endianness for rw_float32_e");
    }
};

template<typename Implementation, exbip::endian endianness>
struct IFloat32Mixin : public CRTPBase<Implementation, IFloat32Mixin<Implementation, endianness>>
{
    void rw_float32(float& value) { this->downcast()->template rw_descriptor<PrimitiveDescriptor<float, endianness>>(value); }
};

/* ------------------ float64 ------------------ */

template<typename Implementation>
struct IFloat64LEMixin : public CRTPBase<Implementation, IFloat64LEMixin<Implementation>>
{
    void rw_float64_le(double& value) { this->downcast()->template rw_descriptor<PrimitiveDescriptor<double, exbip::endian::little>>(value); }
};

template<typename Implementation>
struct IFloat64BEMixin : public CRTPBase<Implementation, IFloat64BEMixin<Implementation>>
{
    void rw_float64_be(double& value) { this->downcast()->template rw_descriptor<PrimitiveDescriptor<double, exbip::endian::big>>(value); }
};

template<typename Implementation>
struct IFloat64EMixin : public CRTPBase<Implementation, IFloat64EMixin<Implementation>>
{
    void rw_float64_e(double& value, exbip::endian endianness) 
    {
        if (endianness == exbip::endian::little)
            this->downcast()->rw_float64_le(value);
        else if (endianness == exbip::endian::big)
            this->downcast()->rw_float64_be(value);
        else
            throw std::runtime_error("invalid endianness for rw_float64_e");
    }
};

template<typename Implementation, exbip::endian endianness>
struct IFloat64Mixin : public CRTPBase<Implementation, IFloat64Mixin<Implementation, endianness>>
{
    void rw_float64(double& value) { this->downcast()->template rw_descriptor<PrimitiveDescriptor<double, endianness>>(value); }
};

/* ------------------ float16 ------------------ */

template<typename Implementation>
struct IFloat16LEMixin : public CRTPBase<Implementation, IFloat16LEMixin<Implementation>>
{
    void rw_float16_le(float& value) { this->downcast()->template rw_descriptor<Float16Descriptor<exbip::endian::little>>(value); }
};

template<typename Implementation>
struct IFloat16BEMixin : public CRTPBase<Implementation, IFloat16BEMixin<Implementation>>
{
    void rw_float16_be(float& value) { this->downcast()->template rw_descriptor<Float16Descriptor<exbip::endian::big>>(value); }
};

template<typename Implementation>
struct IFloat16EMixin : public CRTPBase<Implementation, IFloat16EMixin<Implementation>>
{
    void rw_float16_e(float& value, exbip::endian endianness) 
    {
        if (endianness == exbip::endian::little)
            this->downcast()->rw_float16_le(value);
        else if (endianness == exbip::endian::big)
            this->downcast()->rw_float16_be(value);
        else
            throw std::runtime_error("invalid endianness for rw_float16_e");
    }
};

template<typename Implementation, exbip::endian endianness>
struct IFloat16Mixin : public CRTPBase<Implementation, IFloat16Mixin<Implementation, endianness>>
{
    void rw_float16(float& value) { this->downcast()->template rw_descriptor<Float16Descriptor<endianness>>(value); }
};

template<typename Implementation>
struct IPrimitiveMixins
    : public IInt8LEMixin<Implementation>
    , public IInt8BEMixin<Implementation>
    , public IInt8EMixin<Implementation>
    , public IInt16LEMixin<Implementation>
    , public IInt16BEMixin<Implementation>
    , public IInt16EMixin<Implementation>
    , public IInt32LEMixin<Implementation>
    , public IInt32BEMixin<Implementation>
    , public IInt32EMixin<Implementation>
    , public IInt64LEMixin<Implementation>
    , public IInt64BEMixin<Implementation>
    , public IInt64EMixin<Implementation>
    , public IUInt8LEMixin<Implementation>
    , public IUInt8BEMixin<Implementation>
    , public IUInt8EMixin<Implementation>
    , public IUInt16LEMixin<Implementation>
    , public IUInt16BEMixin<Implementation>
    , public IUInt16EMixin<Implementation>
    , public IUInt32LEMixin<Implementation>
    , public IUInt32BEMixin<Implementation>
    , public IUInt32EMixin<Implementation>
    , public IUInt64LEMixin<Implementation>
    , public IUInt64BEMixin<Implementation>
    , public IUInt64EMixin<Implementation>
    , public IFloat32LEMixin<Implementation>
    , public IFloat32BEMixin<Implementation>
    , public IFloat32EMixin<Implementation>
    , public IFloat64LEMixin<Implementation>
    , public IFloat64BEMixin<Implementation>
    , public IFloat64EMixin<Implementation>
    , public IFloat16LEMixin<Implementation>
    , public IFloat16BEMixin<Implementation>
    , public IFloat16EMixin<Implementation>
{};

template<typename Implementation, exbip::endian endianness>
struct IEndianPrimitiveMixins
    : public IInt8Mixin<Implementation, endianness>
    , public IInt16Mixin<Implementation, endianness>
    , public IInt32Mixin<Implementation, endianness>
    , public IInt64Mixin<Implementation, endianness>
    , public IUInt8Mixin<Implementation, endianness>
    , public IUInt16Mixin<Implementation, endianness>
    , public IUInt32Mixin<Implementation, endianness>
    , public IUInt64Mixin<Implementation, endianness>
    , public IFloat32Mixin<Implementation, endianness>
    , public IFloat64Mixin<Implementation, endianness>
    , public IFloat16Mixin<Implementation, endianness>
{};

END_EXBIP_NAMESPACE
