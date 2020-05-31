#pragma once

#include <sstream>

#include <fuzzer/FuzzedDataProvider.h>

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
auto integralRange(T min, T max)
{
    return [min, max](FuzzedDataProvider &state) {
        return state.ConsumeIntegralInRange(min, max);
    };
}

auto randomString(size_t max_length)
{
    return [max_length](FuzzedDataProvider &state) {
        return state.ConsumeRandomLengthString(max_length);
    };
}

auto fixedString(size_t length)
{
    return [length](FuzzedDataProvider &state) {
        return state.ConsumeBytesAsString(length);
    };
}

template <class T>
T constant(T t)
{
    return [t = std::move(t)](FuzzedDataProvider &state) {
        return t;
    };
}

template<class ContT, class FunT>
ContT container(std::size_t maxSize, FunT elementGen) {
    return [elementGen = std::move(elementGen), maxSize](auto& state) {
        const auto size = integralRange<std::size_t>(0, maxSize)(state);
        ContT c;
        for (std::size_t i = 0; i < size; ++i) {
            c.insert(c.end(), elementGen(state));
        }
        return c;
    };
}

} // namespace Args

} // namespace AutoTest