#include "block-array.hpp"

#include "autotest/autotest.hpp"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    using AutoTest::Args::integral;

    AutoTest::Builder<BlockArray<int>>{ Data, Size }
        (&BlockArray<int>::emplace_back<int>, integral<int>)
        (&BlockArray<int>::pop_back).If([](auto const& self) { return self.size() > 0; })
        (&BlockArray<int>::size)
        (&BlockArray<int>::back)
        .Const(&BlockArray<int>::back)
    .execute();
    
    return 0;
}
