# Project Proposal: Knot Invariant Calculator

## Quick Start
```bash
# Compile
make

# Run tests
./knot_calc
```

(Full functionality coming soon - currently in development)

## Current Status

**Week 2:**
-  Polynomial arithmetic implemented and tested
-  Knot representation in progress
-  Kauffman bracket algorithm (next)
-  Optimization and profiling (later)

See [DEVELOPMENT.md](DEVELOPMENT.md) and [docs/HOW_IT_WORKS.md](docs/HOW_IT_WORKS.md) for details.

---

## Problem Statement

Compute the Jones polynomial for mathematical knots. The Jones polynomial is a knot invariant that assigns a Laurent polynomial to a knot in a way that is independent of the particular diagram used to represent it. Computing the Jones polynomial is computationally intensive due to the exponential growth in the number of states that must be evaluated as the number of crossings increases.

## Motivation and Context

Knots can be represented in multiple ways depending on purpose. Diagrammatic encodings such as Dowker–Thistlethwaite (DT) notation or Gauss codes provide a compact, computable description of a specific knot diagram, while knot invariants such as the Jones polynomial provide an abstract, diagram-independent characterization of the knot itself.

This project focuses on bridging these two perspectives: transforming a concrete, diagram-dependent encoding (DT notation) into a diagram-independent invariant (the Jones polynomial). This transformation is fundamental in computational knot theory and reflects the broader challenge of extracting invariant information from combinatorial data.

The Jones polynomial is particularly well-suited for this task as it is powerful enough to distinguish many non-equivalent knots, yet complex enough to pose significant computational and algorithmic challenges.

## Why This Problem?

* Translates between diagram-level representations and topological invariants
* Computationally complex (exponential time complexity)
* Large state space even for modest knots (10+ crossings)
* Significant memory management challenges
* Directly relevant to my dissertation work in knot theory
* Rich opportunities for algorithmic and data-structure optimization

## Representation vs Invariant

**DT notation / Gauss codes:**
* Encode a specific knot diagram
* Diagram-dependent and non-unique
* Suitable as algorithmic input
* Preserve full combinatorial structure of the knot diagram

**Jones polynomial:**
* A knot invariant
* Independent of diagram choice
* Cannot reconstruct the original knot
* Used for classification and comparison of knots

This project computes a knot invariant from a non-invariant representation, ensuring that different diagrammatic inputs corresponding to the same knot produce identical output polynomials (up to normalization).

## Key Characteristics

* **Input:** Knot diagrams (Dowker–Thistlethwaite notation or Gauss codes)
* **Output:** Jones polynomial V_K(t)
* **Algorithm:** Kauffman bracket expansion with skein relations
* **Complexity:** O(2^n) for n crossings without optimization
* **Data Source:** KnotInfo database (knots up to 12+ crossings)

## Data Structures

* Knot diagram representation (crossings, strands, adjacency)
* Sparse polynomial representation (`map<int, int>` for power → coefficient)
* State cache for memoization
* Efficient diagram encoding for hashing and equality checks

## Optimization Targets

* Memoization to eliminate redundant state evaluations
* Efficient sparse polynomial arithmetic
* Cache-friendly memory layouts
* State-space pruning where possible

## Development Plan

1. Basic implementation in C++ on university machines
2. Profiling using `valgrind`, `gprof`, and `perf`
3. Algorithmic and data-structure optimization
4. Optional port to Raspberry Pi B v1.2
5. Comparative performance analysis

## Expected Challenges

* Managing exponential state explosion
* Memory constraints on target hardware
* Ensuring correctness of polynomial arithmetic
* Validating output against known knot invariants

## Documentation

* [How It Works](docs/HOW_IT_WORKS.md) - High-level overview
* [Polynomials](docs/polynomials.md) - Laurent polynomial implementation
* [Development Log](DEVELOPMENT.md) - Week-by-week progress

## References

* Kauffman, L. H. (1987). State models and the Jones polynomial. *Topology*, 26(3), 395-407.
* Jones, V. F. R. (1985). A polynomial invariant for knots via von Neumann algebras. *Bulletin of the AMS*, 12(1), 103-111.
* KnotInfo: https://knotinfo.math.indiana.edu/