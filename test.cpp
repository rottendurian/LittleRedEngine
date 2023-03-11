#include <iostream>

struct example {
    int x;
    int getX() {return x;}
};


int main() {
    example test = {32};
    test.getX();

    const example *const ptr = &(const example)test;
    *ptr->getX();

    return 0;
}


    // int test = 32;

    // int *const ptr = &(const int)test;

    // std::cout << test;
    
    // *ptr--;

    // std::cout << test;