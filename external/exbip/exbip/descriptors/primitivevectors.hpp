#pragma once

#include <exbip/framework/CRTP/Base.hpp>
#include <exbip/utils/Byteswap.hpp>
#include <exbip/utils/FP16.hpp>
#include <exbip/utils/Endian.hpp>


BEGIN_EXBIP_NAMESPACE

template<typename T, exbip::endian endianness>
struct PrimitiveVectorDescriptor
{
    template<typename Parser>
    static void deserialize(Parser& rw, std::vector<T>& value, size_t count)
    {
        auto& stream = rw.getBytestream();
        size_t NBYTES = sizeof(T)*count;
        value.resize(count);

        std::vector<char> buf;
        buf.resize(NBYTES);
        stream.read(&buf[0], NBYTES);
        T* v = std::bit_cast<T*, char*>(&buf[0]);
        if      constexpr (endianness == exbip::endian::little)
        {}
        else if constexpr (endianness == exbip::endian::big)
        {
            for (int i=0; i < count; ++i)
                v[i] = exbip::byteswap(v[i]);
        }
        else
        {
            static_assert(!std::is_same<T, T>::value, "invalid endianness for primitive descriptor");
        }
        
        for (int i=0; i < count; ++i)
            value[i] = v[i];
    }

    template<typename Parser>
    static void serialize(Parser& rw, const std::vector<T>& value, size_t count)
    {
        auto& stream = rw.getBytestream();
        size_t NBYTES = sizeof(T)*count;

        char* c;
        std::vector<T> buf;
        buf.resize(value.size());

        for (int i=0; i < value.size(); ++i)
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

    template<typename Parser>
    static void count(Parser& rw, const std::vector<T>& value, size_t count)
    {
        auto& stream = rw.getBytestream();
        size_t NBYTES = sizeof(T)*count;

        stream.advance(NBYTES);
    }
};


template<exbip::endian endianness>
struct Float16VectorDescriptor
{
    template<typename Parser>
    static void deserialize(Parser& rw, std::vector<float>& value, size_t count)
    {
        auto& stream = rw.getBytestream();
        size_t NBYTES = sizeof(uint16_t)*count;
        value.resize(count);

        std::vector<char> buf;
        buf.resize(NBYTES);
        stream.read(&buf[0], NBYTES);
        uint16_t* v = std::bit_cast<uint16_t*, char*>(&buf[0]);

        for (int i=0; i < count; ++i)
        {
            uint16_t tmp_val = v[i];
            if constexpr (endianness == exbip::endian::big)
                tmp_val = exbip::byteswap(tmp_val);
            value[i] = fp16_to_fp32(tmp_val);
        }
    }

    template<typename Parser>
    static void serialize(Parser& rw, const std::vector<float>& value, size_t count)
    {
        auto& stream = rw.getBytestream();
        size_t NBYTES = sizeof(uint16_t)*count;

        char* c;
        std::vector<uint16_t> buf;
        buf.resize(value.size());

        for (int i=0; i < value.size(); ++i)
        {
            uint16_t tmp = fp32_to_fp16(value[i]);
            if constexpr (endianness == exbip::endian::big)
                buf[i] = exbip::byteswap(tmp);
            else
                buf[i] = tmp;
        }

        c = std::bit_cast<char*, const uint16_t*>(buf.data());
        stream.write(c, NBYTES);
    }

    template<typename Parser>
    static void count(Parser& rw, const std::vector<float>& value, size_t count)
    {
        auto& stream = rw.getBytestream();
        size_t NBYTES = sizeof(uint16_t)*count;

        stream.advance(NBYTES);
    }
};

/* ------------------ int8 ------------------ */

template<typename Implementation>
struct IInt8VectorLEMixin : public CRTPBase<Implementation, IInt8VectorLEMixin<Implementation>>
{
    void rw_int8s_le(std::vector<int8_t>& value, size_t count) { this->downcast()->template rw_descriptor<PrimitiveVectorDescriptor<int8_t,  exbip::endian::little>>(value, count); }
};

template<typename Implementation>
struct IInt8VectorBEMixin : public CRTPBase<Implementation, IInt8VectorBEMixin<Implementation>>
{
    void rw_int8s_be(std::vector<int8_t>& value, size_t count) { this->downcast()->template rw_descriptor<PrimitiveVectorDescriptor<int8_t,  exbip::endian::big>>(value, count); }
};

template<typename Implementation>
struct IInt8VectorEMixin : public CRTPBase<Implementation, IInt8VectorEMixin<Implementation>>
{
    void rw_int8s_e(std::vector<int8_t>& value, size_t count, exbip::endian endianness) 
    {
        if (endianness == exbip::endian::little)
            this->downcast()->rw_int8s_le(value, count);
        else if (endianness == exbip::endian::big)
            this->downcast()->rw_int8s_be(value, count);
        else
            throw std::runtime_error("invalid endianness for rw_int8s_e");
    }
};

template<typename Implementation, exbip::endian endianness>
struct IInt8VectorMixin : public CRTPBase<Implementation, IInt8VectorMixin<Implementation, endianness>>
{
    void rw_int8s(std::vector<int8_t>& value, size_t count) { this->downcast()->template rw_descriptor<PrimitiveVectorDescriptor<int8_t,  endianness>>(value, count); }
};

/* ------------------ int16 ------------------ */

template<typename Implementation>
struct IInt16VectorLEMixin : public CRTPBase<Implementation, IInt16VectorLEMixin<Implementation>>
{
    void rw_int16s_le(std::vector<int16_t>& value, size_t count) { this->downcast()->template rw_descriptor<PrimitiveVectorDescriptor<int16_t, exbip::endian::little>>(value, count); }
};

template<typename Implementation>
struct IInt16VectorBEMixin : public CRTPBase<Implementation, IInt16VectorBEMixin<Implementation>>
{
    void rw_int16s_be(std::vector<int16_t>& value, size_t count) { this->downcast()->template rw_descriptor<PrimitiveVectorDescriptor<int16_t, exbip::endian::big>>(value, count); }
};

template<typename Implementation>
struct IInt16VectorEMixin : public CRTPBase<Implementation, IInt16VectorEMixin<Implementation>>
{
    void rw_int16s_e(std::vector<int16_t>& value, size_t count, exbip::endian endianness) 
    {
        if (endianness == exbip::endian::little)
            this->downcast()->rw_int16s_le(value, count);
        else if (endianness == exbip::endian::big)
            this->downcast()->rw_int16s_be(value, count);
        else
            throw std::runtime_error("invalid endianness for rw_int16s_e");
    }
};

template<typename Implementation, exbip::endian endianness>
struct IInt16VectorMixin : public CRTPBase<Implementation, IInt16VectorMixin<Implementation, endianness>>
{
    void rw_int16s(std::vector<int16_t>& value, size_t count) { this->downcast()->template rw_descriptor<PrimitiveVectorDescriptor<int16_t, endianness>>(value, count); }
};

/* ------------------ int32 ------------------ */

template<typename Implementation>
struct IInt32VectorLEMixin : public CRTPBase<Implementation, IInt32VectorLEMixin<Implementation>>
{
    void rw_int32s_le(std::vector<int32_t>& value, size_t count) { this->downcast()->template rw_descriptor<PrimitiveVectorDescriptor<int32_t, exbip::endian::little>>(value, count); }
};

template<typename Implementation>
struct IInt32VectorBEMixin : public CRTPBase<Implementation, IInt32VectorBEMixin<Implementation>>
{
    void rw_int32s_be(std::vector<int32_t>& value, size_t count) { this->downcast()->template rw_descriptor<PrimitiveVectorDescriptor<int32_t, exbip::endian::big>>(value, count); }
};

template<typename Implementation>
struct IInt32VectorEMixin : public CRTPBase<Implementation, IInt32VectorEMixin<Implementation>>
{
    void rw_int32s_e(std::vector<int32_t>& value, size_t count, exbip::endian endianness) 
    {
        if (endianness == exbip::endian::little)
            this->downcast()->rw_int32s_le(value, count);
        else if (endianness == exbip::endian::big)
            this->downcast()->rw_int32s_be(value, count);
        else
            throw std::runtime_error("invalid endianness for rw_int32s_e");
    }
};

template<typename Implementation, exbip::endian endianness>
struct IInt32VectorMixin : public CRTPBase<Implementation, IInt32VectorMixin<Implementation, endianness>>
{
    void rw_int32s(std::vector<int32_t>& value, size_t count) { this->downcast()->template rw_descriptor<PrimitiveVectorDescriptor<int32_t, endianness>>(value, count); }
};

/* ------------------ int64 ------------------ */

template<typename Implementation>
struct IInt64VectorLEMixin : public CRTPBase<Implementation, IInt64VectorLEMixin<Implementation>>
{
    void rw_int64s_le(std::vector<int64_t>& value, size_t count) { this->downcast()->template rw_descriptor<PrimitiveVectorDescriptor<int64_t, exbip::endian::little>>(value, count); }
};

template<typename Implementation>
struct IInt64VectorBEMixin : public CRTPBase<Implementation, IInt64VectorBEMixin<Implementation>>
{
    void rw_int64s_be(std::vector<int64_t>& value, size_t count) { this->downcast()->template rw_descriptor<PrimitiveVectorDescriptor<int64_t, exbip::endian::big>>(value, count); }
};

template<typename Implementation>
struct IInt64VectorEMixin : public CRTPBase<Implementation, IInt64VectorEMixin<Implementation>>
{
    void rw_int64s_e(std::vector<int64_t>& value, size_t count, exbip::endian endianness) 
    {
        if (endianness == exbip::endian::little)
            this->downcast()->rw_int64s_le(value, count);
        else if (endianness == exbip::endian::big)
            this->downcast()->rw_int64s_be(value, count);
        else
            throw std::runtime_error("invalid endianness for rw_int64s_e");
    }
};

template<typename Implementation, exbip::endian endianness>
struct IInt64VectorMixin : public CRTPBase<Implementation, IInt64VectorMixin<Implementation, endianness>>
{
    void rw_int64s(std::vector<int64_t>& value, size_t count) { this->downcast()->template rw_descriptor<PrimitiveVectorDescriptor<int64_t, endianness>>(value, count); }
};

/* ------------------ uint8 ------------------ */

template<typename Implementation>
struct IUInt8VectorLEMixin : public CRTPBase<Implementation, IUInt8VectorLEMixin<Implementation>>
{
    void rw_uint8s_le(std::vector<uint8_t>& value, size_t count) { this->downcast()->template rw_descriptor<PrimitiveVectorDescriptor<uint8_t,  exbip::endian::little>>(value, count); }
};

template<typename Implementation>
struct IUInt8VectorBEMixin : public CRTPBase<Implementation, IUInt8VectorBEMixin<Implementation>>
{
    void rw_uint8s_be(std::vector<uint8_t>& value, size_t count) { this->downcast()->template rw_descriptor<PrimitiveVectorDescriptor<uint8_t,  exbip::endian::big>>(value, count); }
};

template<typename Implementation>
struct IUInt8VectorEMixin : public CRTPBase<Implementation, IUInt8VectorEMixin<Implementation>>
{
    void rw_uint8s_e(std::vector<uint8_t>& value, size_t count, exbip::endian endianness) 
    {
        if (endianness == exbip::endian::little)
            this->downcast()->rw_uint8s_le(value, count);
        else if (endianness == exbip::endian::big)
            this->downcast()->rw_uint8s_be(value, count);
        else
            throw std::runtime_error("invalid endianness for rw_uint8s_e");
    }
};

template<typename Implementation, exbip::endian endianness>
struct IUInt8VectorMixin : public CRTPBase<Implementation, IUInt8VectorMixin<Implementation, endianness>>
{
    void rw_uint8s(std::vector<uint8_t>& value, size_t count) { this->downcast()->template rw_descriptor<PrimitiveVectorDescriptor<uint8_t,  endianness>>(value, count); }
};

/* ------------------ uint16 ------------------ */

template<typename Implementation>
struct IUInt16VectorLEMixin : public CRTPBase<Implementation, IUInt16VectorLEMixin<Implementation>>
{
    void rw_uint16s_le(std::vector<uint16_t>& value, size_t count) { this->downcast()->template rw_descriptor<PrimitiveVectorDescriptor<uint16_t, exbip::endian::little>>(value, count); }
};

template<typename Implementation>
struct IUInt16VectorBEMixin : public CRTPBase<Implementation, IUInt16VectorBEMixin<Implementation>>
{
    void rw_uint16s_be(std::vector<uint16_t>& value, size_t count) { this->downcast()->template rw_descriptor<PrimitiveVectorDescriptor<uint16_t, exbip::endian::big>>(value, count); }
};

template<typename Implementation>
struct IUInt16VectorEMixin : public CRTPBase<Implementation, IUInt16VectorEMixin<Implementation>>
{
    void rw_uint16s_e(std::vector<uint16_t>& value, size_t count, exbip::endian endianness) 
    {
        if (endianness == exbip::endian::little)
            this->downcast()->rw_uint16s_le(value, count);
        else if (endianness == exbip::endian::big)
            this->downcast()->rw_uint16s_be(value, count);
        else
            throw std::runtime_error("invalid endianness for rw_uint16s_e");
    }
};

template<typename Implementation, exbip::endian endianness>
struct IUInt16VectorMixin : public CRTPBase<Implementation, IUInt16VectorMixin<Implementation, endianness>>
{
    void rw_uint16s(std::vector<uint16_t>& value, size_t count) { this->downcast()->template rw_descriptor<PrimitiveVectorDescriptor<uint16_t, endianness>>(value, count); }
};

/* ------------------ uint32 ------------------ */

template<typename Implementation>
struct IUInt32VectorLEMixin : public CRTPBase<Implementation, IUInt32VectorLEMixin<Implementation>>
{
    void rw_uint32s_le(std::vector<uint32_t>& value, size_t count) { this->downcast()->template rw_descriptor<PrimitiveVectorDescriptor<uint32_t, exbip::endian::little>>(value, count); }
};

template<typename Implementation>
struct IUInt32VectorBEMixin : public CRTPBase<Implementation, IUInt32VectorBEMixin<Implementation>>
{
    void rw_uint32s_be(std::vector<uint32_t>& value, size_t count) { this->downcast()->template rw_descriptor<PrimitiveVectorDescriptor<uint32_t, exbip::endian::big>>(value, count); }
};

template<typename Implementation>
struct IUInt32VectorEMixin : public CRTPBase<Implementation, IUInt32VectorEMixin<Implementation>>
{
    void rw_uint32s_e(std::vector<uint32_t>& value, size_t count, exbip::endian endianness) 
    {
        if (endianness == exbip::endian::little)
            this->downcast()->rw_uint32s_le(value, count);
        else if (endianness == exbip::endian::big)
            this->downcast()->rw_uint32s_be(value, count);
        else
            throw std::runtime_error("invalid endianness for rw_uint32s_e");
    }
};

template<typename Implementation, exbip::endian endianness>
struct IUInt32VectorMixin : public CRTPBase<Implementation, IUInt32VectorMixin<Implementation, endianness>>
{
    void rw_uint32s(std::vector<uint32_t>& value, size_t count) { this->downcast()->template rw_descriptor<PrimitiveVectorDescriptor<uint32_t, endianness>>(value, count); }
};

/* ------------------ uint64 ------------------ */

template<typename Implementation>
struct IUInt64VectorLEMixin : public CRTPBase<Implementation, IUInt64VectorLEMixin<Implementation>>
{
    void rw_uint64s_le(std::vector<uint64_t>& value, size_t count) { this->downcast()->template rw_descriptor<PrimitiveVectorDescriptor<uint64_t, exbip::endian::little>>(value, count); }
};

template<typename Implementation>
struct IUInt64VectorBEMixin : public CRTPBase<Implementation, IUInt64VectorBEMixin<Implementation>>
{
    void rw_uint64s_be(std::vector<uint64_t>& value, size_t count) { this->downcast()->template rw_descriptor<PrimitiveVectorDescriptor<uint64_t, exbip::endian::big>>(value, count); }
};

template<typename Implementation>
struct IUInt64VectorEMixin : public CRTPBase<Implementation, IUInt64VectorEMixin<Implementation>>
{
    void rw_uint64s_e(std::vector<uint64_t>& value, size_t count, exbip::endian endianness) 
    {
        if (endianness == exbip::endian::little)
            this->downcast()->rw_uint64s_le(value, count);
        else if (endianness == exbip::endian::big)
            this->downcast()->rw_uint64s_be(value, count);
        else
            throw std::runtime_error("invalid endianness for rw_uint64s_e");
    }
};

template<typename Implementation, exbip::endian endianness>
struct IUInt64VectorMixin : public CRTPBase<Implementation, IUInt64VectorMixin<Implementation, endianness>>
{
    void rw_uint64s(std::vector<uint64_t>& value, size_t count) { this->downcast()->template rw_descriptor<PrimitiveVectorDescriptor<uint64_t, endianness>>(value, count); }
};

/* ------------------ float32 ------------------ */

template<typename Implementation>
struct IFloat32VectorLEMixin : public CRTPBase<Implementation, IFloat32VectorLEMixin<Implementation>>
{
    void rw_float32s_le(std::vector<float>& value, size_t count) { this->downcast()->template rw_descriptor<PrimitiveVectorDescriptor<float, exbip::endian::little>>(value, count); }
};

template<typename Implementation>
struct IFloat32VectorBEMixin : public CRTPBase<Implementation, IFloat32VectorBEMixin<Implementation>>
{
    void rw_float32s_be(std::vector<float>& value, size_t count) { this->downcast()->template rw_descriptor<PrimitiveVectorDescriptor<float, exbip::endian::big>>(value, count); }
};

template<typename Implementation>
struct IFloat32VectorEMixin : public CRTPBase<Implementation, IFloat32VectorEMixin<Implementation>>
{
    void rw_float32s_e(std::vector<float>& value, size_t count, exbip::endian endianness) 
    {
        if (endianness == exbip::endian::little)
            this->downcast()->rw_float32s_le(value, count);
        else if (endianness == exbip::endian::big)
            this->downcast()->rw_float32s_be(value, count);
        else
            throw std::runtime_error("invalid endianness for rw_float32s_e");
    }
};

template<typename Implementation, exbip::endian endianness>
struct IFloat32VectorMixin : public CRTPBase<Implementation, IFloat32VectorMixin<Implementation, endianness>>
{
    void rw_float32s(std::vector<float>& value, size_t count) { this->downcast()->template rw_descriptor<PrimitiveVectorDescriptor<float, endianness>>(value, count); }
};

/* ------------------ float64 ------------------ */

template<typename Implementation>
struct IFloat64VectorLEMixin : public CRTPBase<Implementation, IFloat64VectorLEMixin<Implementation>>
{
    void rw_float64s_le(std::vector<double>& value, size_t count) { this->downcast()->template rw_descriptor<PrimitiveVectorDescriptor<double, exbip::endian::little>>(value, count); }
};

template<typename Implementation>
struct IFloat64VectorBEMixin : public CRTPBase<Implementation, IFloat64VectorBEMixin<Implementation>>
{
    void rw_float64s_be(std::vector<double>& value, size_t count) { this->downcast()->template rw_descriptor<PrimitiveVectorDescriptor<double, exbip::endian::big>>(value, count); }
};

template<typename Implementation>
struct IFloat64VectorEMixin : public CRTPBase<Implementation, IFloat64VectorEMixin<Implementation>>
{
    void rw_float64s_e(std::vector<double>& value, size_t count, exbip::endian endianness) 
    {
        if (endianness == exbip::endian::little)
            this->downcast()->rw_float64s_le(value, count);
        else if (endianness == exbip::endian::big)
            this->downcast()->rw_float64s_be(value, count);
        else
            throw std::runtime_error("invalid endianness for rw_float64s_e");
    }
};

template<typename Implementation, exbip::endian endianness>
struct IFloat64VectorMixin : public CRTPBase<Implementation, IFloat64VectorMixin<Implementation, endianness>>
{
    void rw_float64s(std::vector<double>& value, size_t count) { this->downcast()->template rw_descriptor<PrimitiveVectorDescriptor<double, endianness>>(value, count); }
};

/* ------------------ float16 ------------------ */

template<typename Implementation>
struct IFloat16VectorLEMixin : public CRTPBase<Implementation, IFloat16VectorLEMixin<Implementation>>
{
    void rw_float16s_le(std::vector<float>& value, size_t count) { this->downcast()->template rw_descriptor<Float16VectorDescriptor<exbip::endian::little>>(value, count); }
};

template<typename Implementation>
struct IFloat16VectorBEMixin : public CRTPBase<Implementation, IFloat16VectorBEMixin<Implementation>>
{
    void rw_float16s_be(std::vector<float>& value, size_t count) { this->downcast()->template rw_descriptor<Float16VectorDescriptor<exbip::endian::big>>(value, count); }
};

template<typename Implementation>
struct IFloat16VectorEMixin : public CRTPBase<Implementation, IFloat16VectorEMixin<Implementation>>
{
    void rw_float16s_e(std::vector<float>& value, size_t count, exbip::endian endianness) 
    {
        if (endianness == exbip::endian::little)
            this->downcast()->rw_float16s_le(value, count);
        else if (endianness == exbip::endian::big)
            this->downcast()->rw_float16s_be(value, count);
        else
            throw std::runtime_error("invalid endianness for rw_float16s_e");
    }
};

template<typename Implementation, exbip::endian endianness>
struct IFloat16VectorMixin : public CRTPBase<Implementation, IFloat16VectorMixin<Implementation, endianness>>
{
    void rw_float16s(std::vector<float>& value, size_t count) { this->downcast()->template rw_descriptor<Float16VectorDescriptor<endianness>>(value, count); }
};

template<typename Implementation>
struct IPrimitiveVectorMixins
    : public IInt8VectorLEMixin<Implementation>
    , public IInt8VectorBEMixin<Implementation>
    , public IInt8VectorEMixin<Implementation>
    , public IInt16VectorLEMixin<Implementation>
    , public IInt16VectorBEMixin<Implementation>
    , public IInt16VectorEMixin<Implementation>
    , public IInt32VectorLEMixin<Implementation>
    , public IInt32VectorBEMixin<Implementation>
    , public IInt32VectorEMixin<Implementation>
    , public IInt64VectorLEMixin<Implementation>
    , public IInt64VectorBEMixin<Implementation>
    , public IInt64VectorEMixin<Implementation>
    , public IUInt8VectorLEMixin<Implementation>
    , public IUInt8VectorBEMixin<Implementation>
    , public IUInt8VectorEMixin<Implementation>
    , public IUInt16VectorLEMixin<Implementation>
    , public IUInt16VectorBEMixin<Implementation>
    , public IUInt16VectorEMixin<Implementation>
    , public IUInt32VectorLEMixin<Implementation>
    , public IUInt32VectorBEMixin<Implementation>
    , public IUInt32VectorEMixin<Implementation>
    , public IUInt64VectorLEMixin<Implementation>
    , public IUInt64VectorBEMixin<Implementation>
    , public IUInt64VectorEMixin<Implementation>
    , public IFloat32VectorLEMixin<Implementation>
    , public IFloat32VectorBEMixin<Implementation>
    , public IFloat32VectorEMixin<Implementation>
    , public IFloat64VectorLEMixin<Implementation>
    , public IFloat64VectorBEMixin<Implementation>
    , public IFloat64VectorEMixin<Implementation>
    , public IFloat16VectorLEMixin<Implementation>
    , public IFloat16VectorBEMixin<Implementation>
    , public IFloat16VectorEMixin<Implementation>
{};

template<typename Implementation, exbip::endian endianness>
struct IEndianPrimitiveVectorMixins
    : public IInt8VectorMixin<Implementation, endianness>
    , public IInt16VectorMixin<Implementation, endianness>
    , public IInt32VectorMixin<Implementation, endianness>
    , public IInt64VectorMixin<Implementation, endianness>
    , public IUInt8VectorMixin<Implementation, endianness>
    , public IUInt16VectorMixin<Implementation, endianness>
    , public IUInt32VectorMixin<Implementation, endianness>
    , public IUInt64VectorMixin<Implementation, endianness>
    , public IFloat32VectorMixin<Implementation, endianness>
    , public IFloat64VectorMixin<Implementation, endianness>
    , public IFloat16VectorMixin<Implementation, endianness>
{};

END_EXBIP_NAMESPACE
