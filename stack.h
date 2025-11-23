#pragma once
#include <memory_resource>
#include <stdexcept>
#include <utility>
#include <iterator>

template <typename T>
class Stack {
private:
    struct Node {
        T value;
        Node* next;
        Node(const T& val, Node* nxt) : value(val), next(nxt) {}
    };

    Node* head_;
    size_t size_;
    std::pmr::polymorphic_allocator<Node> alloc_;

public:
    explicit Stack(std::pmr::memory_resource* resource = std::pmr::get_default_resource())
        : head_(nullptr), size_(0), alloc_(resource) {}

    ~Stack() {
        while (head_) {
            Node* temp = head_;
            head_ = head_->next;
            alloc_.destroy(temp);
            alloc_.deallocate(temp, 1);
        }
    }

    Stack(const Stack& other) = delete;
    Stack& operator=(const Stack& other) = delete;

    Stack(Stack&& other) noexcept
        : head_(other.head_), size_(other.size_), alloc_(other.alloc_) {
        other.head_ = nullptr;
        other.size_ = 0;
    }

    Stack& operator=(Stack&& other) noexcept {
        if (this != &other) {
            while (head_) {
                Node* temp = head_;
                head_ = head_->next;
                alloc_.destroy(temp);
                alloc_.deallocate(temp, 1);
            }
            
            head_ = other.head_;
            size_ = other.size_;
            alloc_ = other.alloc_;
            
            other.head_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }

    void push(const T& value) {
        Node* new_node = alloc_.allocate(1);
        alloc_.construct(new_node, value, head_);
        head_ = new_node;
        ++size_;
    }

    void push(T&& value) {
        Node* new_node = alloc_.allocate(1);
        alloc_.construct(new_node, std::forward<T>(value), head_);
        head_ = new_node;
        ++size_;
    }

    void pop() {
        if (empty()) {
            throw std::underflow_error("Stack underflow");
        }
        Node* temp = head_;
        head_ = head_->next;
        alloc_.destroy(temp);
        alloc_.deallocate(temp, 1);
        --size_;
    }

    T& top() {
        if (empty()) {
            throw std::underflow_error("Stack is empty");
        }
        return head_->value;
    }

    const T& top() const {
        if (empty()) {
            throw std::underflow_error("Stack is empty");
        }
        return head_->value;
    }

    bool empty() const noexcept {
        return size_ == 0;
    }

    size_t size() const noexcept {
        return size_;
    }

    class Iterator {
    private:
        Node* current_;
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        explicit Iterator(Node* node) : current_(node) {}

        reference operator*() const {
            return current_->value;
        }

        pointer operator->() const {
            return &(current_->value);
        }

        Iterator& operator++() {
            if (current_) {
                current_ = current_->next;
            }
            return *this;
        }

        Iterator operator++(int) {
            Iterator temp = *this;
            ++(*this);
            return temp;
        }

        bool operator==(const Iterator& other) const {
            return current_ == other.current_;
        }

        bool operator!=(const Iterator& other) const {
            return !(*this == other);
        }
    };

    Iterator begin() {
        return Iterator(head_);
    }

    Iterator end() {
        return Iterator(nullptr);
    }
};