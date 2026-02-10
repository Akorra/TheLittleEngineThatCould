#pragma once

#include "TLETC/Core/Types.h"
#include "TLETC/Resources/ResourceManager.h"
#include "TLETC/Resources/Mesh.h"
#include "TLETC/Resources/Material.h"
#include "TLETC/Resources/Texture.h

// Resources
namespace Resources
{
extern ResourceManager<Mesh, MeshHandle>         Meshes;
extern ResourceManager<Material, MaterialHandle> Materials;
extern ResourceManager<Texture, TextureHandle>   Textures;

void ProcessDestroyQueues() 
{  
    Meshes.ProcessDestroyQueue();
    Materials.ProcessDestroyQueue();
    Textures.ProcessDestroyQueue();
}

void Clear()
{
    Meshes.Clear();
    Materials.Clear();
    Textures.Clear();
}

}
