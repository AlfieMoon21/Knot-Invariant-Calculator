#ifndef KNOT_H
#define KNOT_H

#include <vector>
#include <iostream>
#include "crossing.h"

class KnotDiagram {
private:
    std::vector<Crossing> crossings;
    int num_components; // number of separate loops

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
        }
    
        return knot;
    }
};

#endif
