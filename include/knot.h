#ifndef KNOT_H
#define KNOT_H

#include <vector>
#include <iostream>
#include <cstdlib>
#include "crossing.h"

// Knot diagram parsed from Dowker-Thistlethwaite (DT) notation.
//
// DT notation encodes a knot with n crossings as a sequence of n signed integers.
// The knot circle is traversed once; odd positions (1, 3, 5, ...) are first visits,
// and the corresponding even positions are stored in the DT array. A negative value
// indicates a negative (left-handed) crossing.
//
// Crossing positions are stored 0-indexed in [0, 2n), which is what
// KauffmanBracket::apply_smoothing expects.
class KnotDiagram {
private:
    std::vector<Crossing> crossings;
    int num_components;  // reserved for link support; always 1 for knots
    std::vector<std::pair<int,int>> crossing_positions;  // (first_visit, second_visit), 0-indexed in [0, 2n)

public:
    KnotDiagram() : num_components(1) {}

    void add_crossing(int index, int sign) {
        crossings.push_back(Crossing(index, sign));
    }

    int size() const { return crossings.size(); }
    bool is_empty() const { return crossings.empty(); }

    const std::vector<Crossing>& get_crossings() const { return crossings; }
    const std::vector<std::pair<int,int>>& get_crossing_positions() const { return crossing_positions; }

    // Sum of crossing signs.
    int get_writhe() const {
        int w = 0;
        for (const auto& c : crossings) w += c.sign;
        return w;
    }

    std::vector<int> get_crossing_signs() const {
        std::vector<int> signs;
        for (const auto& c : crossings) signs.push_back(c.sign);
        return signs;
    }

    void print() const {
        std::cout << "Knot with " << crossings.size() << " crossings:\n";
        for (const auto& c : crossings) {
            std::cout << " Crossing " << c.index
                      << " (sign: " << (c.sign > 0 ? "+" : "-") << ")\n";
        }
    }

    // Build a KnotDiagram from DT notation.
    // Crossing i sits at arc position 2i (first visit) and abs(dt[i])-1 (second visit).
    static KnotDiagram from_dt(const std::vector<int>& dt_notation) {
        KnotDiagram knot;
        for (size_t i = 0; i < dt_notation.size(); i++) {
            int value = dt_notation[i];
            knot.add_crossing(i, (value > 0) ? 1 : -1);
            knot.crossing_positions.push_back({2 * (int)i, std::abs(value) - 1});
        }
        return knot;
    }
};

#endif
