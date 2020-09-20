#include "autotest/autotest.hpp"
#include "block-array.hpp"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    auto not_empty = [](auto const& self) { return self.size() > 0; };
    AutoTest::Interface<block_array<int>>(data, size)
        .AUTOTEST_FUN(emplace_back, AutoTest::Args::integral<int>)
        .AUTOTEST_FUN(pop_back).If(not_empty)
        .AUTOTEST_FUN(back).If(not_empty)
        .AUTOTEST_CONST_FUN(back).If(not_empty)
        .AUTOTEST_CONST_FUN(size)
    .execute();
    return 0;
}
