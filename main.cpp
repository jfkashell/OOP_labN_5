#include <iostream>
#include <string>
#include "mem.h"
#include "stack.h"git add main.cpp

struct Data {
    int id;
    std::string s;
};

int main() {
    MemRes mr;
    
    std::cout << "=== TEST 1: INT ===\n";
    Stack<int> s1(&mr);
    
    std::cout << "Pushing 10, 20:\n";
    s1.push(10);
    s1.push(20);
    
    std::cout << "Items: ";
    for(auto x : s1) std::cout << x << " ";
    std::cout << "\n\n";

    std::cout << "Pop (saving memory):\n";
    s1.pop(); 
    s1.pop(); 

    std::cout << "Push 30 (checking reuse):\n";
    s1.push(30); 

    std::cout << "\n=== TEST 2: STRUCT ===\n";
    Stack<Data> s2(&mr);
    s2.push({1, "Student"});
    std::cout << "Val: " << s2.begin().curr->val.s << "\n";
    
    return 0;
}