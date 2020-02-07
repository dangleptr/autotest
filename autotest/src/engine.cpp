#include <vector>
#include <iostream>

#include <fuzzer/FuzzedDataProvider.h>

#include "autotest/engine.hpp"

namespace AutoTest
{

Engine::Engine(FuzzedDataProvider &s)
    : state{s} {}

void Engine::addMethod(Method m)
{
    methods.push_back(std::move(m));
}

void Engine::addCondition(std::function<bool()> condition)
{
    methods.back().condition = std::move(condition);
}

int Engine::execute(std::function<bool()> invariant)
{
    applicables.reserve(methods.size());
    if (!invariant())
    {
        std::cerr << "[FuzzTest]: The class invariant does not hold.\n";
        return 1;
    }
    return execLoop([this, &invariant](Method *choice) {
        choice->call(state);
        if (!invariant())
        {
            std::cerr << "[FuzzTest]: The class invariant does not hold.\n";
            return 1;
        }
        return 0;
    });
}

Method *Engine::choseMethod()
{
    static bool noChoiceWarning{false};

    applicables.clear();
    for (std::size_t i = 0; i < methods.size(); ++i)
    {
        if (methods[i].condition())
        {
            applicables.push_back(i);
        }
    }

    if (applicables.size() && state.remaining_bytes() > 0)
    {
        const std::size_t choice = choseMethodImpl();
        return &methods[applicables[choice]];
    }
    else
    {
        if (!noChoiceWarning)
        {
            std::cerr << "[AutoTest]: No methods are applicable. Is this intended?\n";
            noChoiceWarning = true;
        }
        return nullptr;
    }
}

} // namespace AutoTest