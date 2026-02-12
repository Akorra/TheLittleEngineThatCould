#include "TLETC/Platform/Time.h"

#include <GLFW/glfw3.h>

namespace TLETC
{

float  Time::deltaTime_     = 0.0f;
float  Time::totalTime_     = 0.0f;
uint64 Time::frameCount_    = 0;
float  Time::fps_           = 0.0f;
double Time::lastFrameTime_ = 0.0;

void  Time::Update()
{
    double currentTime = glfwGetTime();
    
    if (frameCount_ == 0)
    {
        lastFrameTime_ = currentTime;
        frameCount_ = 1;
        return;
    }

    deltaTime_ = static_cast<float>(currentTime - lastFrameTime_);
    totalTime_ += deltaTime_;
    frameCount_++;

    // FPS calculation (smoothed)
    static const int FPS_SAMPLE_COUNT = 60;
    static float fpsSamples[FPS_SAMPLE_COUNT] = {0};
    static int fpsIndex = 0;

    fpsSamples[fpsIndex] = 1.0f / deltaTime_;
    fpsIndex = (fpsIndex + 1) % FPS_SAMPLE_COUNT;

    float sum = 0;
    for (int i = 0; i < FPS_SAMPLE_COUNT; ++i)
        sum += fpsSamples[i];
    fps_ = sum / FPS_SAMPLE_COUNT;
    
    lastFrameTime_ = currentTime;
}

} // namespace TLETC
