#pragma once

#include <array>
#include <bitset>
#include <cinttypes>
#include <exbip/exbip.hpp>


struct HeaderFlags
{
protected:
    uint32_t buf;
public:
    struct Masks
    {
        const static uint32_t b00             = 0x00000001;
        const static uint32_t b01             = 0x00000002;
        const static uint32_t b02             = 0x00000004;
        const static uint32_t b03             = 0x00000008;
        const static uint32_t b04             = 0x00000010;
        const static uint32_t b05             = 0x00000020;
        const static uint32_t b06             = 0x00000040;
        const static uint32_t b07             = 0x00000080;
        const static uint32_t b08             = 0x00000100;
        const static uint32_t b09             = 0x00000200;
        const static uint32_t b0A             = 0x00000400;
        const static uint32_t b0B             = 0x00000800;
        const static uint32_t b0C             = 0x00001000;
        const static uint32_t b0D             = 0x00002000;
        const static uint32_t b0E             = 0x00004000;
        const static uint32_t b0F             = 0x00008000;
        const static uint32_t b10             = 0x00010000;
        const static uint32_t b11             = 0x00020000;
        const static uint32_t b12             = 0x00040000;
        const static uint32_t b13             = 0x00080000;
        const static uint32_t b14             = 0x00100000;
        const static uint32_t b15             = 0x00200000;
        const static uint32_t b16             = 0x00400000;
        const static uint32_t b17             = 0x00800000;
        const static uint32_t b18             = 0x01000000;
        const static uint32_t b19             = 0x02000000;
        const static uint32_t b1A             = 0x04000000;
        const static uint32_t IS_BIG_ENDIAN   = 0x08000000;
        const static uint32_t EXTENDED_HEADER = 0x10000000;
        const static uint32_t b1D             = 0x20000000;
        const static uint32_t b1E             = 0x40000000;
        const static uint32_t b1F             = 0x80000000;
    };

    uint32_t flag_0()  { return (this->buf & Masks::b00) >> 0x00; }
    uint32_t flag_1()  { return (this->buf & Masks::b01) >> 0x01; }
    uint32_t flag_2()  { return (this->buf & Masks::b02) >> 0x02; }
    uint32_t flag_3()  { return (this->buf & Masks::b03) >> 0x03; }
    uint32_t flag_4()  { return (this->buf & Masks::b04) >> 0x04; }
    uint32_t flag_5()  { return (this->buf & Masks::b05) >> 0x05; }
    uint32_t flag_6()  { return (this->buf & Masks::b06) >> 0x06; }
    uint32_t flag_7()  { return (this->buf & Masks::b07) >> 0x07; }
    uint32_t flag_8()  { return (this->buf & Masks::b08) >> 0x08; }
    uint32_t flag_9()  { return (this->buf & Masks::b09) >> 0x09; }
    uint32_t flag_10() { return (this->buf & Masks::b0A) >> 0x0A; }
    uint32_t flag_11() { return (this->buf & Masks::b0B) >> 0x0B; }
    uint32_t flag_12() { return (this->buf & Masks::b0C) >> 0x0C; }
    uint32_t flag_13() { return (this->buf & Masks::b0D) >> 0x0D; }
    uint32_t flag_14() { return (this->buf & Masks::b0E) >> 0x0E; }
    uint32_t flag_15() { return (this->buf & Masks::b0F) >> 0x0F; }
    uint32_t flag_16() { return (this->buf & Masks::b10) >> 0x10; }
    uint32_t flag_17() { return (this->buf & Masks::b11) >> 0x11; }
    uint32_t flag_18() { return (this->buf & Masks::b12) >> 0x12; }
    uint32_t flag_19() { return (this->buf & Masks::b13) >> 0x13; }
    uint32_t flag_20() { return (this->buf & Masks::b14) >> 0x14; }
    uint32_t flag_21() { return (this->buf & Masks::b15) >> 0x15; }
    uint32_t flag_22() { return (this->buf & Masks::b16) >> 0x16; }
    uint32_t flag_23() { return (this->buf & Masks::b17) >> 0x17; }
    uint32_t flag_24() { return (this->buf & Masks::b18) >> 0x18; }
    uint32_t flag_25() { return (this->buf & Masks::b19) >> 0x19; }
    uint32_t flag_26() { return (this->buf & Masks::b1A) >> 0x1A; }
    uint32_t IS_BIG_ENDIAN  () { return (this->buf & Masks::IS_BIG_ENDIAN  ) >> 0x1B; }
    uint32_t EXTENDED_HEADER() { return (this->buf & Masks::EXTENDED_HEADER) >> 0x1C; }
    uint32_t flag_29() { return (this->buf & Masks::b1D) >> 0x1D; }
    uint32_t flag_30() { return (this->buf & Masks::b1E) >> 0x1E; }
    uint32_t flag_31() { return (this->buf & Masks::b1F) >> 0x1F; }

    const uint32_t flag_0() const noexcept  { return (this->buf & Masks::b00) >> 0x00; }
    const uint32_t flag_1() const noexcept  { return (this->buf & Masks::b01) >> 0x01; }
    const uint32_t flag_2() const noexcept  { return (this->buf & Masks::b02) >> 0x02; }
    const uint32_t flag_3() const noexcept  { return (this->buf & Masks::b03) >> 0x03; }
    const uint32_t flag_4() const noexcept  { return (this->buf & Masks::b04) >> 0x04; }
    const uint32_t flag_5() const noexcept  { return (this->buf & Masks::b05) >> 0x05; }
    const uint32_t flag_6() const noexcept  { return (this->buf & Masks::b06) >> 0x06; }
    const uint32_t flag_7() const noexcept  { return (this->buf & Masks::b07) >> 0x07; }
    const uint32_t flag_8() const noexcept  { return (this->buf & Masks::b08) >> 0x08; }
    const uint32_t flag_9() const noexcept  { return (this->buf & Masks::b09) >> 0x09; }
    const uint32_t flag_10() const noexcept { return (this->buf & Masks::b0A) >> 0x0A; }
    const uint32_t flag_11() const noexcept { return (this->buf & Masks::b0B) >> 0x0B; }
    const uint32_t flag_12() const noexcept { return (this->buf & Masks::b0C) >> 0x0C; }
    const uint32_t flag_13() const noexcept { return (this->buf & Masks::b0D) >> 0x0D; }
    const uint32_t flag_14() const noexcept { return (this->buf & Masks::b0E) >> 0x0E; }
    const uint32_t flag_15() const noexcept { return (this->buf & Masks::b0F) >> 0x0F; }
    const uint32_t flag_16() const noexcept { return (this->buf & Masks::b10) >> 0x10; }
    const uint32_t flag_17() const noexcept { return (this->buf & Masks::b11) >> 0x11; }
    const uint32_t flag_18() const noexcept { return (this->buf & Masks::b12) >> 0x12; }
    const uint32_t flag_19() const noexcept { return (this->buf & Masks::b13) >> 0x13; }
    const uint32_t flag_20() const noexcept { return (this->buf & Masks::b14) >> 0x14; }
    const uint32_t flag_21() const noexcept { return (this->buf & Masks::b15) >> 0x15; }
    const uint32_t flag_22() const noexcept { return (this->buf & Masks::b16) >> 0x16; }
    const uint32_t flag_23() const noexcept { return (this->buf & Masks::b17) >> 0x17; }
    const uint32_t flag_24() const noexcept { return (this->buf & Masks::b18) >> 0x18; }
    const uint32_t flag_25() const noexcept { return (this->buf & Masks::b19) >> 0x19; }
    const uint32_t flag_26() const noexcept { return (this->buf & Masks::b1A) >> 0x1A; }
    const uint32_t IS_BIG_ENDIAN  () const noexcept { return (this->buf & Masks::IS_BIG_ENDIAN  ) >> 0x1B; }
    const uint32_t EXTENDED_HEADER() const noexcept { return (this->buf & Masks::EXTENDED_HEADER) >> 0x1C; }
    const uint32_t flag_29() const noexcept { return (this->buf & Masks::b1D) >> 0x1D; }
    const uint32_t flag_30() const noexcept { return (this->buf & Masks::b1E) >> 0x1E; }
    const uint32_t flag_31() const noexcept { return (this->buf & Masks::b1F) >> 0x1F; }
    
    void flag_0         (bool v) { this->buf &= ~((uint32_t)1 << 0x00); this->buf |= ((v << 0x00) & Masks::b00);             }
    void flag_1         (bool v) { this->buf &= ~((uint32_t)1 << 0x01); this->buf |= ((v << 0x01) & Masks::b01);             }
    void flag_2         (bool v) { this->buf &= ~((uint32_t)1 << 0x02); this->buf |= ((v << 0x02) & Masks::b02);             }
    void flag_3         (bool v) { this->buf &= ~((uint32_t)1 << 0x03); this->buf |= ((v << 0x03) & Masks::b03);             }
    void flag_4         (bool v) { this->buf &= ~((uint32_t)1 << 0x04); this->buf |= ((v << 0x04) & Masks::b04);             }
    void flag_5         (bool v) { this->buf &= ~((uint32_t)1 << 0x05); this->buf |= ((v << 0x05) & Masks::b05);             }
    void flag_6         (bool v) { this->buf &= ~((uint32_t)1 << 0x06); this->buf |= ((v << 0x06) & Masks::b06);             }
    void flag_7         (bool v) { this->buf &= ~((uint32_t)1 << 0x07); this->buf |= ((v << 0x07) & Masks::b07);             }
    void flag_8         (bool v) { this->buf &= ~((uint32_t)1 << 0x08); this->buf |= ((v << 0x08) & Masks::b08);             }
    void flag_9         (bool v) { this->buf &= ~((uint32_t)1 << 0x09); this->buf |= ((v << 0x09) & Masks::b09);             }
    void flag_10        (bool v) { this->buf &= ~((uint32_t)1 << 0x0A); this->buf |= ((v << 0x0A) & Masks::b0A);             }
    void flag_11        (bool v) { this->buf &= ~((uint32_t)1 << 0x0B); this->buf |= ((v << 0x0B) & Masks::b0B);             }
    void flag_12        (bool v) { this->buf &= ~((uint32_t)1 << 0x0C); this->buf |= ((v << 0x0C) & Masks::b0C);             }
    void flag_13        (bool v) { this->buf &= ~((uint32_t)1 << 0x0D); this->buf |= ((v << 0x0D) & Masks::b0D);             }
    void flag_14        (bool v) { this->buf &= ~((uint32_t)1 << 0x0E); this->buf |= ((v << 0x0E) & Masks::b0E);             }
    void flag_15        (bool v) { this->buf &= ~((uint32_t)1 << 0x0F); this->buf |= ((v << 0x0F) & Masks::b0F);             }
    void flag_16        (bool v) { this->buf &= ~((uint32_t)1 << 0x10); this->buf |= ((v << 0x10) & Masks::b10);             }
    void flag_17        (bool v) { this->buf &= ~((uint32_t)1 << 0x11); this->buf |= ((v << 0x11) & Masks::b11);             }
    void flag_18        (bool v) { this->buf &= ~((uint32_t)1 << 0x12); this->buf |= ((v << 0x12) & Masks::b12);             }
    void flag_19        (bool v) { this->buf &= ~((uint32_t)1 << 0x13); this->buf |= ((v << 0x13) & Masks::b13);             }
    void flag_20        (bool v) { this->buf &= ~((uint32_t)1 << 0x14); this->buf |= ((v << 0x14) & Masks::b14);             }
    void flag_21        (bool v) { this->buf &= ~((uint32_t)1 << 0x15); this->buf |= ((v << 0x15) & Masks::b15);             }
    void flag_22        (bool v) { this->buf &= ~((uint32_t)1 << 0x16); this->buf |= ((v << 0x16) & Masks::b16);             }
    void flag_23        (bool v) { this->buf &= ~((uint32_t)1 << 0x17); this->buf |= ((v << 0x17) & Masks::b17);             }
    void flag_24        (bool v) { this->buf &= ~((uint32_t)1 << 0x18); this->buf |= ((v << 0x18) & Masks::b18);             }
    void flag_25        (bool v) { this->buf &= ~((uint32_t)1 << 0x19); this->buf |= ((v << 0x19) & Masks::b19);             }
    void flag_26        (bool v) { this->buf &= ~((uint32_t)1 << 0x1A); this->buf |= ((v << 0x1A) & Masks::b1A);             }
    void IS_BIG_ENDIAN  (bool v) { this->buf &= ~((uint32_t)1 << 0x1B); this->buf |= ((v << 0x1B) & Masks::IS_BIG_ENDIAN);   }
    void EXTENDED_HEADER(bool v) { this->buf &= ~((uint32_t)1 << 0x1C); this->buf |= ((v << 0x1C) & Masks::EXTENDED_HEADER); }
    void flag_29        (bool v) { this->buf &= ~((uint32_t)1 << 0x1D); this->buf |= ((v << 0x1D) & Masks::b1D);             }
    void flag_30        (bool v) { this->buf &= ~((uint32_t)1 << 0x1E); this->buf |= ((v << 0x1E) & Masks::b1E);             }
    void flag_31        (bool v) { this->buf &= ~((uint32_t)1 << 0x1F); this->buf |= ((v << 0x1F) & Masks::b1F);             }

    void        set       (uint32_t v)       noexcept;
    uint32_t&   getRef    ()                 noexcept;
    uint32_t    get       ()           const noexcept;
    void        operator&=(uint32_t v)       noexcept;
    void        operator|=(uint32_t v)       noexcept;
    HeaderFlags operator& (uint32_t v) const noexcept;
    HeaderFlags operator| (uint32_t v) const noexcept;
    HeaderFlags operator~ ()           const noexcept;

    template<typename Parser>
    void exbip_rw(Parser& rw)
    {
        rw.rw_uint32(this->buf);
    }
private:
    uint32_t get_bit(uint8_t index)
    { 
        uint32_t mask = ((uint32_t)1 << index);
        return (this->buf & mask) >> index;
    }

    void set_bit(uint8_t index, bool value)
    { 
        uint32_t mask = ((uint32_t)1 << index);
        this->buf &= ~mask; 
        this->buf |= (value << index);
    }
};
