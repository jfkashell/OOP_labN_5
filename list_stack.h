#ifndef LIST_STACK_H
#define LIST_STACK_H

#include <memory_resource>
#include <iterator>

template <typename T>
class MyStackContainer {
    struct Elem_t { 
        T data; 
        Elem_t* next_ptr; 
    };

    std::pmr::polymorphic_allocator<Elem_t> m_alloc;
    Elem_t* m_head;

public:
    MyStackContainer(std::pmr::memory_resource* res) : m_alloc(res) {
        m_head = nullptr;
    }

    ~MyStackContainer() {
        while (m_head != nullptr) {
            pop();
        }
    }

    void push(const T& val) {
        Elem_t* new_elem = m_alloc.allocate(1);
        m_alloc.construct(new_elem, Elem_t{val, m_head});
        m_head = new_elem;
    }

    void pop() {
        if (m_head == nullptr) return;
        
        Elem_t* old = m_head;
        m_head = m_head->next_ptr;
        
        m_alloc.destroy(old);
        m_alloc.deallocate(old, 1);
    }

    class StackIter {
    public:
        typedef std::forward_iterator_tag iterator_category;
        typedef T value_type;
        typedef std::ptrdiff_t difference_type;
        typedef T* pointer;
        typedef T& reference;

        Elem_t* ptr;

        StackIter(Elem_t* p) : ptr(p) {}

        T& operator*() { 
            return ptr->data; 
        }

        StackIter& operator++() {
            if (ptr) ptr = ptr->next_ptr;
            return *this;
        }

        bool operator!=(const StackIter& other) {
            return ptr != other.ptr;
        }
    };

    StackIter begin() { 
        return StackIter(m_head); 
    }
    
    StackIter end() { 
        return StackIter(nullptr); 
    }
};

#endif