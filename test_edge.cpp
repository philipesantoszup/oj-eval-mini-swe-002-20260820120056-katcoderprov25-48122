#include "src/include/int2048.h"
#include <iostream>
#include <sstream>

int main() {
    // Edge case: division by 1
    sjtu::int2048 a("123456789");
    sjtu::int2048 one(1);
    std::cout << "123456789 / 1 = ";
    (a / one).print();
    std::cout << "\n";
    
    // Edge case: 0 / n
    sjtu::int2048 zero(0);
    sjtu::int2048 b("999999999");
    std::cout << "0 / 999999999 = ";
    (zero / b).print();
    std::cout << "\n";
    
    // Edge case: n / n
    std::cout << "999999999 / 999999999 = ";
    (b / b).print();
    std::cout << "\n";
    
    // Edge case: small / large
    sjtu::int2048 small(5);
    sjtu::int2048 large("1000000000000");
    std::cout << "5 / 1000000000000 = ";
    (small / large).print();
    std::cout << "\n";
    
    // Edge case: stream input
    std::istringstream iss("123456789012345678901234567890 -987654321098765432109876543210");
    sjtu::int2048 x, y;
    iss >> x >> y;
    std::cout << "Stream read: x = ";
    x.print();
    std::cout << ", y = ";
    y.print();
    std::cout << "\n";
    
    // Edge case: -0
    sjtu::int2048 neg0("-0");
    std::cout << "-0 = ";
    neg0.print();
    std::cout << "\n";
    std::cout << "-0 == 0: " << (neg0 == sjtu::int2048(0)) << "\n";
    
    // Edge case: multiplication by 0
    sjtu::int2048 big("123456789012345678901234567890");
    std::cout << "big * 0 = ";
    (big * zero).print();
    std::cout << "\n";
    
    // Edge case: 0 * big
    std::cout << "0 * big = ";
    (zero * big).print();
    std::cout << "\n";
    
    // Edge case: negative * 0
    sjtu::int2048 neg("-123456789");
    std::cout << "-123456789 * 0 = ";
    (neg * zero).print();
    std::cout << "\n";
    
    return 0;
}
