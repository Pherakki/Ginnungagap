#pragma once

#include <cinttypes>
#include <array>
#include <vector>


struct UnknownData2
{
public:
    std::array<uint8_t, 0x208> data{0};
public:
    template<typename Parser>
    void exbip_rw(Parser& rw)
    {
        rw.rw_uint8s(this->data);
    }
};

struct UnknownRecord
{
public:
    uint32_t unknown_0x00;
    uint32_t unknown_0x04;
    uint32_t group_id;
    uint32_t count_1;

    uint64_t offset_1;
    uint32_t count_2;
    uint64_t offset_2;

    std::vector<uint64_t> data_1;
    std::vector<UnknownData2> data_2;
public:
    template<typename Parser>
    void exbip_rw(Parser& rw)
    {
        rw.rw_uint32(this->unknown_0x00);
        rw.rw_uint32(this->unknown_0x04);
        rw.rw_uint32(this->group_id);
        rw.rw_uint32(this->count_1);

        rw.rw_offset64(this->offset_1);
        rw.rw_uint32(this->count_2);
        rw.template rw_padding<0x04>();

        rw.rw_offset64(this->offset_2);
        rw.template rw_padding<0x50>();
    }

    template<typename Parser>
    void rw_data1(Parser& rw)
    {
        if (rw.section_exists(this->offset_1, this->count_1))
        {
            rw.verify_and_set_stream_offset(this->offset_1, "UnknownTable1::Data1");
            rw.rw_offset64s(this->data_1, this->count_1);
        }
    }

    template<typename Parser>
    void rw_data2(Parser& rw)
    {
        if (rw.section_exists(this->offset_2, this->count_2))
        {
            rw.verify_and_set_stream_offset(this->offset_2, "UnknownTable1::Data2");
            rw.rw_objs(this->data_2, this->count_2);
        }
    }
};
