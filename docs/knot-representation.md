# Knot Representation

## Overview
We represent knots using two main structures:
1. `Crossing` - a single crossing point
2. `KnotDiagram` - a collection of crossings

## The Crossing Structure
```cpp
struct Crossing {
    int index;      // Which crossing (0, 1, 2, ...)
    int sign;       // +1 (positive) or -1 (negative)
};
```

### What is a Crossing?

A crossing is where two strands of the knot cross each other:

**[IMAGE PLACEHOLDER: Diagram showing a crossing - two strands intersecting]**

### Crossing Sign

**Positive (+1):** Right-handed twist
**Negative (-1):** Left-handed twist

**[IMAGE PLACEHOLDER: Side-by-side comparison of positive vs negative crossing]**

The sign is determined by the orientation of the strands at the crossing.

## The KnotDiagram Class
```cpp
class KnotDiagram {
    std::vector<Crossing> crossings;  // All crossings in the knot
    int num_components;                // Number of separate loops
};
```

### Key Methods

**Creating from DT notation:**
```cpp
KnotDiagram trefoil = KnotDiagram::from_dt({4, 6, 2});
```

**Getting information:**
```cpp
int n = knot.size();           // Number of crossings
bool empty = knot.is_empty();  // Check if any crossings
```

**Accessing crossings:**
```cpp
const auto& crossings = knot.get_crossings();
for (const auto& c : crossings) {
    // Process each crossing
}
```

## Example: Trefoil Knot

DT notation: `[4, 6, 2]`

**[IMAGE PLACEHOLDER: Trefoil knot diagram with crossings labeled 0, 1, 2]**

Represents:
```cpp
Crossing 0: index=0, sign=+1
Crossing 1: index=1, sign=+1  
Crossing 2: index=2, sign=+1
```

All three crossings are positive (right-handed twists).

## Why This Representation?

**Advantages:**
- Simple and memory-efficient
- Easy to count crossings
- Signs needed for Jones polynomial conversion
- Can be extended later for full topology tracking

**Current Limitation:**
- Doesn't track which strands connect to which
- Component counting is simplified
- Good enough for basic algorithm, needs enhancement for accuracy

## What's Missing?

To compute the **exact** Kauffman bracket, we need to track:
- How smoothing changes the number of components (circles)
- Which regions are created/merged during smoothing

This is planned for the next implementation phase.
EOF