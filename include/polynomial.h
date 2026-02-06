#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H

#include <map>
#include <iostream>

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
    
    Polynomial operator+(const Polynomial& other) const {
        Polynomial result = *this;
        for (auto& [pow, coeff] : other.terms) {
            result.add_term(pow, coeff);
        }
        return result;
    }
    
    Polynomial operator*(const Polynomial& other) const {
        Polynomial result;
        for (auto& [p1, c1] : terms) {
            for (auto& [p2, c2] : other.terms) {
                result.add_term(p1 + p2, c1 * c2);
            }
        }
        return result;
    }
    
    // Multiply by A^n
    Polynomial multiply_by_A(int power) const {
        Polynomial result;
        for (auto& [p, c] : terms) {
            result.add_term(p + power, c);
        }
        return result;  // FIXED: Added semicolon
    }
    
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
                std::cout << (coeff > 0 ? " + " : " - ");  // FIXED: semicolon
                coeff = abs(coeff);
            } else if (coeff < 0) {
                std::cout << "-";
                coeff = abs(coeff);
            }
            
            if (coeff != 1 || power == 0) {  // FIXED: == instead of ++
                std::cout << coeff;
            }
            
            if (power != 0) {
                std::cout << "A";
                if (power != 1) {
                    std::cout << "^" << power;
                }
            }
            
            first = false;
        }
    }
};

#endif