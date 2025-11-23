#ifndef MEM_H
#define MEM_H

#include <vector>
#include <memory_resource>
#include <iostream>

class MemRes : public std::pmr::memory_resource {
    struct Chunk { void* p; size_t n; };
    std::vector<Chunk> pool;
    std::vector<void*> all_blocks;

public:
    ~MemRes() {
        for (void* p : all_blocks) {
            ::operator delete(p);
        }
    }

protected:
    void* do_allocate(size_t bytes, size_t align) override {
        for (size_t i = 0; i < pool.size(); ++i) {
            if (pool[i].n == bytes) {
                void* p = pool[i].p;
                pool.erase(pool.begin() + i);
                std::cout << "  [*] reuse: " << p << " (" << bytes << "b)\n";
                return p;
            }
        }

        void* p = ::operator new(bytes, std::align_val_t(align));
        all_blocks.push_back(p);
        std::cout << "  [+] system alloc: " << p << " (" << bytes << "b)\n";
        return p;
    }

    void do_deallocate(void* p, size_t bytes, size_t) override {
        pool.push_back({p, bytes});
        std::cout << "  [-] to pool: " << p << "\n";
    }

    bool do_is_equal(const std::pmr::memory_resource& o) const noexcept override {
        return this == &o;
    }
};

#endif