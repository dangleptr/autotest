#pragma once

#include <vector>
#include <array>

template<class T>
struct BlockArray {
    BlockArray() : impl(), backPtr() {};

    template<class... Args>
    void emplace_back(Args&&... args) {
        if (impl.size() == 0 || backPtr == impl.back().rbegin()) {
            impl.emplace_back();
            backPtr = impl.back().rend(); // comment out for ooops...
        }
        --backPtr;
        ::new(&(*backPtr)) T(std::forward<Args>(args)...);
    }

    void pop_back() {
        if (backPtr == impl.back().rend()) {
            impl.pop_back();
            backPtr = impl.back().rbegin();
        } else {
            backPtr->~T();
            ++backPtr;
        }
    }
    
    T& back() {
        return *backPtr;
    }
    
    const T& back() const {
        return *backPtr;
    }
    
   std::size_t size() const {
        return blockSize * (impl.size() - 1)
            + (impl.back().rend() - backPtr);
    }
private:
    static constexpr std::size_t blockSize = 16;
    using Block = std::array<T, blockSize>;
    std::vector<Block> impl;
    typename Block::reverse_iterator backPtr;
};