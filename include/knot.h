#ifndef KNOT_H
#define KNOT_H

#include <vector>
#include <iostream>
#include <cstdlib>
#include "crossing.h"

class KnotDiagram {
private:
    std::vector<Crossing> crossings;
    int num_components; // number of separate loops
    std::vector<std::pair<int,int>> crossing_positions; // (first_visit, second_visit) in 0-indexed arc positions

public:
    KnotDiagram() : num_components(1) {}

    // Add crossing to diagram
    void add_crossing(int index, int sign) {
        crossings.push_back(Crossing(index, sign));
    }

    // Get number of crossings
    int size() const {
        return crossings.size();
    }

    // Check if diagram has no crossings
    bool is_empty() const {
        return crossings.empty();
    }

    // Get all crossings
    const std::vector<Crossing>& get_crossings() const {
        return crossings;
    }

    // Get arc position pairs for component counting
    const std::vector<std::pair<int,int>>& get_crossing_positions() const {
        return crossing_positions;
    }

    // Get writhe (sum of crossing signs)
    int get_writhe() const {
        int w = 0;
        for (const auto& c : crossings) w += c.sign;
        return w;
    }

    // Get crossing signs as a plain vector (for passing to Kauffman bracket)
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

    // Parse Dowker-Thistlethwaite notation
    static KnotDiagram from_dt(const std::vector<int>& dt_notation) {
        KnotDiagram knot;
    
        // Each position in DT notation represents a crossing
        for (size_t i = 0; i < dt_notation.size(); i++) {
            int value = dt_notation[i];

            // Sign of the crossing: positive if value > 0, negative if value < 0
            int sign = (value > 0) ? 1 : -1;

            // Add the crossing (index i, with determined sign)
            knot.add_crossing(i, sign);

            // Store arc positions: crossing i is visited at position 2i (first)
            // and abs(value)-1 (second), both 0-indexed in the range [0, 2n)
            int first_visit = 2 * i;
            int second_visit = std::abs(value) - 1;
            knot.crossing_positions.push_back({first_visit, second_visit});
        }
    
        return knot;
    }
};

#endif
