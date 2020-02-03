#pragma once

#include "engine.hpp"
#include "methods.hpp"

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
    Builder &operator()(RetT(T::*fun)(ParamT...), ArgSpecs... specs)
    {
        engine.addMethod(
            makeMethod(
                instance,
                fun,
                specs...));
        return *this;
    }

    template <class FunT, class... ArgSpecs>
    Builder &operator()(FunT fun, ArgSpecs &&... specs)
    {
        engine.addMethod(
            makeMethod(
                instance,
                fun,
                specs...));
        return *this;
    }

    template <class RetT, class... ParamT, class... ArgSpecs>
    Builder &Const(RetT(T::*fun)(ParamT...) const , ArgSpecs &&... specs)
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