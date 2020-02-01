#include "autotest/args.hpp"

namespace AutoTest
{

namespace Args
{

auto random_string(size_t max_length)
{
    return [max_length](FuzzedDataProvider &state) {
        return state.ConsumeRandomLengthString(max_length);
    };
}

auto fixed_string(size_t length)
{
    return [length](FuzzedDataProvider &state) {
        return state.ConsumeBytesAsString(length);
    };
}
} // namespace Args

} // namespace AutoTest