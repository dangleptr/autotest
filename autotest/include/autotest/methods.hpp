#pragma once

#include <functional>
#include <vector>
#include <memory>

#include "fuzzer/FuzzedDataProvider.h"

namespace AutoTest
{

template <class T, class FunT, class... ArgSpecs>
struct MethodCall
{

    MethodCall(FunT fun, ArgSpecs &&... specs)
        : _fun(std::forward<FunT>(fun)),
          _args{std::forward<ArgSpecs>(specs)...}
    {
    }

    void execute(T &obj, FuzzedDataProvider &state) const
    {
        executeImpl(std::index_sequence_for<ArgSpecs...>{}, state, obj);
    }

private:
    template <std::size_t... Indices>
    void executeImpl(std::index_sequence<Indices...>, FuzzedDataProvider &state, T &obj) const
    {
        (obj.*_fun)(std::get<Indices>(_args)(state)...);
    }

    FunT _fun;
    std::tuple<ArgSpecs...> _args;
};

template <class T, class FunT, class... ArgSpecs>
auto makeCall(FunT &&fun, ArgSpecs &&... specs)
{
    return MethodCall<T, FunT, ArgSpecs...>{
        std::forward<FunT>(fun),
        std::forward<ArgSpecs>(specs)...};
}

struct Method
{
    std::function<void(FuzzedDataProvider &)> call;
    std::function<bool()> condition;
};

template <class T, class FunT, class... ArgSpecs>
Method makeMethod(T &obj, FunT &&fun, ArgSpecs &&... specs)
{
    return {
        [call = makeCall<T>(
             std::forward<FunT>(fun),
             std::forward<ArgSpecs>(specs)...),
         &obj](FuzzedDataProvider &buffer) {
            call.execute(obj, buffer);
        },
        [] { return true; }};
};

template <class T, class FunT, class... ArgSpecs>
Method makeConstMethod(T const &obj, FunT &&fun, ArgSpecs &&... specs)
{
    return {
        [call = makeCall<T>(
             std::forward<FunT>(fun),
             std::forward<ArgSpecs>(specs)...),
         &obj](FuzzedDataProvider &buffer) {
            call.execute(obj, buffer);
        },
        [] { return true; }};
};

} // namespace AutoTest
