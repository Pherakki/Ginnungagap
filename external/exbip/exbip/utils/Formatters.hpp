#pragma once

#include <iomanip>
#include <sstream>
#include <type_traits>
#include <exbip/_detail/DefineDetailMacros.hpp>


BEGIN_EXBIP_NAMESPACE

template<typename T>
requires std::is_integral<T>::value
std::string hex_formatter(size_t v)
{
    std::stringstream ss;
    
    ss << "0x" << std::setfill ('0') << std::setw(sizeof(T)*2) << std::hex << v;
    return ss.str();
}

END_EXBIP_NAMESPACE
