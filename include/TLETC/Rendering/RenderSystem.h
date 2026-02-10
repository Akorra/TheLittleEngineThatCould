#pragma once
#include "TLETC/Core/Math.h"
#include "TLETC/Rendering/RenderDevice.h"

namespace TLETC 
{
// forward declarations
class Renderable;
class Camera;

class RenderSystem
{
public:
    static RenderSystem& Get();

    bool Initialize(RenderDevice* device);
    void Shutdown();

    // registration
    void Register(Renderable*);
    void Register(Camera*);
    void Unregister(Renderable*);
    void Unregister(Camera*);

    // per-frame entry from Application
    void RenderAll(float aspect);

private:
    struct DrawItem 
    {
        Renderable* r;
        uint64      sortKey;
    };

    void RenderCamera(Camera*, float aspect);
    void BuildDrawList(Camera*);
    void SortDrawList();
    void Execute(Camera*, float aspect);

private:
    RenderDevice* device_ = nullptr;

    std::vector<Renderable*> renderables_;
    std::vector<Camera*>     cameras_;
    std::vector<DrawItem>    drawList_;
};

} // namespace TLETC