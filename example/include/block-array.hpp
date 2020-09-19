#pragma once

#include <vector>
#include <array>

template<class T>
struct block_array {
    block_array() : impl(), back_ptr() {};

    template<class... Args>
    void emplace_back(Args&&... args) {
        if (impl.size() == 0 || back_ptr == impl.back().rbegin()) {
            impl.emplace_back();
            back_ptr = impl.back().rend(); // comment out for ooops...
        }
        --back_ptr;
        ::new(&(*back_ptr)) T(std::forward<Args>(args)...);
    }

    void pop_back() {
        if (back_ptr == impl.back().rend()) {
            impl.pop_back();
            back_ptr = impl.back().rbegin();
        } else {
            back_ptr->~T();
            ++back_ptr;
        }
    }
    
    T& back() {
        return *back_ptr;
    }
    
    const T& back() const {
        return *back_ptr;
    }
    
   std::size_t size() const {
        return block_size * (impl.size() - 1)
            + (impl.back().rend() - back_ptr);
    }
private:
    static constexpr std::size_t block_size = 16;
    using block = std::array<T, block_size>;
    std::vector<block> impl;
    typename block::reverse_iterator back_ptr;
};