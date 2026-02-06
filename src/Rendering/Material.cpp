#include "TLETC/Rendering/Material.h"
#include "TLETC/Rendering/RenderDevice.h"
#include "TLETC/Rendering/Texture.h"

#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>

namespace TLETC 
{

Material::Material(const std::string& name)
    : name_(name)
    , shader_()
    , cullMode_(CullMode::Back)
    , blendMode_(BlendMode::Opaque)
    , depthFunc_(DepthFunc::LessEqual)
    , depthTest_(true)
    , depthWrite_(true)
    , transparent_(false)
    , renderQueue_(2000)  // Opaque queue
    , parent_(nullptr)
{
}

Material::~Material() 
{
}

// Property setters
void Material::SetFloat(const std::string& name, float value)        { floats_[name] = value; }
void Material::SetInt(const std::string& name, int value)            { ints_[name] = value; }
void Material::SetVec2(const std::string& name, const Vec2& value)   { vec2s_[name] = value; }
void Material::SetVec3(const std::string& name, const Vec3& value)   { vec3s_[name] = value; }
void Material::SetVec4(const std::string& name, const Vec4& value)   { vec4s_[name] = value; }
void Material::SetMat3(const std::string& name, const Mat3& value)   { mat3s_[name] = value; }
void Material::SetMat4(const std::string& name, const Mat4& value)   { mat4s_[name] = value; }
void Material::SetTexture(const std::string& name, Texture* texture) { textures_[name] = texture; }

// Property getters with parent fallback
float Material::GetFloat(const std::string& name, float defaultValue) const 
{
    auto it = floats_.find(name);
    if (it != floats_.end()) return it->second;
    if (parent_) return parent_->GetFloat(name, defaultValue);
    return defaultValue;
}

int Material::GetInt(const std::string& name, int defaultValue) const 
{
    auto it = ints_.find(name);
    if (it != ints_.end()) return it->second;
    if (parent_) return parent_->GetInt(name, defaultValue);
    return defaultValue;
}

Vec2 Material::GetVec2(const std::string& name, const Vec2& defaultValue) const 
{
    auto it = vec2s_.find(name);
    if (it != vec2s_.end()) return it->second;
    if (parent_) return parent_->GetVec2(name, defaultValue);
    return defaultValue;
}

Vec3 Material::GetVec3(const std::string& name, const Vec3& defaultValue) const 
{
    auto it = vec3s_.find(name);
    if (it != vec3s_.end()) return it->second;
    if (parent_) return parent_->GetVec3(name, defaultValue);
    return defaultValue;
}

Vec4 Material::GetVec4(const std::string& name, const Vec4& defaultValue) const 
{
    auto it = vec4s_.find(name);
    if (it != vec4s_.end()) return it->second;
    if (parent_) return parent_->GetVec4(name, defaultValue);
    return defaultValue;
}

Mat3 Material::GetMat3(const std::string& name, const Mat3& defaultValue) const 
{
    auto it = mat3s_.find(name);
    if (it != mat3s_.end()) return it->second;
    if (parent_) return parent_->GetMat3(name, defaultValue);
    return defaultValue;
}

Mat4 Material::GetMat4(const std::string& name, const Mat4& defaultValue) const 
{
    auto it = mat4s_.find(name);
    if (it != mat4s_.end()) return it->second;
    if (parent_) return parent_->GetMat4(name, defaultValue);
    return defaultValue;
}

Texture* Material::GetTexture(const std::string& name) const 
{
    auto it = textures_.find(name);
    if (it != textures_.end()) return it->second;
    if (parent_) return parent_->GetTexture(name);
    return nullptr;
}

bool Material::HasProperty(const std::string& name) const 
{
    if (floats_.count(name))   return true;
    if (ints_.count(name))     return true;
    if (vec2s_.count(name))    return true;
    if (vec3s_.count(name))    return true;
    if (vec4s_.count(name))    return true;
    if (mat3s_.count(name))    return true;
    if (mat4s_.count(name))    return true;
    if (textures_.count(name)) return true;
    if (parent_) 
        return parent_->HasProperty(name);
    return false;
}

void Material::SetTransparent(bool transparent) 
{
    transparent_ = transparent;
    if (transparent) 
    {
        renderQueue_ = 3000;  // Transparent queue (rendered after opaque)
        depthWrite_  = false;   // Transparent objects don't write depth
        blendMode_   = BlendMode::Transparent;
    } 
    else 
    {
        renderQueue_ = 2000;  // Opaque queue
        depthWrite_  = true;
        blendMode_   = BlendMode::Opaque;
    }
}

void Material::Bind(RenderDevice* renderer) const {
    // Use shader
    renderer->UseShader(shader_);
    
    // Set all float properties
    for (const auto& [name, value] : floats_) {
        renderer->SetUniformFloat(shader_, name, value);
    }
    
    // Set all int properties
    for (const auto& [name, value] : ints_) {
        renderer->SetUniformInt(shader_, name, value);
    }
    
    // Set all Vec2 properties
    for (const auto& [name, value] : vec2s_) {
        renderer->SetUniformVec2(shader_, name, value);
    }
    
    // Set all Vec3 properties
    for (const auto& [name, value] : vec3s_) {
        renderer->SetUniformVec3(shader_, name, value);
    }
    
    // Set all Vec4 properties
    for (const auto& [name, value] : vec4s_) {
        renderer->SetUniformVec4(shader_, name, value);
    }
    
    // Set all Mat3 properties
    for (const auto& [name, value] : mat3s_) {
        renderer->SetUniformMat3(shader_, name, value);
    }
    
    // Set all Mat4 properties
    for (const auto& [name, value] : mat4s_) {
        renderer->SetUniformMat4(shader_, name, value);
    }
    
    // Set all texture properties
    int textureSlot = 0;
    for (const auto& [name, texture] : textures_) 
    {
        if (texture && texture->IsValid()) 
        {
            renderer->BindTexture(texture->GetHandle(), textureSlot);
            renderer->SetUniformInt(shader_, name, textureSlot);
            textureSlot++;
        }
    }
    
    // Apply render state (OpenGL state changes)
    // Note: RenderDevice doesn't have these yet, we'll add them
    // For now, just set uniforms
}

} // namespace TLETC