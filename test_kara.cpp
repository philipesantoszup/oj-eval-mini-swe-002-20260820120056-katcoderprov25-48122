#include "src/include/int2048.h"
#include <iostream>

int main() {
    // Test Karatsuba with numbers that trigger the recursion
    std::string s1 = "1234567890123456789012345678901234567890123456789012345678901234567890";
    std::string s2 = "9876543210987654321098765432109876543210987654321098765432109876543210";
    
    sjtu::int2048 a(s1);
    sjtu::int2048 b(s2);
    sjtu::int2048 c = a * b;
    
    std::cout << "a * b = ";
    c.print();
    std::cout << "\n";
    
    // Verify with Python-style calculation
    // Just check a smaller case
    sjtu::int2048 d("99999999999999999999999999999999999999999999999999");
    sjtu::int2048 e("99999999999999999999999999999999999999999999999999");
    sjtu::int2048 f = d * e;
    std::cout << "99...9 * 99...9 = ";
    f.print();
    std::cout << "\n(expected: 99999999999999999999999999999999999999999999999980000000000000000000000000000000000000000000000001)\n";
    
    return 0;
}
