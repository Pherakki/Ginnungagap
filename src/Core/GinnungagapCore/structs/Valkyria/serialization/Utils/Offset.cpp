#include "Offset.hpp"


size_t align(size_t offset, size_t alignment)
{
    return (alignment - remainderBytes(offset, alignment)) % alignment;
}

size_t remainderBytes(size_t offset, size_t alignment)
{
    return offset % alignment;
}
