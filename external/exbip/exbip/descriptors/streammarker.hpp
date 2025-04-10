#pragma once

#include <exbip/framework/CRTP/Base.hpp>
#include <memory>
#include <vector>
#include <iostream>

BEGIN_EXBIP_NAMESPACE

class OffsetReference
{
public:
    virtual ~OffsetReference() {}
    virtual void receive(int64_t offset) = 0;
    virtual int64_t getOffset() = 0;
    virtual std::string name () { return "BaseOffsetReferenceClass"; }
};

template<typename T>
class TemplateOffsetReference : public OffsetReference
{
public:
    T& offset;

    TemplateOffsetReference(T& ref)
        : offset{ref}
    {}

    void receive(int64_t offset) override 
    { 
        this->offset = static_cast<T>(offset); 
    }
    
    virtual int64_t getOffset() override { return static_cast<int64_t>(this->offset); };

    virtual std::string name () override { return "TemplateOffsetReference"; }
};

using Int32OffsetReference = TemplateOffsetReference<int32_t>;
using Int64OffsetReference = TemplateOffsetReference<int64_t>;
using UInt32OffsetReference = TemplateOffsetReference<uint32_t>;
using UInt64OffsetReference = TemplateOffsetReference<uint64_t>;

class OffsetMarker
{
public:
    OffsetMarker() = default;
    void subscribe(int32_t& v)  { offsets.push_back(std::make_unique<Int32OffsetReference>(v)); }
    void subscribe(int64_t& v)  { offsets.push_back(std::make_unique<Int64OffsetReference>(v)); }
    void subscribe(uint32_t& v) { offsets.push_back(std::make_unique<UInt32OffsetReference>(v)); }
    void subscribe(uint64_t& v) { offsets.push_back(std::make_unique<UInt64OffsetReference>(v)); }
    template<typename T, typename... Args>
    void subscribe(Args&&... args) { offsets.push_back(std::make_unique<T>(args...)); }

    void notify(int64_t offset) const
    {
        for (const std::shared_ptr<OffsetReference>& o : this->offsets)
        {
            o->receive(offset);
        }
    }
private:
    // These should be unique_ptrs but I'm too lazy to get the semantics right currently
    std::vector<std::shared_ptr<OffsetReference>> offsets;
};

class OffsetDeltaMarker : public OffsetMarker
{
public:
    void setBase(int64_t base)
    {
        this->base = base;
    }

    void notify(int64_t offset) const
    {
        OffsetMarker::notify(offset - base);
    }
private:
    int64_t base=0;
};

struct DispatchMarkerDescriptor
{
    template<typename Parser, typename OffsetMarker>
    static void deserialize(Parser& rw, const OffsetMarker& value) {}
    template<typename Parser, typename OffsetMarker>
    static void deserialize(Parser& rw, const OffsetMarker& value, int64_t offset) {}

    template<typename Parser, typename OffsetMarker>
    static void serialize(Parser& rw, const OffsetMarker& value) {}
    template<typename Parser, typename OffsetMarker>
    static void serialize(Parser& rw, const OffsetMarker& value, int64_t offset) {}

    template<typename Parser, typename OffsetMarker>
    static void count(Parser& rw, const OffsetMarker& value) {}
    template<typename Parser, typename OffsetMarker>
    static void count(Parser& rw, const OffsetMarker& value, int64_t offset) {}

    template<typename Parser, typename OffsetMarker>
    static void calculateOffsets(Parser& rw, const OffsetMarker& value) { value.notify(rw.tell());}
    template<typename Parser, typename OffsetMarker>
    static void calculateOffsets(Parser& rw, const OffsetMarker& value, int64_t offset) { value.notify(offset); }
};


struct SetMarkerBaseDescriptor
{
    template<typename Parser>
    static void deserialize(Parser& rw, const OffsetDeltaMarker& value) {}
    template<typename Parser>
    static void deserialize(Parser& rw, const OffsetDeltaMarker& value, int64_t offset) {}

    template<typename Parser>
    static void serialize(Parser& rw, const OffsetDeltaMarker& value) {}
    template<typename Parser>
    static void serialize(Parser& rw, const OffsetDeltaMarker& value, int64_t offset) {}

    template<typename Parser>
    static void count(Parser& rw, const OffsetDeltaMarker& value) {}
    template<typename Parser>
    static void count(Parser& rw, const OffsetDeltaMarker& value, int64_t offset) {}

    template<typename Parser>
    static void calculateOffsets(Parser& rw, OffsetDeltaMarker& value) { value.setBase(rw.tell());}
    template<typename Parser>
    static void calculateOffsets(Parser& rw, OffsetDeltaMarker& value, int64_t offset) { value.setBase(offset); }
};



template<typename Implementation>
struct IDispatchMarkerMixin : public CRTPBase<Implementation, IDispatchMarkerMixin<Implementation>>
{
    template<typename OffsetMarker>
    void dispatch_marker(const OffsetMarker& marker) { this->downcast()->template rw_descriptor<DispatchMarkerDescriptor>(marker); }
    template<typename OffsetMarker>
    void dispatch_marker(const OffsetMarker& marker, int64_t offset) { this->downcast()->template rw_descriptor<DispatchMarkerDescriptor>(marker, offset); }
};


template<typename Implementation>
struct ISetMarkerBaseMixin : public CRTPBase<Implementation, ISetMarkerBaseMixin<Implementation>>
{
    void set_marker_base(OffsetDeltaMarker& marker) { this->template staticDispatchDescriptor<SetMarkerBaseDescriptor>(marker); }
    void set_marker_base(OffsetDeltaMarker& marker, int64_t offset) { this->downcast()->template rw_descriptor<SetMarkerBaseDescriptor>(marker, offset); }
};


END_EXBIP_NAMESPACE
