#include <iostream>
#include <map>

// Inline the Polynomial class for now
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
    
    void print() const {
        if (terms.empty()) {
            std::cout << "0";
            return;
        }
        
        bool first = true;
        for (auto& [power, coeff] : terms) {
            if (!first && coeff > 0) std::cout << "+";
            if (coeff != 1 || power == 0) std::cout << coeff;
            if (power != 0) {
                std::cout << "A";
                if (power != 1) std::cout << "^" << power;
            }
            first = false;
        }
        std::cout << std::endl;
    }
};

int main() {
    std::cout << "=== Testing Polynomial Class ===" << std::endl;
    
    // Test 1: Simple addition
    std::cout << "\nTest 1: Addition" << std::endl;
    Polynomial p1, p2;
    p1.add_term(3, 1);    // A^3
    p1.add_term(-1, 2);   // + 2A^(-1)
    
    p2.add_term(3, 1);    // A^3
    p2.add_term(0, -1);   // - 1
    
    std::cout << "p1 = "; p1.print();
    std::cout << "p2 = "; p2.print();
    
    Polynomial sum = p1 + p2;
    std::cout << "p1 + p2 = "; sum.print();
    std::cout << "Expected: 2A^3 + 2A^(-1) - 1" << std::endl;
    
    // Test 2: Multiplication
    std::cout << "\nTest 2: Multiplication" << std::endl;
    Polynomial p3, p4;
    p3.add_term(1, 1);    // A
    p4.add_term(2, 1);    // A^2
    
    std::cout << "p3 = "; p3.print();
    std::cout << "p4 = "; p4.print();
    
    Polynomial prod = p3 * p4;
    std::cout << "p3 * p4 = "; prod.print();
    std::cout << "Expected: A^3" << std::endl;
    
    // Test 3: Trefoil polynomial (known result)
    std::cout << "\nTest 3: Build Trefoil Result" << std::endl;
    Polynomial trefoil;
    trefoil.add_term(-4, -1);  // -A^(-4)
    trefoil.add_term(-3, 1);   // + A^(-3)
    trefoil.add_term(1, 1);    // + A
    
    std::cout << "Trefoil Jones: "; trefoil.print();
    std::cout << "Expected: -A^(-4) + A^(-3) + A" << std::endl;
    
    std::cout << "\n=== All Tests Complete ===" << std::endl;
    
    return 0;
}