#include <iostream>
#include "wrapper_mem.h"
#include "list_stack.h"

struct TestObj {
    int id;
    float val;
};

int main() {
    MyMemRes mr;

    std::cout << "--- START INT ---\n";
    StackList<int> s1(&mr);
    
    s1.push(10);
    s1.push(20);

    for (auto it = s1.begin(); it != s1.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "\n";

    s1.pop();
    s1.pop();

    std::cout << "--- REUSE CHECK ---\n";
    s1.push(30);

    std::cout << "--- START STRUCT ---\n";
    StackList<TestObj> s2(&mr);
    s2.push({1, 3.14f});
    
    return 0;
}