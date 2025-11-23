#ifndef WRAPPER_MEM_H
#define WRAPPER_MEM_H

#include <vector>
#include <memory_resource>
#include <utility>
#include <cstdio>  

class LabMemResource : public std::pmr::memory_resource {
    std::vector<std::pair<void*, size_t>> _cache;
    std::vector<void*> _system_allocs;

public:
    ~LabMemResource() {
        for (void* p : _system_allocs) {
            ::operator delete(p);
        }
    }

protected:
    void* do_allocate(size_t bytes, size_t align) override {
        for (size_t i = 0; i < _cache.size(); ++i) {
            if (_cache[i].second == bytes) {
                void* ptr = _cache[i].first;

                _cache[i] = _cache.back();
                _cache.pop_back();

                printf("[MEM] Reuse: %p (%zu b)\n", ptr, bytes);
                return ptr;
            }
        }

        void* ptr = ::operator new(bytes, std::align_val_t(align));
        _system_allocs.push_back(ptr);
        printf("[MEM] System Alloc: %p (%zu b)\n", ptr, bytes);
        return ptr;
    }

    void do_deallocate(void* p, size_t bytes, size_t align) override {
        _cache.push_back(std::make_pair(p, bytes));
        printf("[MEM] Saved to pool: %p\n", p);
    }

    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override {
        return this == &other;
    }
};

#endif