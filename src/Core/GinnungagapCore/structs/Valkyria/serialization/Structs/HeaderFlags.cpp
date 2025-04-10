#include "HeaderFlags.hpp"


void HeaderFlags::set(uint32_t v) noexcept
{
    this->buf = v;
}

uint32_t HeaderFlags::get() const noexcept
{
    return this->buf;
}

uint32_t& HeaderFlags::getRef() noexcept
{
    return this->buf;
}

void HeaderFlags::operator&=(uint32_t v) noexcept
{
    this->buf &= v;
}

void HeaderFlags::operator|=(uint32_t v) noexcept
{
    this->buf |= v;
}

HeaderFlags HeaderFlags::operator& (uint32_t v) const noexcept
{
    HeaderFlags out;
    out.set(this->buf & v);
    return out;
}

HeaderFlags HeaderFlags::operator|(uint32_t v) const noexcept
{
    HeaderFlags out;
    out.set(this->buf | v);
    return out;
}

HeaderFlags HeaderFlags::operator~() const noexcept
{
    HeaderFlags out;
    out.set(~this->buf);
    return out;
}

