#ifndef DIAGRAM_STATE_H
#define DIAGRAM_STATE_H

#include <functional>
#include <ostream>

//represent the state of a knot diagram during computation
struct DiagramState {
    int num_crossings;
    int num_components;

    DiagramState(int crossings, int components)
        : num_crossings(crossings), num_components(components) {}

    //for use as a map key
    bool operator==(const DiagramState& other) const {
        return num_crossings == other.num_crossings && 
               num_components == other.num_components;
    }

    bool operator<(const DiagramState& other) const {
        if (num_crossings != other.num_crossings) {
            return  num_crossings < other.num_crossings;
        }
        return num_components < other.num_components;
    }
};

//debugging
inline std::ostream& operator<<(std::ostream& os, const DiagramState& state) {
    os << "[" << state.num_crossings << " crossings, "
       << state.num_components << " components]";
    return os;
}

#endif