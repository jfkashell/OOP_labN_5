#include <iostream>
#include <vector>
#include <memory_resource>
#include <algorithm>
#include <cstdio>

#define LOG(fmt, ...) printf(fmt, __VA_ARGS__)

int g_counter = 0;

class MyMem : public std::pmr::memory_resource {
    struct block_t {
        void* p;
        size_t sz;
    };

    std::vector<block_t> cache;
    std::vector<void*> sys;

public:
    ~MyMem() {
        for (size_t i = 0; i < sys.size(); ++i) {
            ::operator delete(sys[i]);
        }
        LOG(">> Dead. Total: %d\n", g_counter);
    }

protected:
    void* do_allocate(size_t bytes, size_t align) override {
        g_counter++;
        
        int idx = -1;

        for (int i = 0; i < (int)cache.size(); ++i) {
            if (cache[i].sz == bytes) {
                idx = i;
                break;
            }
        }

        if (idx != -1) {
            void* ret = cache[idx].p;
            
            cache[idx] = cache.back();
            cache.pop_back();

            LOG("[MEM] Reuse: %p (%zu)\n", ret, bytes);
            return ret;
        }

        void* ptr = ::operator new(bytes, std::align_val_t(align));
        sys.push_back(ptr);
        LOG("[MEM] New: %p (%zu)\n", ptr, bytes);
        return ptr;
    }

    void do_deallocate(void* p, size_t bytes, size_t align) override {
        block_t b;
        b.p = p;
        b.sz = bytes;
        cache.push_back(b);
        LOG("[MEM] Free: %p\n", p);
    }

    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override {
        return this == &other;
    }
};

template <typename T>
class Stk {
    struct Node {
        T val;
        Node* next;
    };

    std::pmr::polymorphic_allocator<Node> al;
    Node* top;

public:
    Stk(std::pmr::memory_resource* r) : al(r) {
        top = 0;
    }

    ~Stk() {
        while (top != 0) {
            pop();
        }
    }

    void push(const T& v) {
        Node* n = al.allocate(1);
        al.construct(n, Node{v, top});
        top = n;
    }

    void pop() {
        if (top == 0) return;
        
        Node* tmp = top;
        top = top->next;
        
        al.destroy(tmp);
        al.deallocate(tmp, 1);
    }

    class It {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        Node* cur;
        It(Node* c) : cur(c) {}

        T& operator*() { return cur->val; }
        It& operator++() { 
            if(cur) cur = cur->next; 
            return *this; 
        }
        bool operator!=(const It& o) { return cur != o.cur; }
    };

    It begin() { return It(top); }
    It end() { return It(0); }
};

struct Data {
    int x;
    float y;
};

int main() {
    MyMem m;

    std::cout << "--- INT ---\n";
    {
        Stk<int> s(&m);
        s.push(1);
        s.push(2);
        
        for(auto x : s) std::cout << x << " ";
        std::cout << "\n";

        s.pop(); 
        s.pop();

        std::cout << "--- CHECK ---\n";
        s.push(3);
    }

    std::cout << "\n--- DATA ---\n";
    {
        Stk<Data> sd(&m);
        sd.push({10, 5.5});
        std::cout << sd.begin().cur->val.y << "\n";
    }

    return 0;
}