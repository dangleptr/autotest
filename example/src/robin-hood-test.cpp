#include "robin_hood.h"
#include "autotest/autotest.hpp"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    auto key = AutoTest::Args::randomString(20);
    auto key_val = [](auto& state) {
        return robin_hood::pair<std::string, std::string>(
            AutoTest::Args::randomString(20)(state),
            AutoTest::Args::randomString(20)(state)
        );
    };
    using hash_map = robin_hood::unordered_flat_map<std::string, std::string>;
    AutoTest::Interface<hash_map>(data, size)
        .AUTOTEST_FUN(insert, key_val)
        .AUTOTEST_FUN(emplace, key, key)
        .AUTOTEST_CONST_FUN(count, key)
        .AUTOTEST_CONST_FUN(contains, key)
        .AUTOTEST_FUN(erase, key)
        .AUTOTEST_FUN(reserve, AutoTest::Args::integralRange(1, 10000))
        .AUTOTEST_FUN(rehash, AutoTest::Args::integralRange(1, 10000))
        .AUTOTEST_CONST_FUN(find, key)
    .execute();
    
    return 0;
}
