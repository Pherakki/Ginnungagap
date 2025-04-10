#pragma once

#include <exbip/framework/CRTP/Base.hpp>


BEGIN_EXBIP_NAMESPACE


struct SectionExistsDescriptor
{
    template<typename Parser>
    static bool deserialize(Parser& rw, int64_t offset, int64_t count)
    {
        return offset > 0;
    }

    template<typename Parser>
    static bool serialize(Parser& rw, int64_t offset, int64_t count)
    {
        return count > 0;
    }

    template<typename Parser>
    static bool count(Parser& rw, int64_t offset, int64_t count)
    {
        return count > 0;
    }
};


template<typename Implementation>
struct ISectionExistsMixin : public CRTPBase<Implementation, ISectionExistsMixin<Implementation>>
{
    bool section_exists(int64_t offset, int64_t count) { return this->downcast()->template rw_descriptor<SectionExistsDescriptor>(offset, count); }
};


END_EXBIP_NAMESPACE
