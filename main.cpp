#include <iostream>
#include <vector>
#include <memory_resource>
#include <algorithm>
#include <cstdio>

#define DBG_PRINT(fmt, ...) printf(fmt, __VA_ARGS__)

int G_ALLOC_COUNT = 0;

class Lab17Resource : public std::pmr::memory_resource {
    struct _chunk_info {
        void* addr;
        size_t len;
    };
    std::vector<_chunk_info> _pool;
    std::vector<void*> _sys;
public:
    ~Lab17Resource() {
        for (auto p : _sys) { 
            ::operator delete(p); 
        }
        DBG_PRINT(">> Resource died. Total allocs: %d\n", G_ALLOC_COUNT);
    }
protected:
    void* do_allocate(size_t bytes, size_t alignment) override {
        G_ALLOC_COUNT++;
        
        for (int i = 0; i < (int)_pool.size(); i++) {
            if (_pool[i].len == bytes) {
                void* ptr = _pool[i].addr;
                
                _pool[i] = _pool.back();
                _pool.pop_back();

                DBG_PRINT("[MEM] Reuse old block: %p (%zu b)\n", ptr, bytes);
                return ptr;
            }
        }
        void* ptr = ::operator new(bytes, std::align_val_t(alignment));
        _sys.push_back(ptr);
        DBG_PRINT("[MEM] System new: %p (%zu b)\n", ptr, bytes);
        return ptr;
    }
    void do_deallocate(void* p, size_t bytes, size_t alignment) override {
        _pool.push_back({p, bytes});
        DBG_PRINT("[MEM] Return to pool: %p\n", p);
    }
    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override {
        return this == &other;
    }
};

template <typename T>
class SimpleStack {
private:
    struct Node {
        T data;
        Node* next;
    };

    std::pmr::polymorphic_allocator<Node> _allocator;
    Node* _head;

public:
    SimpleStack(std::pmr::memory_resource* mr) 
        : _allocator(mr), _head(nullptr) 
    {}
    ~SimpleStack() {
        while (_head) {
            pop();
        }
    }
    void push(const T& val) {
        Node* n = _allocator.allocate(1);
        _allocator.construct(n, Node{val, _head});
        _head = n;
    }
    void pop() {
        if (!_head) return;
        
        Node* tmp = _head;
        _head = _head->next;
        
        _allocator.destroy(tmp);
        _allocator.deallocate(tmp, 1);
    }

    struct MyIter {
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        Node* curr;
        MyIter(Node* c) : curr(c) {}
        T& operator*() { return curr->data; }
        MyIter& operator++() { 
            if(curr) curr = curr->next; 
            return *this; 
        }
        bool operator!=(const MyIter& o) { return curr != o.curr; }
    };

    MyIter begin() { return MyIter(_head); }
    MyIter end() { return MyIter(nullptr); }
};

struct MyStruct {
    int id;
    float val;
};

int main() {
    Lab17Resource res;

    std::cout << "--- TEST 1 (INT) ---\n";
    {
        SimpleStack<int> s(&res);
        s.push(10);
        s.push(20);
        
        for(auto x : s) {
            std::cout << x << " ";
        }
        std::cout << "\n";

        s.pop(); 
        s.pop();

        std::cout << "Pushing new item (expect reuse)...\n";
        s.push(999);
    }

    std::cout << "\n--- TEST 2 (STRUCT) ---\n";
    {
        SimpleStack<MyStruct> s2(&res);
        s2.push({1, 3.14f});
        std::cout << "Top struct val: " << s2.begin().curr->data.val << "\n";
    }
    return 0;
}