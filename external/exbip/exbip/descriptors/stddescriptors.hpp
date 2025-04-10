#pragma once

#include "bytestring.hpp"
#include "descriptor.hpp"
#include "iterator.hpp"
#include "object.hpp"
#include "primitives.hpp"
#include "primitivearrays.hpp"
#include "primitivevectors.hpp"
#include "section.hpp"
#include "streamalignment.hpp"
#include "streammarker.hpp"
#include "streamoffset.hpp"
#include "streampadding.hpp"


BEGIN_EXBIP_NAMESPACE

template<typename Implementation>
struct StandardDescriptors
    /* -------- bytestrings -------- */
    : IBytestringMixin<Implementation>
    , IPeekBytestringMixin<Implementation>
    , ICBytestringMixin<Implementation>
    /* -------- primitives -------- */
    , IPrimitiveMixins<Implementation>
    , IPrimitiveArrayMixins<Implementation>
    , IPrimitiveVectorMixins<Implementation>
    /* -------- stream manipulation -------- */
    , IDispatchMarkerMixin<Implementation>
    , ISetMarkerBaseMixin<Implementation>
    , IStreamAlignmentMixin<Implementation>
    , IStreamPaddingMixin<Implementation>
    , ISectionExistsMixin<Implementation>
    , IVerifyStreamOffsetMixin<Implementation>
    , IEnforceStreamOffsetMixin<Implementation>

    /* -------- object -------- */
    , IObjectMixin<Implementation>
    , IObjectVectorMixin<Implementation>
    , DescriptorDescriptor<Implementation>

    /* -------- iterables --------*/
    , IVectorIteratorMixin<Implementation>
{
    using IPrimitiveArrayMixins<Implementation>::rw_int8s_le;
    using IPrimitiveVectorMixins<Implementation>::rw_int8s_le;
    using IPrimitiveArrayMixins<Implementation>::rw_int16s_le;
    using IPrimitiveVectorMixins<Implementation>::rw_int16s_le;
    using IPrimitiveArrayMixins<Implementation>::rw_int32s_le;
    using IPrimitiveVectorMixins<Implementation>::rw_int32s_le;
    using IPrimitiveArrayMixins<Implementation>::rw_int64s_le;
    using IPrimitiveVectorMixins<Implementation>::rw_int64s_le;

    using IPrimitiveArrayMixins<Implementation>::rw_uint8s_le;
    using IPrimitiveVectorMixins<Implementation>::rw_uint8s_le;
    using IPrimitiveArrayMixins<Implementation>::rw_uint16s_le;
    using IPrimitiveVectorMixins<Implementation>::rw_uint16s_le;
    using IPrimitiveArrayMixins<Implementation>::rw_uint32s_le;
    using IPrimitiveVectorMixins<Implementation>::rw_uint32s_le;
    using IPrimitiveArrayMixins<Implementation>::rw_uint64s_le;
    using IPrimitiveVectorMixins<Implementation>::rw_uint64s_le;

    using IPrimitiveArrayMixins<Implementation>::rw_float16s_le;
    using IPrimitiveVectorMixins<Implementation>::rw_float16s_le;
    using IPrimitiveArrayMixins<Implementation>::rw_float32s_le;
    using IPrimitiveVectorMixins<Implementation>::rw_float32s_le;
    using IPrimitiveArrayMixins<Implementation>::rw_float64s_le;
    using IPrimitiveVectorMixins<Implementation>::rw_float64s_le;

    using IPrimitiveArrayMixins<Implementation>::rw_int8s_be;
    using IPrimitiveVectorMixins<Implementation>::rw_int8s_be;
    using IPrimitiveArrayMixins<Implementation>::rw_int16s_be;
    using IPrimitiveVectorMixins<Implementation>::rw_int16s_be;
    using IPrimitiveArrayMixins<Implementation>::rw_int32s_be;
    using IPrimitiveVectorMixins<Implementation>::rw_int32s_be;
    using IPrimitiveArrayMixins<Implementation>::rw_int64s_be;
    using IPrimitiveVectorMixins<Implementation>::rw_int64s_be;

    using IPrimitiveArrayMixins<Implementation>::rw_uint8s_be;
    using IPrimitiveVectorMixins<Implementation>::rw_uint8s_be;
    using IPrimitiveArrayMixins<Implementation>::rw_uint16s_be;
    using IPrimitiveVectorMixins<Implementation>::rw_uint16s_be;
    using IPrimitiveArrayMixins<Implementation>::rw_uint32s_be;
    using IPrimitiveVectorMixins<Implementation>::rw_uint32s_be;
    using IPrimitiveArrayMixins<Implementation>::rw_uint64s_be;
    using IPrimitiveVectorMixins<Implementation>::rw_uint64s_be;

    using IPrimitiveArrayMixins<Implementation>::rw_float16s_be;
    using IPrimitiveVectorMixins<Implementation>::rw_float16s_be;
    using IPrimitiveArrayMixins<Implementation>::rw_float32s_be;
    using IPrimitiveVectorMixins<Implementation>::rw_float32s_be;
    using IPrimitiveArrayMixins<Implementation>::rw_float64s_be;
    using IPrimitiveVectorMixins<Implementation>::rw_float64s_be;

    using IPrimitiveArrayMixins<Implementation>::rw_int8s_e;
    using IPrimitiveVectorMixins<Implementation>::rw_int8s_e;
    using IPrimitiveArrayMixins<Implementation>::rw_int16s_e;
    using IPrimitiveVectorMixins<Implementation>::rw_int16s_e;
    using IPrimitiveArrayMixins<Implementation>::rw_int32s_e;
    using IPrimitiveVectorMixins<Implementation>::rw_int32s_e;
    using IPrimitiveArrayMixins<Implementation>::rw_int64s_e;
    using IPrimitiveVectorMixins<Implementation>::rw_int64s_e;

    using IPrimitiveArrayMixins<Implementation>::rw_uint8s_e;
    using IPrimitiveVectorMixins<Implementation>::rw_uint8s_e;
    using IPrimitiveArrayMixins<Implementation>::rw_uint16s_e;
    using IPrimitiveVectorMixins<Implementation>::rw_uint16s_e;
    using IPrimitiveArrayMixins<Implementation>::rw_uint32s_e;
    using IPrimitiveVectorMixins<Implementation>::rw_uint32s_e;
    using IPrimitiveArrayMixins<Implementation>::rw_uint64s_e;
    using IPrimitiveVectorMixins<Implementation>::rw_uint64s_e;

    using IPrimitiveArrayMixins<Implementation>::rw_float16s_e;
    using IPrimitiveVectorMixins<Implementation>::rw_float16s_e;
    using IPrimitiveArrayMixins<Implementation>::rw_float32s_e;
    using IPrimitiveVectorMixins<Implementation>::rw_float32s_e;
    using IPrimitiveArrayMixins<Implementation>::rw_float64s_e;
    using IPrimitiveVectorMixins<Implementation>::rw_float64s_e;
};

template<typename Implementation, exbip::endian endianness>
struct StandardEndianDescriptors
    /* -------- primitives -------- */
    : IEndianPrimitiveMixins<Implementation, endianness>
    , IEndianPrimitiveArrayMixins<Implementation, endianness>
    , IEndianPrimitiveVectorMixins<Implementation, endianness>
{
    using IEndianPrimitiveArrayMixins<Implementation, endianness>::rw_int8s;
    using IEndianPrimitiveVectorMixins<Implementation, endianness>::rw_int8s;
    using IEndianPrimitiveArrayMixins<Implementation, endianness>::rw_int16s;
    using IEndianPrimitiveVectorMixins<Implementation, endianness>::rw_int16s;
    using IEndianPrimitiveArrayMixins<Implementation, endianness>::rw_int32s;
    using IEndianPrimitiveVectorMixins<Implementation, endianness>::rw_int32s;
    using IEndianPrimitiveArrayMixins<Implementation, endianness>::rw_int64s;
    using IEndianPrimitiveVectorMixins<Implementation, endianness>::rw_int64s;

    using IEndianPrimitiveArrayMixins<Implementation, endianness>::rw_uint8s;
    using IEndianPrimitiveVectorMixins<Implementation, endianness>::rw_uint8s;
    using IEndianPrimitiveArrayMixins<Implementation, endianness>::rw_uint16s;
    using IEndianPrimitiveVectorMixins<Implementation, endianness>::rw_uint16s;
    using IEndianPrimitiveArrayMixins<Implementation, endianness>::rw_uint32s;
    using IEndianPrimitiveVectorMixins<Implementation, endianness>::rw_uint32s;
    using IEndianPrimitiveArrayMixins<Implementation, endianness>::rw_uint64s;
    using IEndianPrimitiveVectorMixins<Implementation, endianness>::rw_uint64s;

    using IEndianPrimitiveArrayMixins<Implementation, endianness>::rw_float16s;
    using IEndianPrimitiveVectorMixins<Implementation, endianness>::rw_float16s;
    using IEndianPrimitiveArrayMixins<Implementation, endianness>::rw_float32s;
    using IEndianPrimitiveVectorMixins<Implementation, endianness>::rw_float32s;
    using IEndianPrimitiveArrayMixins<Implementation, endianness>::rw_float64s;
    using IEndianPrimitiveVectorMixins<Implementation, endianness>::rw_float64s;
};

template<typename Implementation>
struct StandardLittleEndianDescriptors : public StandardEndianDescriptors<Implementation, exbip::endian::little>
{
};

template<typename Implementation>
struct StandardBigEndianDescriptors : public StandardEndianDescriptors<Implementation, exbip::endian::big>
{
};

template<typename Implementation>
struct StandardViewDescriptorsLE : public StandardDescriptors<Implementation>, public StandardLittleEndianDescriptors<Implementation>
{};

template<typename Implementation>
struct StandardViewDescriptorsBE : public StandardDescriptors<Implementation>, public StandardBigEndianDescriptors<Implementation>
{};

END_EXBIP_NAMESPACE
