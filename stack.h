#ifndef STACK_H
#define STACK_H

#include <memory_resource>
#include <iterator>

template <typename T>
class Stack {
    struct Node { T val; Node* next; };
    
    std::pmr::polymorphic_allocator<Node> alloc;
    Node* top = nullptr;

public:
    Stack(std::pmr::memory_resource* r) : alloc(r) {}
    
    ~Stack() { 
        while (top) pop(); 
    }

    void push(const T& v) {
        Node* n = alloc.allocate(1);
        alloc.construct(n, Node{v, top});
        top = n;
    }

    void pop() {
        if (!top) return;
        Node* t = top;
        top = top->next;
        alloc.destroy(t);
        alloc.deallocate(t, 1);
    }

    struct Iter {
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        Node* curr;
        Iter(Node* c) : curr(c) {}
        
        T& operator*() { return curr->val; }
        Iter& operator++() { curr = curr->next; return *this; }
        bool operator!=(const Iter& o) { return curr != o.curr; }
    };

    Iter begin() { return Iter(top); }
    Iter end() { return Iter(nullptr); }
};

#endif