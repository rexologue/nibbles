#pragma once
#ifndef NIBBLE_H
#define NIBBLE_H

#include <string>

typedef unsigned char uchar;

class Nibble
{
public:
    // Конструктор из готового значения ниббла (0..15)
    explicit Nibble(uchar nib)
    {
        m_bytes[0] = static_cast<uchar>(nib & 0x0F); // храним младшую тетраду
        m_label = "s" + std::to_string(static_cast<int>(m_bytes[0]) + 1); // 0000->s1,...,1111->s16
    }

    std::string label() const { return m_label; }

    // Вернёт строку "0000".."1111" из упакованного байта (MSB→LSB внутри тетрады)
    std::string bytes() const
    {
        std::string s(4, '0');
        for (int i = 0; i < 4; ++i) 
        {
            unsigned bit = (m_bytes[0] >> (3 - i)) & 0x1u;
            s[i] = bit ? '1' : '0';
        }
        return s;
    }

    uchar value() const { return static_cast<uchar>(m_bytes[0] & 0x0F); }

private:
    std::string m_label;
    uchar m_bytes[1]; // значимы младшие 4 бита
};

#endif // NIBBLE_H
