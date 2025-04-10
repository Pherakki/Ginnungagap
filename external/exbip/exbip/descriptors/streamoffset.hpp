#pragma once

#include "streammarker.hpp"


BEGIN_EXBIP_NAMESPACE

namespace _detail
{
    template<typename T>
    inline void assertOffset(T actual, size_t expected)
    {
        if (actual != expected) 
            throw std::runtime_error("Stream at at " + std::to_string(actual) + ", expected to be at " + std::to_string(expected)); 
    }

    template<typename T>
    inline void assertOffset(T actual, size_t expected, const std::string& message)
    {
        if (actual != expected)
        {
            std::string msg = message + ": Stream at at " + std::to_string(actual) + ", expected to be at " + std::to_string(expected);
            std::cout << msg << std::endl;
            throw std::runtime_error(msg);
        }
    }
}

template<bool setOffset=false>
struct NavigateStreamOffsetDescriptor
{
    // Signature 1
    template<typename Parser, typename T>
    static void deserialize(Parser& rw, T offset)  { rw.seek(offset); }
    template<typename Parser, typename T>
    static void serialize(Parser& rw, T offset)  { _detail::assertOffset(rw.tell(), offset); }
    template<typename Parser, typename T>
    static void count(Parser& rw, T offset) { }
    template<typename Parser, typename T>
    static void calculateOffsets(Parser& rw, T& offset) 
    { 
        if constexpr(setOffset)
            offset = static_cast<T>(rw.tell());
    }

    // Signature 2
    template<typename Parser, typename T>
    static void deserialize(Parser& rw, T offset, const std::string& msg)  { rw.seek(offset); }
    template<typename Parser, typename T>
    static void serialize(Parser& rw, T offset, const std::string& msg)  { _detail::assertOffset(rw.tell(), offset, msg); }
    template<typename Parser, typename T>
    static void count(Parser& rw, T offset, const std::string& msg) { }
    template<typename Parser, typename T>
    static void calculateOffsets(Parser& rw, T& offset, const std::string& msg) 
    { 
        if constexpr(setOffset)
            offset = static_cast<T>(rw.tell());
    }

    // Signature 3
    template<typename Parser, typename T>
    static void deserialize(Parser& rw, T offset, const OffsetMarker& marker)  
    { rw.seek(offset); }
    template<typename Parser, typename T>
    static void serialize(Parser& rw, T offset, const OffsetMarker& marker)
    { _detail::assertOffset(rw.tell(), offset); }
    template<typename Parser, typename T>
    static void count(Parser& rw, T offset, const OffsetMarker& marker) 
    { }
    template<typename Parser, typename T>
    static void calculateOffsets(Parser& rw, T& offset, const OffsetMarker& marker) 
    { 
        if constexpr(setOffset)
            offset = static_cast<T>(rw.tell());
        marker.notify(offset);
    }

    // Signature 4
    template<typename Parser, typename T>
    static void deserialize(Parser& rw, T offset, const std::string& msg, const OffsetMarker& marker)  
    { rw.seek(offset); }
    template<typename Parser, typename T>
    static void serialize(Parser& rw, T offset, const std::string& msg, const OffsetMarker& marker)
    { _detail::assertOffset(rw.tell(), offset, msg); }
    template<typename Parser, typename T>
    static void count(Parser& rw, T offset, const std::string& msg, const OffsetMarker& marker) 
    { }
    template<typename Parser, typename T>
    static void calculateOffsets(Parser& rw, T& offset, const std::string& msg, const OffsetMarker& marker) 
    { 
        if constexpr(setOffset)
            offset = static_cast<T>(rw.tell());
        marker.notify(offset);
    }
};


template<bool setOffset=false>
struct VerifyStreamOffsetDescriptor
{
    // Signature 1
    template<typename Parser, typename T>
    static void deserialize(Parser& rw, T offset)  { _detail::assertOffset(rw.tell(), offset); }
    template<typename Parser, typename T>
    static void serialize(Parser& rw, T offset)  { _detail::assertOffset(rw.tell(), offset); }
    template<typename Parser, typename T>
    static void count(Parser& rw, T offset) { }
    template<typename Parser, typename T>
    static void calculateOffsets(Parser& rw, T& offset) 
    { 
        if constexpr(setOffset)
            offset = static_cast<T>(rw.tell());
    }

    // Signature 2
    template<typename Parser, typename T>
    static void deserialize(Parser& rw, T offset, const std::string& msg)  { _detail::assertOffset(rw.tell(), offset, msg); }
    template<typename Parser, typename T>
    static void serialize(Parser& rw, T offset, const std::string& msg)  { _detail::assertOffset(rw.tell(), offset, msg); }
    template<typename Parser, typename T>
    static void count(Parser& rw, T offset, const std::string& msg) { }
    template<typename Parser, typename T>
    static void calculateOffsets(Parser& rw, T& offset, const std::string& msg) 
    { 
        if constexpr(setOffset)
            offset = static_cast<T>(rw.tell());
    }

    // Signature 3
    template<typename Parser, typename T>
    static void deserialize(Parser& rw, T offset, const OffsetMarker& marker)  
    { _detail::assertOffset(rw.tell(), offset); }
    template<typename Parser, typename T>
    static void serialize(Parser& rw, T offset, const OffsetMarker& marker)
    { _detail::assertOffset(rw.tell(), offset); }
    template<typename Parser, typename T>
    static void count(Parser& rw, T offset, const OffsetMarker& marker) 
    { }
    template<typename Parser, typename T>
    static void calculateOffsets(Parser& rw, T& offset, const OffsetMarker& marker) 
    { 
        if constexpr(setOffset)
            offset = static_cast<T>(rw.tell());
        marker.notify(offset);
    }

    // Signature 4
    template<typename Parser, typename T>
    static void deserialize(Parser& rw, T offset, const std::string& msg, const OffsetMarker& marker)  
    { _detail::assertOffset(rw.tell(), offset, msg); }
    template<typename Parser, typename T>
    static void serialize(Parser& rw, T offset, const std::string& msg, const OffsetMarker& marker)
    { _detail::assertOffset(rw.tell(), offset, msg); }
    template<typename Parser, typename T>
    static void count(Parser& rw, T offset, const std::string& msg, const OffsetMarker& marker) 
    { }
    template<typename Parser, typename T>
    static void calculateOffsets(Parser& rw, T& offset, const std::string& msg, const OffsetMarker& marker) 
    { 
        if constexpr(setOffset)
            offset = static_cast<T>(rw.tell());
        marker.notify(offset);
    }
};


template<typename Implementation>
struct IVerifyStreamOffsetMixin : public CRTPBase<Implementation, IVerifyStreamOffsetMixin<Implementation>>
{
    /* Non-setting */
    template<typename T>
    void verify_stream_offset(T offset)
    {
        this->downcast()->template rw_descriptor<VerifyStreamOffsetDescriptor<false>>(offset);
    }

    template<typename T>
    void verify_stream_offset(T offset, const std::string& message)
    {
        this->downcast()->template rw_descriptor<VerifyStreamOffsetDescriptor<false>>(offset, message);
    }

    template<typename T>
    void verify_stream_offset(T offset, OffsetMarker& marker)
    {
        this->downcast()->template rw_descriptor<VerifyStreamOffsetDescriptor<false>>(offset, marker);
    }

    template<typename T>
    void verify_stream_offset(T offset, const std::string& message, OffsetMarker& marker)
    {
        this->downcast()->template rw_descriptor<VerifyStreamOffsetDescriptor<false>>(offset, message, message);
    }

    /* Setting */
    template<typename T>
    void verify_and_set_stream_offset(T& offset)
    {
        this->downcast()->template rw_descriptor<VerifyStreamOffsetDescriptor<true>>(offset);
    }

    template<typename T>
    void verify_and_set_stream_offset(T& offset, const std::string& message)
    {
        this->downcast()->template rw_descriptor<VerifyStreamOffsetDescriptor<true>>(offset, message);
    }

    template<typename T>
    void verify_and_set_stream_offset(T& offset, OffsetMarker& marker)
    {
        this->downcast()->template rw_descriptor<VerifyStreamOffsetDescriptor<true>>(offset, marker);
    }

    template<typename T>
    void verify_and_set_stream_offset(T& offset, const std::string& message, OffsetMarker& marker)
    {
        this->downcast()->template rw_descriptor<VerifyStreamOffsetDescriptor<true>>(offset, message, marker);
    }
};


template<typename Implementation>
struct IEnforceStreamOffsetMixin : public CRTPBase<Implementation, IEnforceStreamOffsetMixin<Implementation>>
{
    /* Non-setting */
    template<typename T>
    void enforce_stream_offset(T offset)
    {
        this->downcast()->template rw_descriptor<NavigateStreamOffsetDescriptor<false>>(offset);
    }

    template<typename T>
    void enforce_stream_offset(T offset, const std::string& message)
    {
        this->downcast()->template rw_descriptor<NavigateStreamOffsetDescriptor<false>>(offset, message);
    }

    template<typename T>
    void enforce_stream_offset(T offset, OffsetMarker& marker)
    {
        this->downcast()->template rw_descriptor<NavigateStreamOffsetDescriptor<false>>(offset, marker);
    }

    template<typename T>
    void enforce_stream_offset(T offset, const std::string& message, OffsetMarker& marker)
    {
        this->downcast()->template rw_descriptor<NavigateStreamOffsetDescriptor<false>>(offset, message, message);
    }

    /* Setting */
    template<typename T>
    void enforce_and_set_stream_offset(T& offset)
    {
        this->downcast()->template rw_descriptor<NavigateStreamOffsetDescriptor<true>>(offset);
    }

    template<typename T>
    void enforce_and_set_stream_offset(T& offset, const std::string& message)
    {
        this->downcast()->template rw_descriptor<NavigateStreamOffsetDescriptor<true>>(offset, message);
    }

    template<typename T>
    void enforce_and_set_stream_offset(T& offset, OffsetMarker& marker)
    {
        this->downcast()->template rw_descriptor<NavigateStreamOffsetDescriptor<true>>(offset, marker);
    }

    template<typename T>
    void enforce_and_set_stream_offset(T& offset, const std::string& message, OffsetMarker& marker)
    {
        this->downcast()->template rw_descriptor<NavigateStreamOffsetDescriptor<true>>(offset, message, marker);
    }
};

END_EXBIP_NAMESPACE
