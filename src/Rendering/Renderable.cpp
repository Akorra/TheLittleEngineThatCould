#include "TLETC/Rendering/Renderable.h"
#include "TLETC/Rendering/RenderSystem.h"

namespace TLETC 
{
Renderable::Renderable(Entity* owner) : Component(owner), mesh_(), material_() {}

void Renderable::OnStart() { RenderSystem::Get().Register(this); }

void Renderable::OnDestroy() { RenderSystem::Get().Unregister(this); }

} // namespace TLETC
