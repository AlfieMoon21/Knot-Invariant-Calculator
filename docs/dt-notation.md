# Dowker-Thistlethwaite (DT) Notation

## What is DT Notation?

A compact way to represent knot diagrams as a sequence of integers.

**Example:** The trefoil knot is `[4, 6, 2]`

## How It Works

### The Basic Idea

1. Number the strands from 1 to 2n (where n = number of crossings)
2. Follow the knot, alternating between under/over crossings
3. Record which strand crosses over which

### Odd vs Even

- **Odd numbers (1, 3, 5, ...):** understrands
- **Even numbers (2, 4, 6, ...):** overstrands

### Reading DT Notation

For `[4, 6, 2]`:
- **Position 1 (value 4):** Strand 1 goes under strand 4
- **Position 2 (value 6):** Strand 3 goes under strand 6
- **Position 3 (value 2):** Strand 5 goes under strand 2

### Positive vs Negative Crossings

The **sign** of the number indicates crossing type:
- **Positive (e.g., 4):** Positive crossing (right-handed)
- **Negative (e.g., -4):** Negative crossing (left-handed)

## Examples

### Trefoil: `[4, 6, 2]`
- 3 crossings, all positive
- Classic right-handed trefoil knot

### Figure-Eight: `[4, 6, 8, 2]`
- 4 crossings, all positive
- The figure-eight knot

### Mixed Signs: `[4, -6, 2]`
- 3 crossings: positive, negative, positive
- A different knot than the trefoil

## How We Parse It
```cpp
KnotDiagram parse_dt(vector<int> dt_notation) {
    KnotDiagram knot;
    for (int i = 0; i < dt_notation.size(); i++) {
        int value = dt_notation[i];
        int sign = (value > 0) ? 1 : -1;  // Determine crossing type
        knot.add_crossing(i, sign);
    }
    return knot;
}
```

## Usage in Our Code
```cpp
// Create a trefoil
std::vector<int> trefoil_dt = {4, 6, 2};
KnotDiagram trefoil = KnotDiagram::from_dt(trefoil_dt);

// Create a figure-eight
std::vector<int> fig8_dt = {4, 6, 8, 2};
KnotDiagram fig8 = KnotDiagram::from_dt(fig8_dt);
```

## Where to Find DT Notation

**KnotInfo Database:** https://knotinfo.math.indiana.edu/
- Contains DT notation for thousands of knots
- Can download and use as test data

## Limitations

DT notation:
-  Compact and easy to parse
-  Uniquely identifies a specific diagram
-  Different diagrams of the same knot have different DT notations
-  Doesn't directly encode the invariant properties

This is why we compute the Jones polynomial - it's diagram-independent!
EOF