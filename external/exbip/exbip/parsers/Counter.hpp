#pragma once

#include "../descriptors/stddescriptors.hpp"
#include "../framework/parsers/BaseCounter.hpp"

BEGIN_EXBIP_NAMESPACE

struct StdCounter : public ICounterBase<StdCounter, StandardViewDescriptorsLE, StandardViewDescriptorsBE>
{
    using ICounterBase<StdCounter, StandardViewDescriptorsLE, StandardViewDescriptorsBE>::ICounterBase;
};

END_EXBIP_NAMESPACE
