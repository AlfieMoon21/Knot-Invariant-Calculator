# How the Knot Calculator Works

## Overview
This program computes the Jones polynomial for mathematical knots using the Kauffman bracket algorithm.

## Current Status (Week 2)
 **Implemented:**
- Polynomial arithmetic (addition, multiplication, scaling by A^n)
- Knot diagram representation
- DT notation parser
- Simplified Kauffman bracket (recursive structure)

 **In Progress:**
- Component tracking during smoothing
- Proper base case with multiple circles

 **Planned:**
- Memoization for optimization
- Conversion to Jones polynomial
- Performance profiling

## Table of Contents
1. [Polynomials](polynomials.md) - How we represent and manipulate polynomials
2. [Knot Representation](knot-representation.md) - How we store knots in memory
3. [DT Notation](dt-notation.md) - How we parse input
4. [The Algorithm](algorithm.md) - How we compute the Kauffman bracket
5. [Next Steps](NEXT_STEPS.md) - What needs to be implemented

---

## Quick Summary

**Input:** A knot in DT notation (e.g., `[4, 6, 2]` for trefoil)
**Output:** Kauffman bracket polynomial (e.g., `A^7 - A^3 - A^5`)
**Method:** Recursively "smooth" crossings and combine results

### The Process Flow
```
[4, 6, 2] (DT notation)
    ↓
KnotDiagram (3 crossings)
    ↓
Kauffman Bracket (recursive computation)
    ↓
Polynomial result
    ↓
Convert to Jones polynomial (future)
```

## How It Works

### 1. Parse Input
DT notation `[4, 6, 2]` → 3 crossings, all positive

### 2. Recursive Smoothing
Pick a crossing, smooth it two ways:
- **A-smoothing:** multiply result by A
- **B-smoothing:** multiply result by A^(-1)

Combine: `⟨K⟩ = A × ⟨A-smooth⟩ + A^(-1) × ⟨B-smooth⟩`

### 3. Base Case
When no crossings remain:
- 1 circle → return 1
- Multiple circles → return `(-A² - A^(-2))^(n-1)`

### 4. Build Polynomial
Recursive calls build up the polynomial term by term.

## Example: 1 Crossing
```
Start: 1 crossing
    ↓
Smooth it both ways
    ↓
A × ⟨0 crossings⟩ + A^(-1) × ⟨0 crossings⟩
    ↓
A × 1 + A^(-1) × 1
    ↓
Result: A + A^(-1)
```

## Complexity

- **Time:** O(2^n) without memoization (n = number of crossings)
- **Space:** O(n) for recursion depth + polynomial storage
- **Example:** 3 crossings = 15 recursive calls, 10 crossings = 2047 calls

This exponential growth is why optimization is crucial!
EOF