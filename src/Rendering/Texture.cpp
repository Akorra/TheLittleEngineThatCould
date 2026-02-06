#include "TLETC/Rendering/Texture.h"
#include "TLETC/Rendering/RenderDevice.h"
#include <iostream>

namespace TLETC {

Texture::Texture()
    : handle_()
    , width_(0)
    , height_(0)
    , format_(TextureFormat::RGBA)
    , minFilter_(TextureFilter::LinearMipmapLinear)
    , magFilter_(TextureFilter::Linear)
    , wrapS_(TextureWrap::Repeat)
    , wrapT_(TextureWrap::Repeat)
{
}

Texture::~Texture() 
{
    // User must call Destroy() explicitly
    if (handle_.IsValid())
        std::cerr << "Warning: Texture not destroyed: " << filepath_ << std::endl;
}

bool Texture::LoadFromFile(const std::string& filepath, RenderDevice* renderer) 
{
    filepath_ = filepath;
    handle_ = renderer->LoadTexture(filepath.c_str(), width_, height_, format_);
    
    if (!handle_.IsValid()) {
        std::cerr << "Failed to load texture: " << filepath << std::endl;
        return false;
    }
    
    // Apply default filtering
    SetFilter(minFilter_, magFilter_, renderer);
    SetWrap(wrapS_, wrapT_, renderer);
    GenerateMipmaps(renderer);
    
    std::cout << "Loaded texture: " << filepath << " (" << width_ << "x" << height_ << ")" << std::endl;
    return true;
}

bool Texture::CreateFromMemory(int width, int height, TextureFormat format, const void* data, RenderDevice* renderer) 
{
    width_ = width;
    height_ = height;
    format_ = format;
    
    handle_ = renderer->CreateTexture(width, height, format, data);
    
    if (!handle_.IsValid()) {
        std::cerr << "Failed to create texture from memory" << std::endl;
        return false;
    }
    
    SetFilter(minFilter_, magFilter_, renderer);
    SetWrap(wrapS_, wrapT_, renderer);
    
    return true;
}

bool Texture::Create(int width, int height, TextureFormat format, RenderDevice* renderer) 
{
    return CreateFromMemory(width, height, format, nullptr, renderer);
}

void Texture::Destroy(RenderDevice* renderer) 
{
    if (handle_.IsValid()) {
        renderer->DestroyTexture(handle_);
        handle_.Reset();
        width_ = 0;
        height_ = 0;
    }
}

void Texture::SetFilter(TextureFilter minFilter, TextureFilter magFilter, RenderDevice* renderer) 
{
    minFilter_ = minFilter;
    magFilter_ = magFilter;
    renderer->SetTextureFilter(handle_, minFilter, magFilter);
}

void Texture::SetWrap(TextureWrap wrapS, TextureWrap wrapT, RenderDevice* renderer) 
{
    wrapS_ = wrapS;
    wrapT_ = wrapT;
    renderer->SetTextureWrap(handle_, wrapS, wrapT);
}

void Texture::GenerateMipmaps(RenderDevice* renderer) 
{
    renderer->GenerateTextureMipmaps(handle_);
}

} // namespace TLETC