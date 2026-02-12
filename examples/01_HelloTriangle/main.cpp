#include "TLETC/Core/Application.h"
#include "TLETC/Core/Log.h"

class TestApp : public TLETC::Application
{
public:
    TestApp() : Application("Milestone 1 Test") {}
    
    void OnStartup() override
    {
        TLETC_INFO("Game started!");
    }
    
    void OnUpdate(float dt) override
    {
        // Test input
        if (input_->IsKeyPressed(TLETC::KeyCode::Escape))
        {
            TLETC_INFO("Escape pressed - exiting");
            RequestShutdown(); // This will end the loop
        }
        
        if (input_->IsKeyDown(TLETC::KeyCode::W))
            TLETC_TRACE("W held");
        
        if (input_->IsMouseButtonPressed(TLETC::MouseButton::Left))
        {
            auto pos = input_->GetMousePosition();
            TLETC_INFO("Mouse clicked at: ", pos.x, ", ", pos.y);
        }
    }
    
    void OnRender() override
    {
    }
    
    void OnShutdown() override
    {
        TLETC_INFO("Game shutdown");
    }
};

int main()
{
    TestApp app;
    app.Run();
    return 0;
}