#ifndef DIAGRAM_STATE_H
#define DIAGRAM_STATE_H

#include <map>
#include <ostream>

//represent the state of a knot diagram during computation
struct DiagramState {
    std::map<int, char> smoothing_history; // crossing id 'A' or 'B'
    int num_components;

    DiagramState(int components = 1) : num_components(components) {}

    // Equility for caching 
    bool operator==(const DiagramState& other) const {
        return smoothing_history == other.smoothing_history &&
               num_components == other.num_components;
    }

    //Ordering for map key
    bool operator<(const DiagramState& other) const {
        if (smoothing_history != other.smoothing_history) {
            return smoothing_history < other.smoothing_history;
        }
        return num_components < other.num_components;
    }
};

// For debugging - show the state
inline std::ostream& operator<<(std::ostream& os, const DiagramState& state) {
    os << "[history: ";
    for (const auto& [crossing, type] : state.smoothing_history) {
        os << crossing << "→" << type << " ";
    }
    os << "| " << state.num_components << " components]";
    return os;
}

#endif