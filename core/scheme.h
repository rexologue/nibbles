#pragma once
#ifndef SCHEME_H
#define SCHEME_H

#include <array>
#include <cstdint>
#include <vector>
#include <cmath>
#include "nibble.h"

class Scheme {
public:
    using Counts     = std::array<std::array<uint64_t, 16>, 16>;
    using ProbMatrix = std::array<std::array<double,   16>, 16>;

    // Строим счётчики и обе матрицы вероятностей:
    //  - m_joint: P(a,b) = N_ab / N  (совместные)
    //  - m_cond : P(b|a) = N_ab / N_a (условные по строкам)
    explicit Scheme(const std::vector<Nibble>& seq)
    {
        for (auto& r : m_counts) r.fill(0);
        m_row_sum.fill(0);
        m_total = 0;

        if (seq.size() >= 2) {
            for (size_t i = 0; i + 1 < seq.size(); ++i) {
                const int a = seq[i].value();     // 0..15
                const int b = seq[i+1].value();   // 0..15
                m_counts[a][b] += 1;
                m_row_sum[a]   += 1;
                ++m_total;
            }
        }

        // Совместные P(a,b)
        for (int a = 0; a < 16; ++a) {
            for (int b = 0; b < 16; ++b) {
                m_joint[a][b] = (m_total ? static_cast<double>(m_counts[a][b]) / static_cast<double>(m_total) : 0.0);
            }
        }

        // Условные P(b|a)
        for (int a = 0; a < 16; ++a) {
            const uint64_t Na = m_row_sum[a];
            const double invNa = (Na ? 1.0 / static_cast<double>(Na) : 0.0);
            for (int b = 0; b < 16; ++b) {
                m_cond[a][b] = (Na ? static_cast<double>(m_counts[a][b]) * invNa : 0.0);
            }
        }
    }

    // === Доступ к данным ===
    // Совместные вероятности P(a,b) = N_ab / N  (сумма по всем a,b = 1)
    const ProbMatrix& table() const { return m_joint; }
    // Условные вероятности P(b|a) = N_ab / N_a  (каждая строка суммируется к ~1)
    const ProbMatrix& table_conditional() const { return m_cond; }
    // Сырые счётчики N_ab
    const Counts& counts() const { return m_counts; }
    // Суммы по строкам N_a
    const std::array<uint64_t,16>& row_sums() const { return m_row_sum; }
    // Общее число переходов N = M-1
    uint64_t transitions() const { return m_total; }

    // === Энтропии ===
    // Совместная энтропия H(S_i, S_{i+1}) по P(a,b) [бит на пару]
    double entropy_joint() const {
        double H = 0.0;
        for (int a = 0; a < 16; ++a) {
            for (int b = 0; b < 16; ++b) {
                const double p = m_joint[a][b];
                if (p > 0.0) H -= p * std::log2(p);
            }
        }
        return H;
    }

    // Маргинальная энтропия H(S_i) (предыдущего ниббла) [бит на ниббл]
    double entropy_prev() const {
        double H = 0.0;
        for (int a = 0; a < 16; ++a) {
            double pa = 0.0;
            for (int b = 0; b < 16; ++b) pa += m_joint[a][b]; // сумма по строке
            if (pa > 0.0) H -= pa * std::log2(pa);
        }
        return H;
    }

    // Условная энтропия H(S_{i+1} | S_i) [бит на ниббл]
    // Эквивалентно: H_joint - H_prev
    double entropy_conditional_nibble() const {
        return entropy_joint() - entropy_prev();
    }

    // Энтропия на бит (нормированная на 1 б/бит)
    double entropy_per_bit() const {
        return entropy_conditional_nibble() / 4.0;
    }

    // Относительная (к максимуму 4 б/ниббл), т.е. то же, что и per_bit()
    double entropy_relative() const {
        return entropy_conditional_nibble() / 4.0;
    }

    double entropy_max() const { return 4.0; }

private:
    Counts                   m_counts{};   // N_ab
    std::array<uint64_t,16>  m_row_sum{};  // N_a
    uint64_t                 m_total{0};   // N
    ProbMatrix               m_joint{};    // P(a,b)
    ProbMatrix               m_cond{};     // P(b|a)
};

#endif // SCHEME_H


