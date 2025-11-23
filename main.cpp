#include "mem.h"
#include "stack.h"
#include <iostream>
#include <string>

struct ComplexType {
    int id;
    std::string name;
    double value;
    
    friend std::ostream& operator<<(std::ostream& os, const ComplexType& obj) {
        os << "ID: " << obj.id << ", Name: " << obj.name << ", Value: " << obj.value;
        return os;
    }
};

template <typename T>
void demonstrate_stack(Stack<T>& stack) {
    std::cout << "Stack contents: ";
    for (auto it = stack.begin(); it != stack.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "\nTop element: " << stack.top() << std::endl;
    std::cout << "Stack size: " << stack.size() << std::endl;
}

int main() {
    VectorMemoryResource resource;
    
    {
        Stack<int> int_stack(&resource);
        std::cout << "=== Integer Stack Demo ===" << std::endl;
        
        int_stack.push(10);
        int_stack.push(20);
        int_stack.push(30);
        
        demonstrate_stack(int_stack);
        
        int_stack.pop();
        std::cout << "\nAfter pop:" << std::endl;
        demonstrate_stack(int_stack);
    }
    
    std::cout << "\n--- Memory reuse demonstration ---\n" << std::endl;
    
    {
        Stack<int> new_stack(&resource);
        new_stack.push(100);
        std::cout << "New stack with reused memory:" << std::endl;
        demonstrate_stack(new_stack);
    }
    
    std::cout << "\n=== Complex Type Stack Demo ===" << std::endl;
    
    Stack<ComplexType> complex_stack(&resource);
    
    complex_stack.push({1, "First", 10.5});
    complex_stack.push({2, "Second", 20.75});
    complex_stack.push({3, "Third", 30.0});
    
    std::cout << "Stack contents:" << std::endl;
    for (const auto& item : complex_stack) {
        std::cout << item << std::endl;
    }
    
    std::cout << "\nTop element: ";
    std::cout << complex_stack.top() << std::endl;
    
    complex_stack.pop();
    std::cout << "\nAfter pop, new top element: ";
    std::cout << complex_stack.top() << std::endl;
    
    return 0;
}