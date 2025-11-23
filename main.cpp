#include <iostream>
#include "wrapper_mem.h"
#include "list_stack.h"

struct XData {
    int a;
    int b;
};

int main() {
    Lab5Mem resource;

    std::cout << ">>> Test 1 (Integers) <<<\n";
    MyStack<int> st(&resource);
    
    st.push(111);
    st.push(222);
    st.push(333);

    auto it = st.begin();
    while (it != st.end()) {
        std::cout << *it << " ";
        ++it;
    }
    std::cout << "\n";

    st.pop();
    st.pop();
    st.pop(); 

    std::cout << ">>> Test Reuse <<<\n";
    st.push(444); 
    st.push(555);

    std::cout << ">>> Test 2 (Structs) <<<\n";
    MyStack<XData> st_str(&resource);
    st_str.push({1, 2});
    
    std::cout << "Done.\n";
    return 0;
}