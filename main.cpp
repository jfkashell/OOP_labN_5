#include <iostream>
#include <string>
#include "wrapper_mem.h"
#include "list_stack.h"

void print_sep(const std::string& title) {
    std::cout << "\n=== " << title << " ===\n";
}

struct TestStruct {
    int id;
    std::string name;
};

int main() {
    LabMemoryResource mem;
    
    print_sep("Int Test Start");
    
    MyStackContainer<int> st1(&mem);
    
    st1.push(100);
    st1.push(200);
    
    std::cout << "Current stack: ";
    for (auto it = st1.begin(); it != st1.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "\n";

    st1.pop();
    st1.pop();

    std::cout << "Adding new item (Check Reuse):\n";
    st1.push(300);

    print_sep("Struct Test Start");
    
    MyStackContainer<TestStruct> st2(&mem);
    st2.push({1, "Alpha"});
    
    auto it = st2.begin();
    std::cout << "Top: " << it.ptr->data.name << "\n";
    
    return 0;
}