#pragma once

#include <stddef.h>


size_t align(size_t offset, size_t alignment);
size_t remainderBytes(size_t offset, size_t alignment);
