#pragma once

#include <iostream>

#include "engine.hpp"
#include "methods.hpp"

#define AUTOTEST_FUN(FUN_NAME, ...) addFunctor([](auto& obj, auto&&... a) { return obj.FUN_NAME(a...); }, ## __VA_ARGS__)
#define AUTOTEST_CONST_FUN(FUN_NAME, ...) addFunctor([](const auto& obj, auto&&... a) { return obj.FUN_NAME(a...); }, ## __VA_ARGS__)

namespace AutoTest
{

template <class T>
struct Builder
{
    template <class Constructor = T (*)(FuzzedDataProvider &)>
    Builder(
        const uint8_t *Data, std::size_t Size,
        Constructor constructor = +[](FuzzedDataProvider &) { return T{}; })
        : state{Data, Size},
          instance{constructor(state)},
          engine{state}
    {
    }

    template <class RetT, class... ParamT, class... ArgSpecs>
    Builder & Const(RetT(T::*fun)(ParamT...) const, ArgSpecs... specs)
    {
        std::cerr << "fun" << (bool)fun << "\n";
        engine.addMethod(
            makeMethod(
                instance,
                [fun](const auto& obj, auto&&... a) {
        std::cerr << "funn" <<  (void*)(&obj) << "\n";
                    return (obj.*fun)(a...);
                    },
                specs...));
        return *this;
    }

    template <class RetT, class... ParamT, class... ArgSpecs>
    Builder &operator()(RetT(T::*fun)(ParamT...), ArgSpecs... specs)
    {
        engine.addMethod(
            makeMethod(
                instance,
                [fun](auto& obj, auto&&... a) { return (obj.*fun)(a...); },
                specs...));
        return *this;
    }

    template <class FunT, class... ArgSpecs>
    Builder &addFunctor(FunT fun, ArgSpecs &&... specs)
    {
        engine.addMethod(
            makeMethod(
                instance,
                fun,
                specs...));
        return *this;
    }

    template <class Pred>
    Builder &If(Pred &&pred)
    {
        engine.addCondition(
            [&obj = instance, f = std::move(pred)] {
                return f(obj);
            });
        return *this;
    }

    template <class Invariant>
    int execute(Invariant invariant)
    {
        return engine.execute(
            [this, invariant] {
                return invariant(static_cast<const T&>(instance));
            });
    }

    int execute()
    {
        return engine.execute();
    }

private:
    FuzzedDataProvider state;
    T instance;
    Engine engine;
};

} // namespace AutoTest