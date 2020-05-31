#include "autotest/autotest.hpp"
#include "block-array.hpp"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    using AutoTest::Args::integral;
    auto not_empty = [](auto const& self) { return self.size() > 0; };

    AutoTest::Builder<BlockArray<int>>{ Data, Size }
        AUTOTEST_FUN(emplace_back, integral<int>)
        AUTOTEST_FUN(pop_back).If(not_empty)
        AUTOTEST_FUN(back).If(not_empty)
        AUTOTEST_CONST_FUN(back).If(not_empty)
        AUTOTEST_CONST_FUN(size)
    .execute();
    return 0;
}
