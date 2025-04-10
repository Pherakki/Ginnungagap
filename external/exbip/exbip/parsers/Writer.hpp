#pragma once

#include <exbip/framework/parsers/BaseWriter.hpp>
#include <exbip/descriptors/stddescriptors.hpp>


BEGIN_EXBIP_NAMESPACE

/* -------- MemWriter -------- */

struct StdMemWriter : public IMemWriterBase<StdMemWriter, StandardViewDescriptorsLE, StandardViewDescriptorsBE>
{
    using IMemWriterBase<StdMemWriter, StandardViewDescriptorsLE, StandardViewDescriptorsBE>::IMemWriterBase;
};

/* -------- FileWriter -------- */

struct StdFileWriter : public IFileWriterBase<StdFileWriter, StandardViewDescriptorsLE, StandardViewDescriptorsBE>
{
    using IFileWriterBase<StdFileWriter, StandardViewDescriptorsLE, StandardViewDescriptorsBE>::IFileWriterBase;
};

END_EXBIP_NAMESPACE
