# How the Knot Calculator Works

## Overview

This program computes the Jones polynomial for mathematical knots using the Kauffman bracket algorithm.

## Table of Contents

1. [Polynomials](polynomials.md) - How we represent and manipulate polynomials
2. [Knot Representation](knot-representation.md) - How we store knots in memory
3. [The Algorithm](algorithm.md) - How we compute the Jones polynomial
4. [Optimization](optimization.md) - How we make it fast

---

## Quick Summary

**Input:** A knot (e.g., trefoil knot)
**Output:** A polynomial (e.g., -A^4 + A^3 + A)
**Method:** Recursively "smooth" crossings and combine results

EOF
I