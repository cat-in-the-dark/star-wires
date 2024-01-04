#pragma once

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

class LApi {
    sol::state lua;
  public:
    LApi();

    void Run();
    void Draw();
    void Update();
};