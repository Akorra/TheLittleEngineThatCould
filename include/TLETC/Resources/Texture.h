#pragma once

#include "TLETC/Core/Types.h"
#include "TLETC/Rendering/Handle.h"

#include <string>

namespace TLETC 
{

// Forward declaration
class RenderDevice;

enum class TextureFormat 
{
    RGB,
    RGBA,
    R,              // Single channel (grayscale)
    RG,             // Two channels
    Depth,          // Depth texture
    DepthStencil
};

enum class TextureFilter 
{
    Nearest,
    Linear,
    NearestMipmapNearest,
    LinearMipmapNearest,
    NearestMipmapLinear,
    LinearMipmapLinear
};

enum class TextureWrap 
{
    Repeat,
    MirroredRepeat,
    ClampToEdge,
    ClampToBorder
};

/**
 * Texture - Manages GPU texture resources
 * 
 * User is responsible for calling Destroy() before application shutdown.
 * 
 * Usage:
 *   Texture texture;
 *   texture.LoadFromFile("brick.png", renderer);
 *   material->SetTexture("u_diffuseMap", &texture);
 */
class Texture 
{
public:
    Texture()  = default;
    ~Texture() = default;
    
    // File path (optional)
    std::string filepath_;

    // Pixel data (only needed for CPU uploads or streaming)
    std::vector<uint8_t> pixelData_;

    int width_ = 0;
    int height_ = 0;
    TextureFormat format_ = TextureFormat::RGBA;
    
    TextureFilter minFilter_ = TextureFilter::LinearMipmapLinear;
    TextureFilter magFilter_ = TextureFilter::Linear;

    TextureWrap wrapS_ = TextureWrap::Repeat;
    TextureWrap wrapT_ = TextureWrap::Repeat;

    bool hasPixelData() const { return !pixelData_.empty(); }
    bool LoadFromImage(const std::string& filepath);
};

} // namespace TLETC 