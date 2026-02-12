#pragma once

#include "TLETC/Core/Types.h"

namespace TLETC
{
class Time
{
public:
    // Call once per frame
    static void Update();
    
    // Variable delta (frame time)
    static float DeltaTime() { return deltaTime_; }
    
    // Total elapsed time
    static float TotalTime() { return totalTime_; }
    
    // Frame count
    static uint64 FrameCount() { return frameCount_; }
    
    // FPS
    static float FPS() { return fps_; }
    
private:
    static float  deltaTime_;
    static float  totalTime_;
    static uint64 frameCount_;
    static float  fps_;
    
    static double lastFrameTime_;
};
    
} // namespace TLETC
