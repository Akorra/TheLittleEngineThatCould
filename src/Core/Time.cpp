#include "TLETC/Core/Time.h"

namespace TLETC
{
    
Time::clock::time_point Time::lastTimeStamp_ = Time::clock::now();
Time::clock::time_point Time::currTimeStamp_ = Time::clock::now();

void  Time::Update()
{
    lastTimeStamp_ = currTimeStamp_;
    currTimeStamp_ = clock::now();
}

float Time::DeltaTime()
{
    std::chrono::duration<float, std::milli> delta = currTimeStamp_ - lastTimeStamp_;
    return delta.count(); // seconds
}

} // namespace TLETC
