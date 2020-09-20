#pragma once

#include <functional>
#include <vector>
#include <fstream>

#include "methods.hpp"

namespace AutoTest
{

template<class T>
using Statement = std::function<bool(const T&)>;

template <class T>
struct Engine
{
    template<class ConstructorT = T(*)(FuzzedDataProvider&)>
    Engine(const uint8_t* data, size_t size, ConstructorT&& constr)
        : state{data, size}, instance{constr(state)} {}

    void addMethodCall(std::unique_ptr<Method<T>> functor, Statement<T>&& precondition) {
        methods.push_back(std::move(functor));

        if (precondition) {
            preconditions.push_back(std::move(precondition));
            if (preconditions.size() != methods.size()) {
                std::swap(methods[preconditions.size() - 1], methods.back());
            }
        }

        for (std::size_t i = 0; i < methods.size(); ++i) {
       //     std::cout << i << ":" << methods[i]->_name << " " << (i < preconditions.size() ? "has condition\n" : "no condition\n");
        }
    }

    template<class InvariantT>
    void execute(InvariantT invariant) {
        invariant(instance);
        Method<T>* choice = {};
        while ((choice = chooseMethod())) {
            choice->execute(instance, state);
            invariant(instance);
        }
    }

private:
    Method<T>* chooseMethod();

    FuzzedDataProvider state;
    std::vector<Statement<T>> preconditions;
    std::vector<std::unique_ptr<Method<T>>> methods; // first `preconditions.size()` member has preconditions
    std::vector<std::size_t> applicable_precs;
    T instance;
};

template<class T>
Method<T>* Engine<T>::chooseMethod() {
    static bool noChoiceWarning{false};
    if (state.remaining_bytes() == 0) {
    //    std::cerr << "done\n";
        return nullptr;
    }

    applicable_precs.clear();
    std::string s;
    for (std::size_t i = 0; i < preconditions.size(); ++i) {
        if (preconditions[i](instance)) {
            applicable_precs.push_back(i);
        }
    }
    std::size_t all_applicable = methods.size() - preconditions.size() + applicable_precs.size();

    if (all_applicable < 1) {
        if (!noChoiceWarning) {
            std::cerr << "[AutoTest]: No methods are applicable. Is this intended?\n";
            noChoiceWarning = true;
        }
        return nullptr;
    }

    std::size_t choice = state.template ConsumeIntegralInRange<std::size_t>(0, all_applicable - 1);
    if (choice < applicable_precs.size()) {
        return methods[applicable_precs[choice]].get();
    } else {
        return methods[choice - applicable_precs.size() + preconditions.size()].get();
    }
}

} // namespace AutoTest