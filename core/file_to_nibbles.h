#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include "Nibble.h"


inline std::vector<uchar> read_to_bin(const std::string& path) 
{
    std::ifstream f(path, std::ios::binary);
    if (!f) throw std::runtime_error("Cannot open file: " + path);

    f.seekg(0, std::ios::end);
    std::streampos sz = f.tellg();
    f.seekg(0, std::ios::beg);

    std::vector<uchar> bytes;
    bytes.resize(static_cast<size_t>(sz));
    if (sz > 0) f.read(reinterpret_cast<char*>(bytes.data()), sz);

    return bytes;
}

/*
 * Конвертация байтов в вектор нибблов (MSB-first внутри байта).
 * Для каждого байта b: сначала hi-ниббл (b7..b4), затем lo-ниббл (b3..b0).
 */
inline std::vector<Nibble> convert_to_nibbles(const std::vector<uchar>& bytes) 
{
    std::vector<Nibble> out;
    out.reserve(bytes.size() * 2);

    for (uchar b : bytes) 
    {
        uchar hi = static_cast<uchar>((b >> 4) & 0x0F); // b7..b4
        uchar lo = static_cast<uchar>( b        & 0x0F); // b3..b0
        out.emplace_back(hi); // сразу кладём готовое значение ниббла
        out.emplace_back(lo);
    }
    return out;
}


inline std::vector<Nibble> file_to_nibbles(const std::string& path)
{
    auto bytes = read_to_bin(path);
    return convert_to_nibbles(bytes);
}