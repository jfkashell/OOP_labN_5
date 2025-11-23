#ifndef WRAPPER_MEM_H
#define WRAPPER_MEM_H

#include <vector>
#include <iostream>
#include <memory_resource>

class MyMemRes : public std::pmr::memory_resource {
    struct block {
        void* ptr;
        size_t size;
    };

    std::vector<block> _pool; 
    std::vector<void*> _all;

public:
    ~MyMemRes() {
        for (size_t i = 0; i < _all.size(); ++i) {
            ::operator delete(_all[i]);
        }
    }

protected:
    void* do_allocate(size_t bytes, size_t align) override {
        for (size_t i = 0; i < _pool.size(); ++i) {
            if (_pool[i].size == bytes) {
                void* p = _pool[i].ptr;
                _pool.erase(_pool.begin() + i);
                std::cout << "alloc: reuse " << p << std::endl;
                return p;
            }
        }

        void* p = ::operator new(bytes, std::align_val_t(align));
        _all.push_back(p);
        std::cout << "alloc: new " << p << " (" << bytes << ")" << std::endl;
        return p;
    }

    void do_deallocate(void* p, size_t bytes, size_t align) override {
        block b;
        b.ptr = p;
        b.size = bytes;
        _pool.push_back(b);
        std::cout << "dealloc: save " << p << std::endl;
    }

    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override {
        return this == &other;
    }
};

#endif