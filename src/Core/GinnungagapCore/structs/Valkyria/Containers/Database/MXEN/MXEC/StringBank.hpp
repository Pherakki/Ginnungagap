#pragma once

#include <string>
#include <map>
#include <exbip/exbip.hpp>


class MXECStringBank
{
public:
    using bank_t = std::map<std::string, exbip::OffsetMarker>;
public:
    bank_t bank;
    size_t begin_offset=(size_t)-1;
    size_t end_offset=0;
public:
    MXECStringBank() = default;
    ~MXECStringBank() = default;

    template<typename T>
    requires (std::is_integral_v<T>)
    void addString(const std::string& s, T& offset)
    {
        if (bank.count(s) == 0)
            this->bank.emplace(s, exbip::OffsetMarker());
        
        auto& marker = this->bank.at(s);
        marker.subscribe(offset);
    }

    template<typename T>
    requires (std::is_integral_v<T>)
    void updateOffsets(const std::string& s, T offset)
    {
        this->begin_offset = std::min(this->begin_offset, offset);
        this->end_offset   = std::max(this->end_offset, offset + s.size() + 1); // +1 for null-terminator
    }
    
    bank_t::iterator begin() { return this->bank.begin(); }
    bank_t::iterator end  () { return this->bank.end(); }
    size_t bytesize() { return this->end_offset - this->begin_offset; }
    size_t size()  { return this->bank.size(); }

    void reset()
    {
        this->bank.clear();
        this->begin_offset = 0;
        this->end_offset   = 0;
    }
};
