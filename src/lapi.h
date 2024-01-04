#pragma once

#include <sol/sol.hpp>

class LApi {
    sol::state lua;
  public:
    LApi();

    void Run();
    void Draw();
    void Update();
};