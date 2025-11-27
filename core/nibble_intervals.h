#pragma once

#ifndef NIBBLE_INTERVALS_H
#define NIBBLE_INTERVALS_H

#include <map>
#include <tuple>
#include <vector>
#include <string>      
#include <cstdint> 
#include <fstream>    
#include <stdexcept>   

#include "nibble.h"

class NibbleIntervalArchiever
{
public:
    NibbleIntervalArchiever() {}

    std::vector<std::uint64_t> encode(const std::vector<Nibble>& nibbles);
    std::vector<Nibble> decode(const std::vector<std::uint64_t>& encoded_nibbles);

    void pack(const std::vector<Nibble>& nibbles, const std::string& path);
    std::vector<Nibble> unpack(const std::string& path);

private:
    std::map<uchar, std::uint64_t> init_counter();
    void get_away_counter(std::map<uchar, std::uint64_t>& counter);

};

inline std::map<uchar, std::uint64_t> NibbleIntervalArchiever::init_counter()
{
    std::map<uchar, std::uint64_t> counter;
    
    // Устанавливаем базовые значения 
    for (int i = static_cast<int>(MAX_NIBBLE_VALUE); i >= 0; --i)
    {
        counter[uchar(i)] = static_cast<std::uint64_t>(i);
    }

    return counter;
}

inline void NibbleIntervalArchiever::get_away_counter(std::map<uchar, std::uint64_t> &counter)
{
    // Увеличиваем расстояние до предыдщего символа на 1 для всех символов
    for (int i = static_cast<int>(MAX_NIBBLE_VALUE); i >= 0; --i)
    {
        counter[uchar(i)] += 1;
    }
}

inline std::vector<std::uint64_t> NibbleIntervalArchiever::encode(const std::vector<Nibble>& nibbles)
{
    std::map<uchar, std::uint64_t> counter = init_counter();
    std::vector<std::uint64_t> encoded_nibbles(nibbles.size());

    std::size_t j = 0;
    for(const Nibble& n : nibbles)
    {
        encoded_nibbles[j] = counter[n.value()];

        get_away_counter(counter);

        counter[n.value()] = 0;
        j += 1;
    }

    return encoded_nibbles;
}

inline std::vector<Nibble> NibbleIntervalArchiever::decode(const std::vector<std::uint64_t>& encoded_nibbles)
{
    std::map<uchar, std::uint64_t> counter = init_counter();
    std::vector<Nibble> nibbles(encoded_nibbles.size());

    auto get_key_by_val = [&counter](std::uint64_t val) {
        for(const auto& [k, v] : counter)
        {
            if (val == v)
                return std::make_tuple(k, true);
        }

        return std::make_tuple(MAX_NIBBLE_VALUE, false);
    };

    std::size_t j = 0;
    for(const std::uint64_t& n : encoded_nibbles)
    {
        auto t = get_key_by_val(n);

        if (!std::get<1>(t))
            throw std::runtime_error("Invalid value in encoded sequence" + std::to_string(n));

        uchar key = std::get<0>(t);
        nibbles[j] = Nibble(key);

        get_away_counter(counter);

        counter[key] = 0;
        j += 1;
    }

    return nibbles;
}

inline void NibbleIntervalArchiever::pack(const std::vector<Nibble> &nibbles,
                                          const std::string& path)
{
    std::vector<std::uint64_t> encoded_nibbles = encode(nibbles);

    std::ofstream f(path, std::ios::binary | std::ios::trunc);
    if (!f) {
        throw std::runtime_error("Cannot open file for writing: " + path);
    }

    if (!encoded_nibbles.empty()) {
        const auto bytes =
            static_cast<std::streamsize>(encoded_nibbles.size() * sizeof(std::uint64_t));

        f.write(reinterpret_cast<const char*>(encoded_nibbles.data()), bytes);
        if (!f) {
            throw std::runtime_error("Failed to write all data to file: " + path);
        }
    }
}


inline std::vector<Nibble> NibbleIntervalArchiever::unpack(const std::string &path)
{
    std::ifstream f(path, std::ios::binary);
    if (!f) {
        throw std::runtime_error("Cannot open file for reading: " + path);
    }

    // Определяем размер файла
    f.seekg(0, std::ios::end);
    const std::streampos sz = f.tellg();
    if (sz < std::streampos{0}) {
        throw std::runtime_error("Cannot determine file size: " + path);
    }
    if (sz == std::streampos{0}) {
        // Пустой файл — пустой набор нибблов
        return {};
    }

    // Проверяем, что размер кратен sizeof(uint64_t)
    const auto uint64_size = static_cast<std::streampos>(sizeof(std::uint64_t));
    if (sz % uint64_size != std::streampos{0}) {
        throw std::runtime_error("File size is not multiple of uint64_t size: " + path);
    }

    f.seekg(0, std::ios::beg);

    const std::size_t count =
        static_cast<std::size_t>(sz / uint64_size);
    std::vector<std::uint64_t> encoded_nibbles(count);

    const auto bytes =
        static_cast<std::streamsize>(count * sizeof(std::uint64_t));

    f.read(reinterpret_cast<char*>(encoded_nibbles.data()), bytes);
    if (f.gcount() != bytes) {
        throw std::runtime_error("Failed to read entire file: " + path);
    }

    return decode(encoded_nibbles);
}


#endif // NIBBLE_INTERVALS_H