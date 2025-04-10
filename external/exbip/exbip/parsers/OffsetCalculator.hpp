#pragma once

#include "../descriptors/stddescriptors.hpp"
#include "../framework/parsers/BaseOffsetCalculator.hpp"

BEGIN_EXBIP_NAMESPACE

struct StdOffsetCalculator : public IOffsetCalculatorBase<StdOffsetCalculator, StandardViewDescriptorsLE, StandardViewDescriptorsBE>
{
    using IOffsetCalculatorBase<StdOffsetCalculator, StandardViewDescriptorsLE, StandardViewDescriptorsBE>::IOffsetCalculatorBase;
};

END_EXBIP_NAMESPACE
