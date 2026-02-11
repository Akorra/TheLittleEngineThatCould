#pragma once

#include "TLETC/Resources/ResourceManager.h"
#include "TLETC/Resources/Texture.h"

namespace TLETC 
{
class TextureManager : public ResourceManager<Texture, TextureHandle> 
{
public:
    // Load from file (CPU)
    TextureHandle LoadFromFile(const std::string& path);

    // Upload to GPU
    void UploadToGPU(class RenderDevice* device, TextureHandle h);

    // Cleanup GPU resources for destroyed textures
    void ProcessDestroyQueueGPU(class RenderDevice* device);

private:
    std::vector<TextureHandle> gpuToDestroy_;
};
} // namespace TLETC