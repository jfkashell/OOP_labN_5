#ifndef WRAPPER_MEM_H
#define WRAPPER_MEM_H

#include <vector>
#include <memory_resource>
#include <iostream>

#define LOG_MSG(msg, p) std::cout << msg << p << std::endl

class LabMemoryResource : public std::pmr::memory_resource {
private:
    struct _block_info { 
        void* addr; 
        size_t sz; 
    };

    std::vector<_block_info> _cache;
    std::vector<void*> _global_list;
    int _counter = 0;

public:
    ~LabMemoryResource() {
        for (size_t i = 0; i < _global_list.size(); ++i) {
            ::operator delete(_global_list[i]);
        }
    }

protected:
    void* do_allocate(size_t bytes, size_t alignment) override {
        _counter++;
        for (int k = 0; k < _cache.size(); ++k) {
            if (_cache[k].sz == bytes) {
                void* ret_ptr = _cache[k].addr;
                
                std::swap(_cache[k], _cache.back());
                _cache.pop_back();

                std::cout << "[ALLOC] Reuse: " << ret_ptr << " Size: " << bytes << "\n";
                return ret_ptr;
            }
        }

        void* p = ::operator new(bytes, std::align_val_t(alignment));
        _global_list.push_back(p);
        std::cout << "[ALLOC] System: " << p << " Size: " << bytes << "\n";
        return p;
    }

    void do_deallocate(void* p, size_t bytes, size_t alignment) override {
        _block_info temp;
        temp.addr = p;
        temp.sz = bytes;
        _cache.push_back(temp);
        std::cout << "[DEALLOC] Saved: " << p << "\n";
    }

    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override {
        return this == &other;
    }
};

#endif