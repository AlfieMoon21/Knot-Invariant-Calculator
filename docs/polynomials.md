# Polynomial Representation

## What is a Laurent Polynomial?

A Laurent polynomial is like a regular polynomial, but powers can be **negative**.

**Examples:**
- Regular polynomial: `3x² + 5x - 2`
- Laurent polynomial: `A³ + 2A⁻¹ - 1`

## Why We Need This

The Jones polynomial (our output) uses negative powers of A.

Example: The trefoil knot's Jones polynomial is `-A⁻⁴ + A⁻³ + A`

## How We Store It

We use a C++ `std::map<int, int>`:
```cpp
std::map<int, int> terms;  // power -> coefficient
```

**Example:** The polynomial `A³ + 2A⁻¹ - 1` is stored as:
```
terms[3] = 1      // A³
terms[-1] = 2     // 2A⁻¹  
terms[0] = -1     // -1
```

## Operations

### Addition
Combine like terms: `A³ + A³ = 2A³`
```cpp
Polynomial operator+(const Polynomial& other) {
    // Copy this polynomial
    Polynomial result = *this;
    // Add each term from other
    for (auto& [power, coeff] : other.terms) {
        result.add_term(power, coeff);
    }
    return result;
}
```

### Multiplication
Add powers, multiply coefficients: `A² × A³ = A⁵`
```cpp
Polynomial operator*(const Polynomial& other) {
    Polynomial result;
    // Multiply each term with each other term
    for (auto& [p1, c1] : terms) {
        for (auto& [p2, c2] : other.terms) {
            result.add_term(p1 + p2, c1 * c2);  // Add powers!
        }
    }
    return result;
}
```