#ifndef DIAGRAM_STATE_H
#define DIAGRAM_STATE_H

// NOT USED IN THE CURRENT BUILD.
//
// DiagramState was the cache key in the original caching strategy: it stored the
// full smoothing history (crossing id mapped to 'A' or 'B') as the key. That produced zero
// hits because the recursion always processes crossings in the same order, making
// every history unique.
//
// Replaced by CacheKey in kauffman.h, which keys on the resulting arc connectivity
// rather than the sequence of choices that produced it, so two different histories
// that yield the same topology share a cache entry.
//
// Kept to illustrate exactly why the naive approach fails.

#include <map>
#include <ostream>

// Original history-based cache key (superseded, see kauffman.h CacheKey).
struct DiagramState {
    std::map<int, char> smoothing_history; // crossing id 'A' or 'B'
    int num_components;

    DiagramState(int components = 1) : num_components(components) {}

    bool operator==(const DiagramState& other) const {
        return smoothing_history == other.smoothing_history &&
               num_components == other.num_components;
    }

    bool operator<(const DiagramState& other) const {
        if (smoothing_history != other.smoothing_history) {
            return smoothing_history < other.smoothing_history;
        }
        return num_components < other.num_components;
    }
};

// Debug printer.
inline std::ostream& operator<<(std::ostream& os, const DiagramState& state) {
    os << "[history: ";
    for (const auto& [crossing, type] : state.smoothing_history) {
        os << crossing << "→" << type << " ";
    }
    os << "| " << state.num_components << " components]";
    return os;
}

#endif