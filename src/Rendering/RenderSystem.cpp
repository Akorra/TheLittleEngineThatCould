#include "TLETC/Rendering/RenderSystem.h"

#include "TLETC/Resources/ResourceDB.h"
#include "TLETC/Rendering/Renderable.h"
#include "TLETC/Rendering/Camera.h"
#include "TLETC/Scene/Entity.h"

#include <algorithm>

namespace TLETC 
{
static RenderSystem* instance = nullptr;
RenderSystem& RenderSystem::Get() { return *instance; }

bool RenderSystem::Initialize(RenderDevice* device)
{
    device_ = device;
    instance = this;
    return true;
}

void RenderSystem::Shutdown()
{
    renderables_.clear();
    cameras_.clear();
    drawList_.clear();
    device_  = nullptr;
    instance = nullptr;
}

void RenderSystem::Register(Renderable* r)
{
    renderables_.push_back(r);
}

void RenderSystem::Unregister(Renderable* r)
{
    renderables_.erase(
        std::remove(renderables_.begin(), renderables_.end(), r),
        renderables_.end()
    );
}

void RenderSystem::Register(Camera* c) 
{
    cameras_.push_back(c);
}

void RenderSystem::Unregister(Camera* c) 
{
    cameras_.erase(
        std::remove(cameras_.begin(), cameras_.end(), c),
        cameras_.end()
    );
}

void RenderSystem::RenderAll(float aspect)
{
    std::sort(cameras_.begin(), cameras_.end(), 
        [](Camera* a, Camera* b) 
        { 
            return a->GetRenderOrder() < b->GetRenderOrder(); 
        }
    );

    for (Camera* cam : cameras_) 
        RenderCamera(cam, aspect);  
}


void RenderSystem::RenderCamera(Camera* cam, float aspect)
{
    // Clear appropriately
    switch (cam->GetClearFlags()) 
    {
        case Camera::ClearFlags::All:
        case Camera::ClearFlags::ColorOnly:
            device_->Clear(cam->GetClearColor());
            break;
        case Camera::ClearFlags::DepthOnly:
            // depth only clear requires direct call but Clear does both color and depth — extend it later
            device_->Clear(Vec4(0,0,0,1));
            break;
        case Camera::ClearFlags::Nothing:
            break;
    }

    // Build & sort
    drawList_.clear();
    BuildDrawList(cam);
    SortDrawList();
    Execute(cam, aspect);
}

void RenderSystem::BuildDrawList(Camera* cam)
{
    uint32_t camMask = cam->GetCullingMask();

    for (auto* r : renderables_) 
    {
        if (!r->IsEnabled()) continue;
        if (!r->GetMesh().IsValid() || !r->GetMaterial().IsValid()) continue;

        if ((r->GetLayer() & camMask) == 0) continue;

        Material& mat = Resources::Materials.Get(r->GetMaterial());

        uint64_t key = 0;

        key |= (uint64_t(mat.GetRenderQueue()) & 0xFFFFull) << 48;
        key |= (uint64_t(mat.GetProgram().GetID()) & 0xFFFFFFFFull) << 16;
        key |= (uint64_t(reinterpret_cast<uintptr_t>(&mat)) & 0xFFFFull);

        drawList_.push_back({r, key});
    }
}

void RenderSystem::SortDrawList()
{
    std::stable_sort(drawList_.begin(), drawList_.end(),
        [](const DrawItem& a, const DrawItem& b) 
        {
            return a.sortKey < b.sortKey;
        }
    );
}

void RenderSystem::Execute(Camera* cam, float aspect)
{
    Mat4 view       = cam->GetViewMatrix();
    Mat4 projection = cam->GetProjectionMatrix(aspect);

    ShaderHandle   boundProgram;
    MaterialHandle boundMaterial;

    for (const auto& item : drawList_) 
    {
        auto* r = item.r;

        if (!r->GetMesh().IsValid() || !r->GetMaterial().IsValid()) continue;

        auto& mesh = Resources::Meshes.Get(r->GetMesh());
        auto& mat  = Resources::Materials.Get(r->GetMaterial());

        if (mat != boundMaterial) {
            ShaderHandle prog = mat->GetProgram();

            if (prog != boundProgram) {
                device_->UseShader(prog);
                boundProgram = prog;

                device_->SetUniformMat4(prog, "u_view", view);
                device_->SetUniformMat4(prog, "u_projection", projection);
            }

            device_->EnableDepthTest(mat->GetDepthTest());
            device_->EnableCulling(mat->GetCullMode() != Material::CullMode::None);
            device_->EnableBlending(mat->IsTransparent());
            
            Material::PolyMode poly = Material::PolyMode::FrontAndBack;
            Material::RastMode rast = Material::RastMode::Fill;
            mat->GetPolygonMode(poly, rast);
            device_->SetPolygonMode(uint8(poly), uint8(rast));

            boundMaterial = mat;

            int slot = 0;
            for (auto& [name, tex] : mat->GetTextures()) {
                device_->BindTexture(tex->GetHandle(), slot);
                device_->SetUniformInt(prog, name, slot);
                slot++;
            }

            for (auto& [name, v] : mat->GetFloats()) device_->SetUniformFloat(prog, name, v);
            for (auto& [name, v] : mat->GetInts())   device_->SetUniformInt  (prog, name, v);
            for (auto& [name, v] : mat->GetVec2s())  device_->SetUniformVec2 (prog, name, v);
            for (auto& [name, v] : mat->GetVec3s())  device_->SetUniformVec3 (prog, name, v);
            for (auto& [name, v] : mat->GetVec4s())  device_->SetUniformVec4 (prog, name, v);
            for (auto& [name, v] : mat->GetMat3s())  device_->SetUniformMat3 (prog, name, v);
            for (auto& [name, v] : mat->GetMat4s())  device_->SetUniformMat4 (prog, name, v);
        }

        Mat4 model = r->GetEntity()->transform.GetModelMatrix();
        device_->SetUniformMat4(boundProgram, "u_model", model);

        device_->DrawMesh(r->GetMesh(), model);

        device_->SetPolygonMode(0, 0);
    }
}

}