int main() {
    Polynomial p1, p2;
    p1.add_term(3, 1);   // A^3
    p1.add_term(-1, 2);  // 2A^(-1)
    
    p2.add_term(1, 1);   // A
    
    Polynomial sum = p1 + p2;
    Polynomial prod = p1 * p2;
    
    std::cout << "Sum: "; sum.print();
    std::cout << "Product: "; prod.print();
    
    return 0;
}