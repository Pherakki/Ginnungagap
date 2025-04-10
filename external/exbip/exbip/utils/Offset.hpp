#pragma once


inline size_t bytesToAlignment(size_t offset, size_t alignment)
{
    return (alignment - (offset % alignment)) % alignment;
}

inline size_t roundToAlignment(size_t offset, size_t alignment)
{
    return offset + bytesToAlignment(offset, alignment);
}
