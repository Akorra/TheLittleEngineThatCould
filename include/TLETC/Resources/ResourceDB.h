#pragma once

#include "TLETC/Core/Types.h"
#include "TLETC/Resources/ResourceManager.h"
#include "TLETC/Resources/Mesh.h"
#include "TLETC/Resources/Material.h"
#include "TLETC/Resources/TextureManager.h"

using MeshManager     = ResourceManager<Mesh, MeshHandle>;
using MaterialManager = ResourceManager<Material, MaterialHandle>;

// Resources
namespace Resources
{
extern MeshManager     Meshes;
extern MaterialManager Materials;
extern TextureManager  Textures;

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
