#pragma once

#include <string>
#include <vector>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <stdexcept>

#include "nibble.h"

namespace nibble_io
{

inline std::vector<std::uint8_t> read_to_bin(const std::string& path)
{
    std::ifstream f(path, std::ios::binary);
    if (!f) {
        throw std::runtime_error("Cannot open file: " + path);
    }

    f.seekg(0, std::ios::end);
    const std::streampos sz = f.tellg();
    if (sz < std::streampos{0}) {
        throw std::runtime_error("Cannot determine file size: " + path);
    }
    f.seekg(0, std::ios::beg);

    std::vector<std::uint8_t> bytes(static_cast<std::size_t>(sz));
    if (sz > 0) {
        f.read(reinterpret_cast<char*>(bytes.data()), sz);
        if (f.gcount() != static_cast<std::streamsize>(sz)) {
            throw std::runtime_error("Failed to read entire file: " + path);
        }
    }

    return bytes;
}

inline std::vector<Nibble> convert_to_nibbles(const std::vector<std::uint8_t>& bytes)
{
    std::vector<Nibble> out;
    out.reserve(bytes.size() * 2);

    for (std::uint8_t b : bytes) {
        const std::uint8_t hi = static_cast<std::uint8_t>((b >> 4) & 0x0F); // b7..b4
        const std::uint8_t lo = static_cast<std::uint8_t>(b & 0x0F);        // b3..b0
        out.emplace_back(hi); // сразу кладём готовое значение ниббла
        out.emplace_back(lo);
    }

    return out;
}

inline std::vector<Nibble> file_to_nibbles(const std::string& path)
{
    const auto bytes = read_to_bin(path);
    return convert_to_nibbles(bytes);
}

inline void write_nibbles_to_file(const std::string& path,
                                  const std::vector<Nibble>& nibbles)
{
    // Для корректного восстановления байтов количество нибблов должно быть чётным
    if (nibbles.size() % 2 != 0) {
        throw std::runtime_error("Number of nibbles must be even to form bytes");
    }

    std::vector<std::uint8_t> bytes;
    bytes.reserve(nibbles.size() / 2);

    for (std::size_t i = 0; i < nibbles.size(); i += 2) {
        // первый ниббл — старшая тетрада, второй — младшая
        const std::uint8_t hi = static_cast<std::uint8_t>(nibbles[i].value()     & 0x0F);
        const std::uint8_t lo = static_cast<std::uint8_t>(nibbles[i + 1].value() & 0x0F);
        bytes.push_back(static_cast<std::uint8_t>((hi << 4) | lo));
    }

    std::ofstream f(path, std::ios::binary | std::ios::trunc);
    if (!f) {
        throw std::runtime_error("Cannot open file for writing: " + path);
    }

    if (!bytes.empty()) {
        f.write(reinterpret_cast<const char*>(bytes.data()),
                static_cast<std::streamsize>(bytes.size()));
        if (!f) {
            throw std::runtime_error("Failed to write all data to file: " + path);
        }
    }
}


} // namespace nibble_io

