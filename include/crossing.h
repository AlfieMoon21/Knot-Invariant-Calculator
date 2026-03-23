#ifndef CROSSING_H
#define CROSSING_H

#include <vector>

// Single crossing in a DT-encoded knot diagram.
// sign = +1 (right-handed) or -1 (left-handed).
struct Crossing {
    int index;  // 0-indexed position in the crossing sequence
    int sign;   // +1 or -1

    Crossing(int i, int s) : index(i), sign(s) {}

    bool operator==(const Crossing& other) const {
        return index == other.index && sign == other.sign;
    }
};

#endif