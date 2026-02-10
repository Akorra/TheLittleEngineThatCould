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
    Texture();
    ~Texture();
    
    // Load from file
    bool LoadFromFile(const std::string& filepath, RenderDevice* renderer);
    
    // Create from memory
    bool CreateFromMemory(int width, int height, TextureFormat format, 
                         const void* data, RenderDevice* renderer);
    
    // Create empty texture
    bool Create(int width, int height, TextureFormat format, RenderDevice* renderer);
    
    // Destroy (must be called explicitly with RenderDevice)
    void Destroy(RenderDevice* renderer);
    
    // Getters
    TextureHandle GetHandle() const { return handle_; }
    int GetWidth() const { return width_; }
    int GetHeight() const { return height_; }
    TextureFormat GetFormat() const { return format_; }
    bool IsValid() const { return handle_.IsValid(); }
    
    // Texture parameters
    void SetFilter(TextureFilter minFilter, TextureFilter magFilter, RenderDevice* renderer);
    void SetWrap(TextureWrap wrapS, TextureWrap wrapT, RenderDevice* renderer);
    void GenerateMipmaps(RenderDevice* renderer);
    
    // Name for debugging
    const std::string& GetFilepath() const { return filepath_; }
private:
    TextureHandle handle_;
    
    int width_;
    int height_;
    
    TextureFormat format_;
    std::string filepath_;
    
    TextureFilter minFilter_;
    TextureFilter magFilter_;
    
    TextureWrap wrapS_;
    TextureWrap wrapT_;
};

} // namespace TLETC 