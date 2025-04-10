#pragma once

#include <exbip/framework/parsers/BaseReader.hpp>
#include <exbip/descriptors/stddescriptors.hpp>


BEGIN_EXBIP_NAMESPACE

/* -------- MemReader -------- */


struct StdMemReader : public IMemReaderBase<StdMemReader, StandardViewDescriptorsLE, StandardViewDescriptorsBE>
{
    using IMemReaderBase<StdMemReader, StandardViewDescriptorsLE, StandardViewDescriptorsBE>::IMemReaderBase;
};

/* -------- FileReader -------- */

struct StdFileReader : public IFileReaderBase<StdFileReader, StandardViewDescriptorsLE, StandardViewDescriptorsBE>
{
    using IFileReaderBase<StdFileReader, StandardViewDescriptorsLE, StandardViewDescriptorsBE>::IFileReaderBase;
};

END_EXBIP_NAMESPACE
