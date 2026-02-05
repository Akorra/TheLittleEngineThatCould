#pragma once

#include "TLETC/Core/Types.h"
#include "TLETC/Core/Math.h"
#include "TLETC/Rendering/Handles.h"
#include <string>
#include <unordered_map>

namespace TLETC 
{
// Forward Declarations
class RenderDevice;

/**
 * Material - Shader wrapper with properties and render state
 * 
 * Stores shader uniforms, textures, and OpenGL state (blending, culling, etc.)
 * Application calls Bind() during rendering to apply everything to GPU.
 * 
 * Usage:
 *   auto* mat = new Material("RedPlastic");
 *   mat->SetShader(shader);
 *   mat->SetVec3("u_color", Vec3(1, 0, 0));
 *   mat->SetFloat("u_metallic", 0.0f);
 */
class Material 
{
public:
    enum class CullMode  { None, Front, Back };
    enum class BlendMode { Opaque, Transparent, Additive, Multiply };
    enum class DepthFunc { Always, Never, Less, LessEqual, Greater, GreaterEqual, Equal, NotEqual };

    Material(const std::string& name);
    ~Material();

    // Shader
    void SetShader(ShaderHandle shader) { shader_ = shader; }
    ShaderHandle GetShader() const { return shader_; }

    // Property setters
    void SetFloat(const std::string& name, float value);
    void SetInt(const std::string& name, int value);
    void SetVec2(const std::string& name, const Vec2& value);
    void SetVec3(const std::string& name, const Vec3& value);
    void SetVec4(const std::string& name, const Vec4& value);
    void SetMat3(const std::string& name, const Mat3& value);
    void SetMat4(const std::string& name, const Mat4& value);
    void SetTexture(const std::string& name, TextureHandle texture);

    // Property getters (with defaults)
    float GetFloat(const std::string& name, float defaultValue = 0.0f) const;
    int GetInt(const std::string& name, int defaultValue = 0) const;
    Vec2 GetVec2(const std::string& name, const Vec2& defaultValue = Vec2(0)) const;
    Vec3 GetVec3(const std::string& name, const Vec3& defaultValue = Vec3(0)) const;
    Vec4 GetVec4(const std::string& name, const Vec4& defaultValue = Vec4(0)) const;
    Mat3 GetMat3(const std::string& name, const Mat3& defaultValue = Mat3(1)) const;
    Mat4 GetMat4(const std::string& name, const Mat4& defaultValue = Mat4(1)) const;
    TextureHandle GetTexture(const std::string& name) const;

    // Check if property exists
    bool HasProperty(const std::string& name) const;

    // Render state
    void SetCullMode(CullMode mode) { cullMode_ = mode; }
    CullMode GetCullMode() const { return cullMode_; }
    
    void SetBlendMode(BlendMode mode) { blendMode_ = mode; }
    BlendMode GetBlendMode() const { return blendMode_; }
    
    void SetDepthTest(bool enabled) { depthTest_ = enabled; }
    bool GetDepthTest() const { return depthTest_; }
    
    void SetDepthWrite(bool enabled) { depthWrite_ = enabled; }
    bool GetDepthWrite() const { return depthWrite_; }
    
    void SetDepthFunc(DepthFunc func) { depthFunc_ = func; }
    DepthFunc GetDepthFunc() const { return depthFunc_; }
    
    void SetTransparent(bool transparent_);
    bool IsTransparent() const { return transparent_; }
    
    void SetRenderQueue(int queue) { renderQueue_ = queue; }
    int GetRenderQueue() const { return renderQueue_; }
    
    // Parent material (for property inheritance)
    void SetParent(Material* parent) { parent_ = parent; }
    Material* GetParent() const { return parent_; }
    
    // Apply all properties and render state to GPU (called by Application)
    void Bind(RenderDevice* renderer) const;
    
    // Name for debugging
    const std::string& GetName() const { return name_; }
    void SetName(const std::string& name) { name_ = name; }

private:
    std::string name_;
    ShaderHandle shader_;
    
    // Property storage (separate maps per type)
    std::unordered_map<std::string, float>         floats_;
    std::unordered_map<std::string, int>           ints_;
    std::unordered_map<std::string, Vec2>          vec2s_;
    std::unordered_map<std::string, Vec3>          vec3s_;
    std::unordered_map<std::string, Vec4>          vec4s_;
    std::unordered_map<std::string, Mat3>          mat3s_;
    std::unordered_map<std::string, Mat4>          mat4s_;
    std::unordered_map<std::string, TextureHandle> textures_;
    
    // Render state
    CullMode  cullMode_;
    BlendMode blendMode_;
    DepthFunc depthFunc_;

    bool depthTest_;
    bool depthWrite_;
    bool transparent_;
    int  renderQueue_;
    
    // Parent material (for shared properties)
    Material* parent_;   
};

} // namespace TLETC 