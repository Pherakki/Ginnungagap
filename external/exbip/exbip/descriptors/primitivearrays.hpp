#pragma once

#include <array>
#include <exbip/framework/CRTP/Base.hpp>
#include <exbip/utils/Byteswap.hpp>
#include <exbip/utils/FP16.hpp>
#include <exbip/utils/Endian.hpp>


BEGIN_EXBIP_NAMESPACE

template<typename T, exbip::endian endianness>
struct PrimitiveArrayDescriptor
{
    template <typename Parser, size_t N>
    static void deserialize(Parser& rw, std::array<T, N>& value)
    {
        auto& stream = rw.getBytestream();
        constexpr size_t NBYTES = sizeof(T)*N;

        std::array<char, NBYTES> buf;
        stream.read(&buf[0], NBYTES);
        T* v = std::bit_cast<T*, char*>(&buf[0]);
        if      constexpr (endianness == exbip::endian::little)
        {}
        else if constexpr (endianness == exbip::endian::big)
        {
            for (int i=0; i < N; ++i)
                v[i] = exbip::byteswap(v[i]);
        }
        else
        {
            static_assert(!std::is_same<T, T>::value, "invalid endianness for primitive descriptor");
        }
        
        for (int i=0; i < N; ++i)
            value[i] = v[i];
    }

    template<typename Parser, size_t N>
    static void serialize(Parser& rw, const std::array<T, N>& value)
    {
        auto& stream = rw.getBytestream();
        constexpr size_t NBYTES = sizeof(T)*N;

        char* c;
        std::array<T, N> buf;

        for (int i=0; i < N; ++i)
        {
            if      constexpr (endianness == exbip::endian::little)
            {
                buf[i] = value[i];
            }
            else if constexpr (endianness == exbip::endian::big)
            {
                buf[i] = exbip::byteswap(value[i]);
            }
            else
            {
                static_assert(!std::is_same<T, T>::value, "invalid endianness for primitive descriptor");
            }
        }

        c = std::bit_cast<char*, const T*>(buf.data());
        stream.write(c, NBYTES);
    }

    template <typename Parser, size_t N>
    static void count(Parser& rw, const std::array<T, N>& value)
    {
        auto& stream = rw.getBytestream();
        constexpr size_t NBYTES = sizeof(T)*N;

        stream.advance(NBYTES);
    }
};


template<exbip::endian endianness>
struct Float16ArrayDescriptor
{
    template <typename Parser, size_t N>
    static void deserialize(Parser& rw, std::array<float, N>& value)
    {
        auto& stream = rw.getBytestream();

        constexpr size_t NBYTES = 2*N;
        std::array<char, NBYTES> buf;
        stream.read(&buf[0], NBYTES);
        uint16_t* tmp = *std::bit_cast<uint16_t*, char*>(&buf[0]);
        for (int i=0; i < N; ++i)
        {
            uint16_t tmp_val = tmp[i];
            if constexpr (endianness == exbip::endian::big)
                tmp_val = exbip::byteswap(tmp_val);
            value[i] = fp16_to_fp32(tmp_val);
        }
    }

    template <typename Parser, size_t N>
    static void serialize(Parser& rw, const std::array<float, N>& value)
    {
        auto& stream = rw.getBytestream();

        constexpr size_t NBYTES = 2*N;
        std::array<float, N> buf;
        for (int i=0; i < N; ++i)
        {
            uint16_t tmp = fp32_to_fp16(value[i]);
            if constexpr (endianness == exbip::endian::big)
                buf[i] = exbip::byteswap(tmp);
            else
                buf[i] = tmp;
        }

        char* c = std::bit_cast<char*, const uint16_t*>(buf.data());
        stream.write(c, 2*N);
    }

    template <typename Parser, size_t N>
    static void count(Parser& rw, const float& value)
    {
        auto& stream = rw.getBytestream();
        stream.advance(2*N);
    }
};

/* ------------------ int8 ------------------ */

template<typename Implementation>
struct IInt8ArrayLEMixin : public CRTPBase<Implementation, IInt8ArrayLEMixin<Implementation>>
{
    template <size_t N>
    void rw_int8s_le(std::array<int8_t, N>& value) { this->downcast()->template rw_descriptor<PrimitiveArrayDescriptor<int8_t,  exbip::endian::little>>(value); }
};

template<typename Implementation>
struct IInt8ArrayBEMixin : public CRTPBase<Implementation, IInt8ArrayBEMixin<Implementation>>
{
    template <size_t N>
    void rw_int8s_be(std::array<int8_t, N>& value) { this->downcast()->template rw_descriptor<PrimitiveArrayDescriptor<int8_t,  exbip::endian::big>>(value); }
};

template<typename Implementation>
struct IInt8ArrayEMixin : public CRTPBase<Implementation, IInt8ArrayEMixin<Implementation>>
{
    template <size_t N>
    void rw_int8s_e(std::array<int8_t, N>& value, exbip::endian endianness) 
    {
        if (endianness == exbip::endian::little)
            this->downcast()->rw_int8s_le(value);
        else if (endianness == exbip::endian::big)
            this->downcast()->rw_int8s_be(value);
        else
            throw std::runtime_error("invalid endianness for rw_int8s_e");
    }
};

template<typename Implementation, exbip::endian endianness>
struct IInt8ArrayMixin : public CRTPBase<Implementation, IInt8ArrayMixin<Implementation, endianness>>
{
    template<size_t N>
    void rw_int8s(std::array<int8_t, N>& value) { this->downcast()->template rw_descriptor<PrimitiveArrayDescriptor<int8_t,  endianness>>(value); }
};

/* ------------------ int16 ------------------ */

template<typename Implementation>
struct IInt16ArrayLEMixin : public CRTPBase<Implementation, IInt16ArrayLEMixin<Implementation>>
{
    template <size_t N>
    void rw_int16s_le(std::array<int16_t, N>& value) { this->downcast()->template rw_descriptor<PrimitiveArrayDescriptor<int16_t, exbip::endian::little>>(value); }
};

template<typename Implementation>
struct IInt16ArrayBEMixin : public CRTPBase<Implementation, IInt16ArrayBEMixin<Implementation>>
{
    template <size_t N>
    void rw_int16s_be(std::array<int16_t, N>& value) { this->downcast()->template rw_descriptor<PrimitiveArrayDescriptor<int16_t, exbip::endian::big>>(value); }
};

template<typename Implementation>
struct IInt16ArrayEMixin : public CRTPBase<Implementation, IInt16ArrayEMixin<Implementation>>
{
    template <size_t N>
    void rw_int16s_e(std::array<int16_t, N>& value, exbip::endian endianness) 
    {
        if (endianness == exbip::endian::little)
            this->downcast()->rw_int16s_le(value);
        else if (endianness == exbip::endian::big)
            this->downcast()->rw_int16s_be(value);
        else
            throw std::runtime_error("invalid endianness for rw_int16s_e");
    }
};

template<typename Implementation, exbip::endian endianness>
struct IInt16ArrayMixin : public CRTPBase<Implementation, IInt16ArrayMixin<Implementation, endianness>>
{
    template<size_t N>
    void rw_int16s(std::array<int16_t, N>& value) { this->downcast()->template rw_descriptor<PrimitiveArrayDescriptor<int16_t, endianness>>(value); }
};

/* ------------------ int32 ------------------ */

template<typename Implementation>
struct IInt32ArrayLEMixin : public CRTPBase<Implementation, IInt32ArrayLEMixin<Implementation>>
{
    template <size_t N>
    void rw_int32s_le(std::array<int32_t, N>& value) { this->downcast()->template rw_descriptor<PrimitiveArrayDescriptor<int32_t, exbip::endian::little>>(value); }
};

template<typename Implementation>
struct IInt32ArrayBEMixin : public CRTPBase<Implementation, IInt32ArrayBEMixin<Implementation>>
{
    template <size_t N>
    void rw_int32s_be(std::array<int32_t, N>& value) { this->downcast()->template rw_descriptor<PrimitiveArrayDescriptor<int32_t, exbip::endian::big>>(value); }
};

template<typename Implementation>
struct IInt32ArrayEMixin : public CRTPBase<Implementation, IInt32ArrayEMixin<Implementation>>
{
    template <size_t N>
    void rw_int32s_e(std::array<int32_t, N>& value, exbip::endian endianness) 
    {
        if (endianness == exbip::endian::little)
            this->downcast()->rw_int32s_le(value);
        else if (endianness == exbip::endian::big)
            this->downcast()->rw_int32s_be(value);
        else
            throw std::runtime_error("invalid endianness for rw_int32s_e");
    }
};

template<typename Implementation, exbip::endian endianness>
struct IInt32ArrayMixin : public CRTPBase<Implementation, IInt32ArrayMixin<Implementation, endianness>>
{
    template<size_t N>
    void rw_int32s(std::array<int32_t, N>& value) { this->downcast()->template rw_descriptor<PrimitiveArrayDescriptor<int32_t, endianness>>(value); }
};

/* ------------------ int64 ------------------ */

template<typename Implementation>
struct IInt64ArrayLEMixin : public CRTPBase<Implementation, IInt64ArrayLEMixin<Implementation>>
{
    template <size_t N>
    void rw_int64s_le(std::array<int64_t, N>& value) { this->downcast()->template rw_descriptor<PrimitiveArrayDescriptor<int64_t, exbip::endian::little>>(value); }
};

template<typename Implementation>
struct IInt64ArrayBEMixin : public CRTPBase<Implementation, IInt64ArrayBEMixin<Implementation>>
{
    template <size_t N>
    void rw_int64s_be(std::array<int64_t, N>& value) { this->downcast()->template rw_descriptor<PrimitiveArrayDescriptor<int64_t, exbip::endian::big>>(value); }
};

template<typename Implementation>
struct IInt64ArrayEMixin : public CRTPBase<Implementation, IInt64ArrayEMixin<Implementation>>
{
    template <size_t N>
    void rw_int64s_e(std::array<int64_t, N>& value, exbip::endian endianness) 
    {
        if (endianness == exbip::endian::little)
            this->downcast()->rw_int64s_le(value);
        else if (endianness == exbip::endian::big)
            this->downcast()->rw_int64s_be(value);
        else
            throw std::runtime_error("invalid endianness for rw_int64s_e");
    }
};

template<typename Implementation, exbip::endian endianness>
struct IInt64ArrayMixin : public CRTPBase<Implementation, IInt64ArrayMixin<Implementation, endianness>>
{
    template<size_t N>
    void rw_int64s(std::array<int64_t, N>& value) { this->downcast()->template rw_descriptor<PrimitiveArrayDescriptor<int64_t, endianness>>(value); }
};

/* ------------------ uint8 ------------------ */

template<typename Implementation>
struct IUInt8ArrayLEMixin : public CRTPBase<Implementation, IUInt8ArrayLEMixin<Implementation>>
{
    template <size_t N>
    void rw_uint8s_le(std::array<uint8_t, N>& value) { this->downcast()->template rw_descriptor<PrimitiveArrayDescriptor<uint8_t,  exbip::endian::little>>(value); }
};

template<typename Implementation>
struct IUInt8ArrayBEMixin : public CRTPBase<Implementation, IUInt8ArrayBEMixin<Implementation>>
{
    template <size_t N>
    void rw_uint8s_be(std::array<uint8_t, N>& value) { this->downcast()->template rw_descriptor<PrimitiveArrayDescriptor<uint8_t,  exbip::endian::big>>(value); }
};

template<typename Implementation>
struct IUInt8ArrayEMixin : public CRTPBase<Implementation, IUInt8ArrayEMixin<Implementation>>
{
    template <size_t N>
    void rw_uint8s_e(std::array<uint8_t, N>& value, exbip::endian endianness) 
    {
        if (endianness == exbip::endian::little)
            this->downcast()->rw_uint8s_le(value);
        else if (endianness == exbip::endian::big)
            this->downcast()->rw_uint8s_be(value);
        else
            throw std::runtime_error("invalid endianness for rw_uint8s_e");
    }
};

template<typename Implementation, exbip::endian endianness>
struct IUInt8ArrayMixin : public CRTPBase<Implementation, IUInt8ArrayMixin<Implementation, endianness>>
{
    template<size_t N>
    void rw_uint8s(std::array<uint8_t, N>& value) { this->downcast()->template rw_descriptor<PrimitiveArrayDescriptor<uint8_t,  endianness>>(value); }
};

/* ------------------ uint16 ------------------ */

template<typename Implementation>
struct IUInt16ArrayLEMixin : public CRTPBase<Implementation, IUInt16ArrayLEMixin<Implementation>>
{
    template <size_t N>
    void rw_uint16s_le(std::array<uint16_t, N>& value) { this->downcast()->template rw_descriptor<PrimitiveArrayDescriptor<uint16_t, exbip::endian::little>>(value); }
};

template<typename Implementation>
struct IUInt16ArrayBEMixin : public CRTPBase<Implementation, IUInt16ArrayBEMixin<Implementation>>
{
    template <size_t N>
    void rw_uint16s_be(std::array<uint16_t, N>& value) { this->downcast()->template rw_descriptor<PrimitiveArrayDescriptor<uint16_t, exbip::endian::big>>(value); }
};

template<typename Implementation>
struct IUInt16ArrayEMixin : public CRTPBase<Implementation, IUInt16ArrayEMixin<Implementation>>
{
    template <size_t N>
    void rw_uint16s_e(std::array<uint16_t, N>& value, exbip::endian endianness) 
    {
        if (endianness == exbip::endian::little)
            this->downcast()->rw_uint16s_le(value);
        else if (endianness == exbip::endian::big)
            this->downcast()->rw_uint16s_be(value);
        else
            throw std::runtime_error("invalid endianness for rw_uint16s_e");
    }
};

template<typename Implementation, exbip::endian endianness>
struct IUInt16ArrayMixin : public CRTPBase<Implementation, IUInt16ArrayMixin<Implementation, endianness>>
{
    template<size_t N>
    void rw_uint16s(std::array<uint16_t, N>& value) { this->downcast()->template rw_descriptor<PrimitiveArrayDescriptor<uint16_t, endianness>>(value); }
};

/* ------------------ uint32 ------------------ */

template<typename Implementation>
struct IUInt32ArrayLEMixin : public CRTPBase<Implementation, IUInt32ArrayLEMixin<Implementation>>
{
    template <size_t N>
    void rw_uint32s_le(std::array<uint32_t, N>& value) { this->downcast()->template rw_descriptor<PrimitiveArrayDescriptor<uint32_t, exbip::endian::little>>(value); }
};

template<typename Implementation>
struct IUInt32ArrayBEMixin : public CRTPBase<Implementation, IUInt32ArrayBEMixin<Implementation>>
{
    template <size_t N>
    void rw_uint32s_be(std::array<uint32_t, N>& value) { this->downcast()->template rw_descriptor<PrimitiveArrayDescriptor<uint32_t, exbip::endian::big>>(value); }
};

template<typename Implementation>
struct IUInt32ArrayEMixin : public CRTPBase<Implementation, IUInt32ArrayEMixin<Implementation>>
{
    template <size_t N>
    void rw_uint32s_e(std::array<uint32_t, N>& value, exbip::endian endianness) 
    {
        if (endianness == exbip::endian::little)
            this->downcast()->rw_uint32s_le(value);
        else if (endianness == exbip::endian::big)
            this->downcast()->rw_uint32s_be(value);
        else
            throw std::runtime_error("invalid endianness for rw_uint32s_e");
    }
};

template<typename Implementation, exbip::endian endianness>
struct IUInt32ArrayMixin : public CRTPBase<Implementation, IUInt32ArrayMixin<Implementation, endianness>>
{
    template<size_t N>
    void rw_uint32s(std::array<uint32_t, N>& value) { this->downcast()->template rw_descriptor<PrimitiveArrayDescriptor<uint32_t, endianness>>(value); }
};

/* ------------------ uint64 ------------------ */

template<typename Implementation>
struct IUInt64ArrayLEMixin : public CRTPBase<Implementation, IUInt64ArrayLEMixin<Implementation>>
{
    template <size_t N>
    void rw_uint64s_le(std::array<uint64_t, N>& value) { this->downcast()->template rw_descriptor<PrimitiveArrayDescriptor<uint64_t, exbip::endian::little>>(value); }
};

template<typename Implementation>
struct IUInt64ArrayBEMixin : public CRTPBase<Implementation, IUInt64ArrayBEMixin<Implementation>>
{
    template <size_t N>
    void rw_uint64s_be(std::array<uint64_t, N>& value) { this->downcast()->template rw_descriptor<PrimitiveArrayDescriptor<uint64_t, exbip::endian::big>>(value); }
};

template<typename Implementation>
struct IUInt64ArrayEMixin : public CRTPBase<Implementation, IUInt64ArrayEMixin<Implementation>>
{
    template <size_t N>
    void rw_uint64s_e(std::array<uint64_t, N>& value, exbip::endian endianness) 
    {
        if (endianness == exbip::endian::little)
            this->downcast()->rw_uint64s_le(value);
        else if (endianness == exbip::endian::big)
            this->downcast()->rw_uint64s_be(value);
        else
            throw std::runtime_error("invalid endianness for rw_uint64s_e");
    }
};

template<typename Implementation, exbip::endian endianness>
struct IUInt64ArrayMixin : public CRTPBase<Implementation, IUInt64ArrayMixin<Implementation, endianness>>
{
    template<size_t N>
    void rw_uint64s(std::array<uint64_t, N>& value) { this->downcast()->template rw_descriptor<PrimitiveArrayDescriptor<uint64_t, endianness>>(value); }
};

/* ------------------ float32 ------------------ */

template<typename Implementation>
struct IFloat32ArrayLEMixin : public CRTPBase<Implementation, IFloat32ArrayLEMixin<Implementation>>
{
    template <size_t N>
    void rw_float32s_le(std::array<float, N>& value) { this->downcast()->template rw_descriptor<PrimitiveArrayDescriptor<float, exbip::endian::little>>(value); }
};

template<typename Implementation>
struct IFloat32ArrayBEMixin : public CRTPBase<Implementation, IFloat32ArrayBEMixin<Implementation>>
{
    template <size_t N>
    void rw_float32s_be(std::array<float, N>& value) { this->downcast()->template rw_descriptor<PrimitiveArrayDescriptor<float, exbip::endian::big>>(value); }
};

template<typename Implementation>
struct IFloat32ArrayEMixin : public CRTPBase<Implementation, IFloat32ArrayEMixin<Implementation>>
{
    template <size_t N>
    void rw_float32s_e(std::array<float, N>& value, exbip::endian endianness) 
    {
        if (endianness == exbip::endian::little)
            this->downcast()->rw_float32s_le(value);
        else if (endianness == exbip::endian::big)
            this->downcast()->rw_float32s_be(value);
        else
            throw std::runtime_error("invalid endianness for rw_float32s_e");
    }
};

template<typename Implementation, exbip::endian endianness>
struct IFloat32ArrayMixin : public CRTPBase<Implementation, IFloat32ArrayMixin<Implementation, endianness>>
{
    template<size_t N>
    void rw_float32s(std::array<float, N>& value) { this->downcast()->template rw_descriptor<PrimitiveArrayDescriptor<float, endianness>>(value); }
};

/* ------------------ float64 ------------------ */

template<typename Implementation>
struct IFloat64ArrayLEMixin : public CRTPBase<Implementation, IFloat64ArrayLEMixin<Implementation>>
{
    template <size_t N>
    void rw_float64s_le(std::array<double, N>& value) { this->downcast()->template rw_descriptor<PrimitiveArrayDescriptor<double, exbip::endian::little>>(value); }
};

template<typename Implementation>
struct IFloat64ArrayBEMixin : public CRTPBase<Implementation, IFloat64ArrayBEMixin<Implementation>>
{
    template <size_t N>
    void rw_float64s_be(std::array<double, N>& value) { this->downcast()->template rw_descriptor<PrimitiveArrayDescriptor<double, exbip::endian::big>>(value); }
};

template<typename Implementation>
struct IFloat64ArrayEMixin : public CRTPBase<Implementation, IFloat64ArrayEMixin<Implementation>>
{
    template <size_t N>
    void rw_float64s_e(std::array<double, N>& value, exbip::endian endianness) 
    {
        if (endianness == exbip::endian::little)
            this->downcast()->rw_float64s_le(value);
        else if (endianness == exbip::endian::big)
            this->downcast()->rw_float64s_be(value);
        else
            throw std::runtime_error("invalid endianness for rw_float64s_e");
    }
};

template<typename Implementation, exbip::endian endianness>
struct IFloat64ArrayMixin : public CRTPBase<Implementation, IFloat64ArrayMixin<Implementation, endianness>>
{
    template<size_t N>
    void rw_float64s(std::array<double, N>& value) { this->downcast()->template rw_descriptor<PrimitiveArrayDescriptor<double, endianness>>(value); }
};

/* ------------------ float16 ------------------ */

template<typename Implementation>
struct IFloat16ArrayLEMixin : public CRTPBase<Implementation, IFloat16ArrayLEMixin<Implementation>>
{
    template <size_t N>
    void rw_float16s_le(std::array<float, N>& value) { this->downcast()->template rw_descriptor<Float16ArrayDescriptor<exbip::endian::little>>(value); }
};

template<typename Implementation>
struct IFloat16ArrayBEMixin : public CRTPBase<Implementation, IFloat16ArrayBEMixin<Implementation>>
{
    template <size_t N>
    void rw_float16s_be(std::array<float, N>& value) { this->downcast()->template rw_descriptor<Float16ArrayDescriptor<exbip::endian::big>>(value); }
};

template<typename Implementation>
struct IFloat16ArrayEMixin : public CRTPBase<Implementation, IFloat16ArrayEMixin<Implementation>>
{
    template <size_t N>
    void rw_float16s_e(std::array<float, N>& value, exbip::endian endianness) 
    {
        if (endianness == exbip::endian::little)
            this->downcast()->rw_float16s_le(value);
        else if (endianness == exbip::endian::big)
            this->downcast()->rw_float16s_be(value);
        else
            throw std::runtime_error("invalid endianness for rw_float16s_e");
    }
};

template<typename Implementation, exbip::endian endianness>
struct IFloat16ArrayMixin : public CRTPBase<Implementation, IFloat16ArrayMixin<Implementation, endianness>>
{
    template<size_t N>
    void rw_float16s(std::array<float, N>& value) { this->downcast()->template rw_descriptor<Float16ArrayDescriptor<endianness>>(value); }
};

template<typename Implementation>
struct IPrimitiveArrayMixins
    : public IInt8ArrayLEMixin<Implementation>
    , public IInt8ArrayBEMixin<Implementation>
    , public IInt8ArrayEMixin<Implementation>
    , public IInt16ArrayLEMixin<Implementation>
    , public IInt16ArrayBEMixin<Implementation>
    , public IInt16ArrayEMixin<Implementation>
    , public IInt32ArrayLEMixin<Implementation>
    , public IInt32ArrayBEMixin<Implementation>
    , public IInt32ArrayEMixin<Implementation>
    , public IInt64ArrayLEMixin<Implementation>
    , public IInt64ArrayBEMixin<Implementation>
    , public IInt64ArrayEMixin<Implementation>
    , public IUInt8ArrayLEMixin<Implementation>
    , public IUInt8ArrayBEMixin<Implementation>
    , public IUInt8ArrayEMixin<Implementation>
    , public IUInt16ArrayLEMixin<Implementation>
    , public IUInt16ArrayBEMixin<Implementation>
    , public IUInt16ArrayEMixin<Implementation>
    , public IUInt32ArrayLEMixin<Implementation>
    , public IUInt32ArrayBEMixin<Implementation>
    , public IUInt32ArrayEMixin<Implementation>
    , public IUInt64ArrayLEMixin<Implementation>
    , public IUInt64ArrayBEMixin<Implementation>
    , public IUInt64ArrayEMixin<Implementation>
    , public IFloat32ArrayLEMixin<Implementation>
    , public IFloat32ArrayBEMixin<Implementation>
    , public IFloat32ArrayEMixin<Implementation>
    , public IFloat64ArrayLEMixin<Implementation>
    , public IFloat64ArrayBEMixin<Implementation>
    , public IFloat64ArrayEMixin<Implementation>
    , public IFloat16ArrayLEMixin<Implementation>
    , public IFloat16ArrayBEMixin<Implementation>
    , public IFloat16ArrayEMixin<Implementation>
{};

template<typename Implementation, exbip::endian endianness>
struct IEndianPrimitiveArrayMixins
    : public IInt8ArrayMixin<Implementation, endianness>
    , public IInt16ArrayMixin<Implementation, endianness>
    , public IInt32ArrayMixin<Implementation, endianness>
    , public IInt64ArrayMixin<Implementation, endianness>
    , public IUInt8ArrayMixin<Implementation, endianness>
    , public IUInt16ArrayMixin<Implementation, endianness>
    , public IUInt32ArrayMixin<Implementation, endianness>
    , public IUInt64ArrayMixin<Implementation, endianness>
    , public IFloat32ArrayMixin<Implementation, endianness>
    , public IFloat64ArrayMixin<Implementation, endianness>
    , public IFloat16ArrayMixin<Implementation, endianness>
{};

END_EXBIP_NAMESPACE
