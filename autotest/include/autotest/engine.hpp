#pragma once

#include <functional>
#include <vector>
#include <fstream>

#include "methods.hpp"

namespace AutoTest
{

struct Engine
{
    Engine(FuzzedDataProvider &s);
    void addMethod(Method m);
    void addCondition(std::function<bool()> condition);
    int execute(std::function<bool()> invariant = [] { return true; });
    Method *choseMethod();

private:
    std::size_t choseMethodImpl()
    {
        return state.template ConsumeIntegralInRange<std::size_t>(
            0, applicables.size() - 1);
    }

    template <class Inner>
    int execLoop(Inner &&f)
    {
        Method *choice = choseMethod();
        while (choice)
        {
            if (f(choice))
            {
                return 1;
            }
            choice = choseMethod();
        }
        return 0;
    }

    std::vector<Method> methods;
    std::vector<std::size_t> applicables;
    FuzzedDataProvider &state;
};

} // namespace AutoTest