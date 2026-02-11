#include "TLETC/Resources/TextureManager.h"

#include "TLETC/Rendering/RenderDevice.h"

#include <stb_image.h>
#include <iostream>


namespace TLETC {

TextureHandle TextureManager::LoadFromFile(const std::string& path)
{
    int w, h, ch;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path.c_str(), &w, &h, &ch, 0);

    if (!data) {
        std::cerr << "Failed to load image: " << path << std::endl;
        return {};
    }

    TextureFormat fmt;
    switch (ch) {
        case 1: fmt = TextureFormat::R; break;
        case 2: fmt = TextureFormat::RG; break;
        case 3: fmt = TextureFormat::RGB; break;
        case 4: fmt = TextureFormat::RGBA; break;
        default:
            stbi_image_free(data);
            std::cerr << "Unsupported channel count: " << ch << std::endl;
            return {};
    }

    Texture tex;
    tex.filepath_ = path;
    tex.width_ = w;
    tex.height_ = h;
    tex.format_ = fmt;
    tex.pixelData_.assign(data, data + (w * h * ch));

    stbi_image_free(data);

    return Create(std::move(tex));
}

void TextureManager::UploadToGPU(RenderDevice* device, TextureHandle h)
{
    // GPU upload handled by RenderDevice
    auto& tex = Get(h);
    device->CreateTexture(h);
}

void TextureManager::ProcessDestroyQueueGPU(RenderDevice* device)
{
    for (auto& h : gpuToDestroy_)
        device->DestroyTexture(h);
    gpuToDestroy_.clear();
}

} // namespace TLETC
