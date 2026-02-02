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
};

#endif
