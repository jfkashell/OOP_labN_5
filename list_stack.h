#ifndef LIST_STACK_H
#define LIST_STACK_H

#include <memory_resource>
#include <iterator>

template <typename T>
class CustomStack {
    struct Item {
        T value;
        Item* next;
    };

    std::pmr::polymorphic_allocator<Item> _alloc;
    Item* _top;

public:
    CustomStack(std::pmr::memory_resource* r) : _alloc(r), _top(nullptr) {}

    ~CustomStack() {
        while (_top) pop();
    }

    void push(const T& val) {
        Item* new_item = _alloc.allocate(1);
        _alloc.construct(new_item, Item{val, _top});
        _top = new_item;
    }

    void pop() {
        if (!_top) return;
        Item* tmp = _top;
        _top = _top->next;
        
        _alloc.destroy(tmp);
        _alloc.deallocate(tmp, 1);
    }

    class Iterator {
    public:
        typedef std::forward_iterator_tag iterator_category;
        typedef T value_type;
        typedef std::ptrdiff_t difference_type;
        typedef T* pointer;
        typedef T& reference;

        Item* curr;

        Iterator(Item* ptr) : curr(ptr) {}

        T& operator*() { return curr->value; }
        
        Iterator& operator++() {
            if (curr) curr = curr->next;
            return *this;
        }

        bool operator!=(const Iterator& other) {
            return curr != other.curr;
        }
    };

    Iterator begin() { return Iterator(_top); }
    Iterator end() { return Iterator(nullptr); }
};

#endif