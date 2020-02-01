#include "block-array.hpp"

#include "autotest/autotest.hpp"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    using Deque = BlockArray<int>;
    using AutoTest::Args::integral;

    AutoTest::Builder<Deque>{ Data, Size }
        (&Deque::emplace_back<int>, integral<int>)
        (&Deque::pop_back).If([](auto const& self) { return self.size() > 0; })
        (&Deque::size)
    .execute();
    
    return 0;
}
