#pragma once

#include <functional>
#include <vector>
#include <memory>
#include <type_traits>

#include <fuzzer/FuzzedDataProvider.h>

namespace AutoTest
{

struct NoOp {};

template<class T>
struct Method {
    virtual void execute(T& obj, FuzzedDataProvider &state) = 0;
    const char* _name;
};

template <class T, class FunT, class HookT, class... ArgSpecs>
struct MethodImpl : Method<T> {

    MethodImpl(const char* name,  FunT&& fun, HookT&& hook,std::tuple<ArgSpecs...>&& args)
        : _hook{std::move(hook)}, _fun{std::move(fun)}, _args{std::move(args)} {
        this->_name = name;
    }

    virtual void execute(T& obj, FuzzedDataProvider &state) override {
        executeImpl(std::index_sequence_for<ArgSpecs...>{}, state, obj);
    }

private:
    template <std::size_t... Indices>
    void executeImpl(std::index_sequence<Indices...>, FuzzedDataProvider &state, T &obj) {
        executeProxy(obj, std::get<Indices>(_args)(state)...);
    }

    template<class... Args>
    void executeProxy(T& obj, Args&&... args) {
        if constexpr (std::is_same_v<NoOp, HookT>) {
            _fun(obj, std::forward<Args>(args)...); 
        } else if constexpr (!std::is_invocable_v<decltype(_hook(obj, this->_name, args...)), const T&>) {
            _hook(obj, this->_name, args...);
            _fun(obj, std::forward<Args>(args)...); 
        } else {
            auto postCond = _hook(obj, this->_name, args...);
            _fun(obj, std::forward<Args>(args)...);
            assert(postCond(obj) && "post condition not satisfied");
        }
    }

    HookT _hook;
    FunT _fun;
    std::tuple<ArgSpecs...> _args;
};

} // namespace AutoTest
