#include "robin_hood.h"
#include "autotest/autotest.hpp"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    using namespace robin_hood;
    using namespace AutoTest;
    auto key = Args::randomString(20);
    auto keyVal = [](auto& state) {
        return pair<std::string, std::string>(
            Args::randomString(20)(state),
            Args::randomString(20)(state)
        );
    };
    Builder<unordered_flat_map<std::string, std::string>>{ Data, Size }
        AUTOTEST_FUN(insert, keyVal)
        AUTOTEST_FUN(emplace, key, key)
        AUTOTEST_CONST_FUN(count, key)
        AUTOTEST_CONST_FUN(contains, key)
        AUTOTEST_FUN(erase, key)
        AUTOTEST_FUN(reserve, Args::integralRange(1, 10000))
        AUTOTEST_FUN(rehash, Args::integralRange(1, 10000))
        AUTOTEST_CONST_FUN(find, key)
    .execute();
    
    return 0;
}
