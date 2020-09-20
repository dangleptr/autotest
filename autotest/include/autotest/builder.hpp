#pragma once

#include <iostream>
#include <cassert>

#include "engine.hpp"
#include "methods.hpp"

#define AUTOTEST_FUN(FUN_NAME, ...) addFunctor(#FUN_NAME, [](auto& obj, auto&&... a) { return obj.FUN_NAME(a...); }, ## __VA_ARGS__)
#define AUTOTEST_CONST_FUN(FUN_NAME, ...) addFunctor(#FUN_NAME, [](const auto& obj, auto&&... a) { return obj.FUN_NAME(a...); }, ## __VA_ARGS__)

namespace AutoTest
{

// chained state machine - required for retaining more type information
// for minimizing virtuality in the Method implementations

// nothing is pending
// <START> = Empty
// Empty -(addFunctor)-> Fun        | start new method reg
// Empty -(execute)-> <END>         | start execution

// Has a method pending
// Fun -(addFunctor)-> Fun          | finalize and start new method reg
// Fun -(If)-> Fun                  | add precond to pending method
// Fun -(addHook)-> FunWithHook     | add hook to pending method
// Fun -(execute)-> <END>           | finalize and start execution

// Has a method and a precall hook pending
// FunWithHook -(addFunctor)-> Fun  | finalize and start new method reg
// FunWithHook -(execute)-> <END>   | finalize and start execution

template<class T>
struct BuilderEmpty;

template<class T, class FunT, class... ArgSpecs>
struct BuilderFun;

template<class T, class FunT, class HookT, class... ArgSpecs>
struct BuilderFunWithHook;

template<class T, class ConstructorT = T(*)(FuzzedDataProvider&)>
BuilderEmpty<T> Interface(
    const uint8_t* data, size_t size,
    ConstructorT&& constr = +[](FuzzedDataProvider&) { return T{}; }
) {
    return BuilderEmpty<T>{
        std::make_unique<Engine<T>>(data, size, std::move(constr))
    };
}

// inheritance is used to remove code duplication - it's not exactly
// a best practice but...

template<class T, class ActualState>
struct BuilderStateBase {
    BuilderStateBase(std::unique_ptr<Engine<T>> e) : engine{std::move(e)} {}

    template<class InvariantT = bool(*)(const T&)>
    void execute(InvariantT invariant = [] (const T&) { return true; }) {
        static_cast<ActualState*>(this)->finalize();
        engine->execute(std::move(invariant));
    }

    template<class FunT, class... ArgSpecs>
    auto addFunctor(const char* name, FunT fun, ArgSpecs... args) {
        static_cast<ActualState*>(this)->finalize();
        return BuilderFun<T, FunT, ArgSpecs...>{name, std::move(engine), std::move(fun), std::move(args)...};
    }

    std::unique_ptr<Engine<T>> engine;
};

template <class T>
struct BuilderEmpty : BuilderStateBase<T, BuilderEmpty<T>>
{
    BuilderEmpty(std::unique_ptr<Engine<T>> e) 
        : BuilderStateBase<T, BuilderEmpty<T>>{std::move(e)} {}

    void finalize() {

    } // nothing to finalize
};

template<class T, class FunT, class... ArgSpecs>
struct BuilderFun : BuilderStateBase<T, BuilderFun<T, FunT, ArgSpecs...>> {
    BuilderFun(const char* n, std::unique_ptr<Engine<T>> e, FunT&& f, ArgSpecs&&... a)
        : BuilderStateBase<T, BuilderFun<T, FunT, ArgSpecs...>>{std::move(e)},
          name{n},
          fun{std::move(f)},
          args{std::move(a)...}
    {}

    BuilderFun& If(Statement<T> s) {
        assert(!precond);
        precond = std::move(s);
        return *this;
    }

    template<class HookT>
    auto addHook(HookT hook) {
        return BuilderFunWithHook<T, FunT, HookT, ArgSpecs...>{*this, std::move(hook)};
    }

    void finalize() {
        this->engine->addMethodCall(
            std::make_unique<MethodImpl<T, FunT, NoOp, ArgSpecs...>>(
                name,
                std::move(fun),
                NoOp{},
                std::move(args)
            ),
            std::move(precond)
        );
    }

    const char* name;
    FunT fun;
    Statement<T> precond;
    std::tuple<ArgSpecs...> args;
};

template<class T, class FunT, class HookT, class... ArgSpecs>
struct BuilderFunWithHook : BuilderStateBase<T, BuilderFunWithHook<T, FunT, HookT, ArgSpecs...>> {
    BuilderFunWithHook(BuilderFun<T, FunT, ArgSpecs...>& b, HookT&& h)
        : BuilderStateBase<T, BuilderFunWithHook<T, FunT, HookT, ArgSpecs...>>{std::move(b.engine)},
          builderFun{b}, hook{std::move(h)}
    {}

    void finalize() {
        this->engine->addMethodCall(
            std::make_unique<MethodImpl<T, FunT, HookT, ArgSpecs...>>(
                builderFun._name,
                std::move(builderFun.fun),
                std::move(hook),
                std::move(builderFun.args)
            ),
            std::move(builderFun.precond)
        );
    }

    HookT hook;
    BuilderFun<T, FunT, ArgSpecs...>& builderFun;
};

} // namespace AutoTest