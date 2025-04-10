#pragma once

#include <array>
#include <exbip/exbip.hpp>
#include "ParamDefinitions/Definition.hpp"
#include "GinnungagapCore/utils/StringUtils.hpp"


struct Parameter
{
    union BasicData
    {
        int8_t   m_int8;
        int16_t  m_int16;
        int32_t  m_int32;
        int64_t  m_int64;
        uint8_t  m_uint8;
        uint16_t m_uint16;
        uint32_t m_uint32;
        uint64_t m_uint64;
        float    m_float;
        double   m_double;
        std::array<uint8_t, 4> m_char4;
        std::array<float, 4>   m_float4;
    };
    
    ElementType type;
    BasicData   data;
    std::string string;

    bool is_int() const noexcept
    {
        return (this->type == ElementType::Int8 )
            || (this->type == ElementType::Int16)
            || (this->type == ElementType::Int32)
            || (this->type == ElementType::Int64);
    }
    
    bool is_uint() const noexcept
    {
        return (this->type == ElementType::UInt8 )
            || (this->type == ElementType::UInt16)
            || (this->type == ElementType::UInt32)
            || (this->type == ElementType::UInt64);
    }

    bool is_float() const noexcept
    {
        return (this->type == ElementType::Float32) || (this->type == ElementType::Float64);
    }

    bool is_string() const noexcept
    {
        return (this->type == ElementType::SJISString) || (this->type == ElementType::UTF8String);
    }

    template<typename T>
    void extract_uint(T& out)
    {
        switch (this->type)
        {
            case ElementType::UInt64:
                out = data.m_uint64;
                break;
            case ElementType::UInt32:
                out = data.m_uint32;
                break;
            case ElementType::UInt16:
                out = data.m_uint16;
                break;
            case ElementType::UInt8:
                out = data.m_uint8;
                break;
            default:
                throw std::runtime_error("Element type is not an integer");
        }
    }

    template<typename T>
    void extract_int(T& out)
    {
        switch (this->type)
        {
            case ElementType::Int64:
                out = data.m_uint64;
                break;
            case ElementType::Int32:
                out = data.m_uint32;
                break;
            case ElementType::Int16:
                out = data.m_uint16;
                break;
            case ElementType::Int8:
                out = data.m_uint8;
                break;
            default:
                throw std::runtime_error("Element type is not an integer");
        }
    }

    template<typename T>
    void extract_integer(T& out)
    {
        switch (this->type)
        {
            case ElementType::UInt64:
                out = data.m_uint64;
                break;
            case ElementType::UInt32:
                out = data.m_uint32;
                break;
            case ElementType::UInt16:
                out = data.m_uint16;
                break;
            case ElementType::UInt8:
                out = data.m_uint8;
                break;
            case ElementType::Int64:
                out = data.m_uint64;
                break;
            case ElementType::Int32:
                out = data.m_uint32;
                break;
            case ElementType::Int16:
                out = data.m_uint16;
                break;
            case ElementType::Int8:
                out = data.m_uint8;
                break;
            default:
                throw std::runtime_error("Element type is not an integer");
        }
    }
    
    std::string asString() const
    {
        switch(this->type)
        {
            case ElementType::Int8:    return std::to_string(this->data.m_int8);
            case ElementType::Int16:   return std::to_string(this->data.m_int16);
            case ElementType::Int32:   return std::to_string(this->data.m_int32);
            case ElementType::Int64:
            case ElementType::Asset:   return std::to_string(this->data.m_int64);
            case ElementType::Pad8:
            case ElementType::UInt8:   return std::to_string(this->data.m_uint8);
            case ElementType::Pad16:
            case ElementType::UInt16:  return std::to_string(this->data.m_uint16);
            case ElementType::Pad32:
            case ElementType::Offset32:
            case ElementType::UInt32:  return std::to_string(this->data.m_uint32);
            case ElementType::Pad64:
            case ElementType::Offset64:
            case ElementType::UInt64:  return std::to_string(this->data.m_uint64);
            case ElementType::Float32: return std::to_string(this->data.m_float);
            case ElementType::Float64: return std::to_string(this->data.m_double);
            case ElementType::SJISString: return stringmanip::sjisToUtf8(this->string);
            case ElementType::UTF8String: return this->string;
            case ElementType::CharColor:
            {
                const auto& data = this->data.m_char4;
                return std::to_string(data[0]) + "_" + 
                    std::to_string(data[1]) + "_" +
                    std::to_string(data[2]) + "_" +
                    std::to_string(data[3]);
            }
            case ElementType::FloatColor:
            {
                const auto& data = this->data.m_float4;
                return std::to_string(data[0]) + "_" + 
                    std::to_string(data[1]) + "_" + 
                    std::to_string(data[2]) + "_" +
                    std::to_string(data[3]);
            }
            default:
            {
                throw std::runtime_error("Unknown data type");
            }
        }
    }

    template<typename Parser>
    void exbip_rw(Parser& rw, ElementType dtype, const std::string& type_name, MXECStringBank& sbank)
    {
        this->type = dtype;
        switch (this->type)
        {
            case ElementType::Int8:
            {
                rw.rw_int8(this->data.m_int8);
                break;
            }
            case ElementType::Int16:
            {
                rw.rw_int16(this->data.m_int16);
                break;
            }
            case ElementType::Int32:
            {
                rw.rw_int32(this->data.m_int32);
                break;
            }
            case ElementType::Int64:
            {
                rw.rw_int64(this->data.m_int64);
                break;
            }
            case ElementType::UInt8:
            {
                rw.rw_uint8(this->data.m_uint8);
                break;
            }
            case ElementType::UInt16:
            {
                rw.rw_uint16(this->data.m_uint16);
                break;
            }
            case ElementType::UInt32:
            {
                rw.rw_uint32(this->data.m_uint32);
                break;
            }
            case ElementType::UInt64:
            {
                rw.rw_uint64(this->data.m_uint64);
                break;
            }
            case ElementType::Pad8:
            {
                rw.rw_uint8(this->data.m_uint8);
                break;
            }
            case ElementType::Pad16:
            {
                rw.rw_uint16(this->data.m_uint16);
                break;
            }
            case ElementType::Pad32:
            {
                rw.rw_uint32(this->data.m_uint32);
                break;
            }
            case ElementType::Pad64:
            {
                rw.rw_uint64(this->data.m_uint64);
                break;
            }
            case ElementType::Asset:
            {
                rw.rw_int64(this->data.m_int64);
                break;
            }
            case ElementType::Offset32:
            {
                rw.rw_offset32(this->data.m_uint32);
                break;
            }
            case ElementType::Offset64:
            {
                rw.rw_offset64(this->data.m_uint64);
                break;
            }
            case ElementType::Float32:
            {
                rw.rw_float32(this->data.m_float);
                break;
            }
            case ElementType::Float64:
            {
                rw.rw_float64(this->data.m_double);
                break;
            }
            case ElementType::SJISString:
            {
                rw.rw_memoized_string64(this->data.m_uint64, this->string, sbank);
                break;
            }
            case ElementType::UTF8String:
            {
                rw.rw_memoized_string64(this->data.m_uint64, this->string, sbank);
                break;
            }
            case ElementType::CharColor:
            {
                rw.rw_uint8s(this->data.m_char4);
                break;
            }
            case ElementType::FloatColor:
            {
                rw.rw_float32s(this->data.m_float4);
                break;
            }
            default:
            {
                throw std::runtime_error("Unknown MXE parameter data type '" + type_name + "'");
            }
        }
    }
};
