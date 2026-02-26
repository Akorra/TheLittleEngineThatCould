#pragma once

#include "TLETC/ECS/Systems/System.h"
#include "TLETC/Render/RenderDevice.h"

namespace TLETC::ECS
{
class MeshRenderer;
class VertexBuffer;
class IndexBuffer;

class RenderSystem : public System
{
public:
    explicit RenderSystem(Render::RenderDevice* device);
    ~RenderSystem() override;

    const char* Name() const override { return "RenderSystem"; }
    int Priority() const override { return 1000; }

    void Startup(Scene& scene) override;
    void Shutdown(Scene& scene) override;

    void Render(Scene& scene, float alpha) override;

private:
    void UploadMesh(MeshRenderer& renderer);
    void RenderMesh(Scene& scene, Entity entity, MeshRenderer& renderer, const mat4& viewProj);

private:
    Render::RenderDevice* device_;
};

} // namespace TLETC::ECS