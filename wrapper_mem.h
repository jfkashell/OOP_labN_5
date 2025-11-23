#ifndef WRAPPER_MEM_H
#define WRAPPER_MEM_H

#include <vector>
#include <iostream>
#include <memory_resource>

#define PRT(x) std::cout << x << std::endl

class Lab5Mem : public std::pmr::memory_resource {
public:
    ~Lab5Mem() {
        while (!_sys_ptrs.empty()) {
            void* last = _sys_ptrs.back();
            _sys_ptrs.pop_back();
            ::operator delete(last);
        }
    }

protected:
    void* do_allocate(size_t n, size_t algn) override {
        bool found = false;
        int idx = -1;

        for (int i = (int)_storage.size() - 1; i >= 0; --i) {
            if (_storage[i].sz == n) {
                idx = i;
                found = true;
                break;
            }
        }

        if (found && idx != -1) {
            void* ret = _storage[idx].p;
            _storage.erase(_storage.begin() + idx);
            std::cout << "[MEM] Reuse: " << ret << std::endl;
            return ret;
        }

        void* ptr = ::operator new(n, std::align_val_t(algn));
        _sys_ptrs.push_back(ptr);
        std::cout << "[MEM] Alloc: " << ptr << " / " << n << std::endl;
        return ptr;
    }

    void do_deallocate(void* p, size_t n, size_t algn) override {
        if (p == nullptr) return;
        _chunk c;
        c.p = p;
        c.sz = n;
        _storage.push_back(c);
        std::cout << "[MEM] Free: " << p << std::endl;
    }

    bool do_is_equal(const std::pmr::memory_resource& o) const noexcept override {
        return this == &o;
    }

private:
    struct _chunk {
        void* p;
        size_t sz;
    };
    std::vector<_chunk> _storage;
    std::vector<void*> _sys_ptrs;
};

#endif