#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H

#include <map>
#include <iostream>

// Sparse Laurent polynomial in A with integer coefficients.
// Stored as a map of power → coefficient; zero-coefficient terms are erased.
//
// OPTIMISATION: sparse storage matters here — Kauffman bracket polynomials have
// O(n) non-zero terms despite spanning a power range of O(n), so a dense array
// would waste time and space on zero entries.
class Polynomial {
private:
    std::map<int, int> terms;  // power -> coefficient

public:
    Polynomial() {}
    
    void add_term(int power, int coeff) {
        terms[power] += coeff;
        if (terms[power] == 0) {
            terms.erase(power);
        }
    }
    
    // Term-wise addition; zero coefficients are removed by add_term.
    Polynomial operator+(const Polynomial& other) const {
        Polynomial result = *this;
        for (auto& [pow, coeff] : other.terms) {
            result.add_term(pow, coeff);
        }
        return result;
    }

    // Full convolution: O(|this| * |other|).
    // Only used in compute_delta_power (small polynomials) — not on the hot path.
    Polynomial operator*(const Polynomial& other) const {
        Polynomial result;
        for (auto& [p1, c1] : terms) {
            for (auto& [p2, c2] : other.terms) {
                result.add_term(p1 + p2, c1 * c2);
            }
        }
        return result;
    }
    
    // OPTIMISATION: shift all exponents by `power` in O(|terms|), used by the
    // skein relation (<K> = A*<A-smooth> + A^{-1}*<B-smooth>). Both multiplications
    // are by a monomial, so a pure exponent shift is enough — operator* would be
    // O(|terms|²) for no gain.
    Polynomial multiply_by_A(int power) const {
        Polynomial result;
        for (auto& [p, c] : terms) {
            result.add_term(p + power, c);
        }
        return result;
    }
    
    // Print in descending-power order, e.g. "A^4 - 2A + 3A^-2".
    // Omits coefficient 1 on non-constant terms and omits "A" on the constant term.
    void print() const {
        if (terms.empty()) {
            std::cout << "0";
            return;
        }

        bool first = true;
        for (auto it = terms.rbegin(); it != terms.rend(); ++it) {
            int power = it->first;
            int coeff = it->second;

            if (!first) {
                std::cout << (coeff > 0 ? " + " : " - ");
                coeff = abs(coeff);
            } else if (coeff < 0) {
                std::cout << "-";
                coeff = abs(coeff);
            }

            if (coeff != 1 || power == 0) std::cout << coeff;

            if (power != 0) {
                std::cout << "A";
                if (power != 1) std::cout << "^" << power;
            }

            first = false;
        }
    }
};

#endif