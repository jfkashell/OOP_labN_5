#pragma once
#include <memory_resource>
#include <vector>
#include <cstdlib>
#include <stdexcept>

class VectorMemoryResource : public std::pmr::memory_resource {
private:
    struct Block {
        void* ptr;
        size_t size;
        bool is_free;
    };
    
    std::vector<Block> blocks_;

    bool is_aligned(void* ptr, size_t alignment) const {
        return reinterpret_cast<uintptr_t>(ptr) % alignment == 0;
    }

    Block* find_suitable_block(size_t bytes, size_t alignment) {
        for (auto& block : blocks_) {
            if (block.is_free && block.size >= bytes && is_aligned(block.ptr, alignment)) {
                return &block;
            }
        }
        return nullptr;
    }

    void* do_allocate(size_t bytes, size_t alignment) override {
        if (Block* free_block = find_suitable_block(bytes, alignment)) {
            free_block->is_free = false;
            return free_block->ptr;
        }
        
        void* ptr = std::aligned_alloc(alignment, bytes);
        if (!ptr) {
            throw std::bad_alloc();
        }
        
        blocks_.push_back({ptr, bytes, false});
        return ptr;
    }

    void do_deallocate(void* p, size_t bytes, size_t alignment) override {
        for (auto& block : blocks_) {
            if (block.ptr == p) {
                block.is_free = true;
                return;
            }
        }
    }

    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override {
        return this == &other;
    }

public:
    ~VectorMemoryResource() {
        for (auto& block : blocks_) {
            std::free(block.ptr);
        }
    }
};