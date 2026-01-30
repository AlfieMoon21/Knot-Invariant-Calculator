// polynomial.h
#include <map>
#include <iostream>

class Polynomial {
private:
    std::map<int, int> terms;  // power -> coefficient

/* 
**Example:** A³ + 2A⁻¹ - 1

terms[3] = 1      // A³ (coefficient is 1)
terms[-1] = 2     // 2A⁻¹ (coefficient is 2)  
terms[0] = -1     // -1 (this is -1×A⁰, and A⁰ = 1)
*/    
public:
    Polynomial() {}
    
    void add_term(int power, int coeff) { // add two terms together
        terms[power] += coeff;            // Add coefficient to existing
        if (terms[power] == 0) {          // If it cancels out
            terms.erase(power);           // Remove it completely
        }
    }

/*
**Example: Polynomial p;
p.add_term(3, 2);    // p = 2A³
p.add_term(3, 3);    // p = 5A³ (added 3 more)
p.add_term(3, -5);   // p = 0 (cancelled out, term removed)
*/
    
    Polynomial operator+(const Polynomial& other) const {   // adds two polynomials 
        Polynomial result = *this;                          // Copy this polynomial
        for (auto& [pow, coeff] : other.terms) {            // For each term in other
            result.add_term(pow, coeff);                    // Add it to result
        }
        return result;
    }
    
/*
- Start with a copy of the first polynomial
- Go through each term in the second polynomial
- Add them one by one

**Example:**
p1 = A³ + 2A⁻¹        →  terms: {3: 1, -1: 2}
p2 = A³ - 1           →  terms: {3: 1, 0: -1}

p1 + p2:
  Start with p1: {3: 1, -1: 2}
  Add A³: {3: 2, -1: 2}        (1+1=2)
  Add -1: {3: 2, -1: 2, 0: -1}
  
Result: 2A³ + 2A⁻¹ - 1
*/

    Polynomial operator*(const Polynomial& other) const {
        Polynomial result;
        for (auto& [p1, c1] : terms) {
            for (auto& [p2, c2] : other.terms) {
                result.add_term(p1 + p2, c1 * c2);
            }
        }
        return result;
    }
    
/*
- remember from algebra: `(2x²)(3x) = 6x³`
- Multiply coefficients: `2 × 3 = 6`
- Add powers: `2 + 1 = 3`

**Example:**
p1 = A² + A           →  terms: {2: 1, 1: 1}
p2 = A                →  terms: {1: 1}

p1 × p2:
  A² × A = A³         (power: 2+1=3, coeff: 1×1=1)
  A × A = A²          (power: 1+1=2, coeff: 1×1=1)
  
Result: A³ + A²
*/

    void print() const {
        bool first = true;
        for (auto& [power, coeff] : terms) {
            if (!first && coeff > 0) std::cout << "+";          // Add + sign
            if (coeff != 1 || power == 0) std::cout << coeff;
            if (power != 0) {
                std::cout << "A";                               // Print A
                if (power != 1) std::cout << "^" << power;      // Print exponent
            }
            first = false;
        }
        std::cout << std::endl;
    }
};

/*
onverts the internal storage to readable form:
{3: 1, -1: 2, 0: -1}  →  prints "1A^3+2A^-1-1"
*/