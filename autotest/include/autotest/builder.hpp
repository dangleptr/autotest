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

    template <class FunT, class... ArgSpecs>
    Builder &operator()(FunT &&fun, ArgSpecs &&... specs)
    {
        engine.addMethod(
            makeMethod(
                instance,
                std::forward<FunT>(fun),
                std::forward<ArgSpecs>(specs)...));
        return *this;
    }

    template <class FunT, class... ArgSpecs>
    Builder &Const(FunT &&fun, ArgSpecs &&... specs)
    {
        engine.addMethod(
            makeConstMethod(
                instance,
                std::forward<FunT>(fun),
                std::forward<ArgSpecs>(specs)...));
        return *this;
    }

    template <class Pred>
    Builder &If(Pred &&pred)
    {
        engine.addCondition(
            [obj = instance, f = std::move(pred)] {
                return f(obj);
            });
        return *this;
    }

    template <class Invariant>
    int execute(Invariant &&invariant)
    {
        return engine.execute(
            [this, f = std::forward<Invariant>(invariant)] {
                return f(static_cast<const T&>(instance));
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