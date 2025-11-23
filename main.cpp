#include <iostream>
#include "wrapper_mem.h"
#include "list_stack.h"

struct MyData {
    int id;
    double val;
};

int main() {
    LabMemResource res;
    
    std::cout << "--- Integer Stack ---\n";
    CustomStack<int> stack_int(&res);
    
    stack_int.push(10);
    stack_int.push(20);

    for (auto val : stack_int) {
        std::cout << val << " ";
    }
    std::cout << "\n";

    stack_int.pop();
    stack_int.pop();

    std::cout << "--- Reuse Check ---\n";
    stack_int.push(99);

    std::cout << "--- Struct Stack ---\n";
    CustomStack<MyData> stack_data(&res);
    stack_data.push({1, 3.14});
    
    return 0;
}