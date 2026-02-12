#pragma once

#include <chrono>

namespace TLETC
{
class Time
{
public:
    static void  Update();
    static float DeltaTime();
private:
    using  clock = std::chrono::steady_clock;
    
    static clock::time_point lastTimeStamp_;
    static clock::time_point currTimeStamp_;
};
    
} // namespace TLETC
