#ifndef LIST_STACK_H
#define LIST_STACK_H

#include <memory_resource>
#include <iterator>

template <typename T>
class MyStack {
private:
    struct _link {
        T val;
        _link* next;
    };

    std::pmr::polymorphic_allocator<_link> _al;
    _link* _top;

public:
    MyStack(std::pmr::memory_resource* mr) : _al(mr) {
        this->_top = nullptr;
    }

    ~MyStack() {
        while (this->_top != nullptr) {
            this->pop();
        }
    }

    void push(const T& obj) {
        _link* ptr = this->_al.allocate(1);
        this->_al.construct(ptr, _link{obj, this->_top});
        this->_top = ptr;
    }

    void pop() {
        if (this->_top == nullptr) {
            return;
        }
        _link* tmp = this->_top;
        this->_top = this->_top->next;
        
        this->_al.destroy(tmp);
        this->_al.deallocate(tmp, 1);
    }

    class Iter {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        Iter(_link* p) : _curr(p) {}

        T& operator*() { 
            return this->_curr->val; 
        }
        
        Iter& operator++() {
            if (this->_curr) {
                this->_curr = this->_curr->next;
            }
            return *this;
        }

        bool operator!=(const Iter& right) {
            return this->_curr != right._curr;
        }

    private:
        _link* _curr;
    };

    Iter begin() { 
        return Iter(this->_top); 
    }
    
    Iter end() { 
        return Iter(nullptr); 
    }
};

#endif