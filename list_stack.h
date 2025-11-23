#ifndef LIST_STACK_H
#define LIST_STACK_H

#include <memory_resource>
#include <iterator>

template <typename T>
class StackList {
    struct node {
        T val;
        node* next;
    };

    std::pmr::polymorphic_allocator<node> al;
    node* head;

public:
    StackList(std::pmr::memory_resource* r) : al(r) {
        head = nullptr;
    }

    ~StackList() {
        while (head) pop();
    }

    void push(const T& v) {
        node* n = al.allocate(1);
        al.construct(n, node{v, head});
        head = n;
    }

    void pop() {
        if (!head) return;
        node* tmp = head;
        head = head->next;
        al.destroy(tmp);
        al.deallocate(tmp, 1);
    }

    class Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        node* cur;
        Iterator(node* c) : cur(c) {}

        T& operator*() { return cur->val; }
        
        Iterator& operator++() {
            if (cur) cur = cur->next;
            return *this;
        }

        bool operator!=(const Iterator& o) { return cur != o.cur; }
    };

    Iterator begin() { return Iterator(head); }
    Iterator end() { return Iterator(nullptr); }
};

#endif