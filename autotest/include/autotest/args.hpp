#pragma once

#include "fuzzer/FuzzedDataProvider.h"

namespace AutoTest
{

namespace Args
{

template <class T>
T integral(FuzzedDataProvider &state)
{
    return state.ConsumeIntegral<T>();
}

template <class T>
auto integral(T min, T max)
{
    return [min, max](FuzzedDataProvider &state) {
        return state.ConsumeIntegralInRange(min, max);
    };
}

auto random_string(size_t max_length);

auto fixed_string(size_t length);

template <class T>
T constant(T t)
{
    return [t = std::move(t)](FuzzedDataProvider &state) {
        return t;
    };
}
} // namespace Args

} // namespace AutoTest