// TLETC/Renderer/Shader.h
#pragma once

#include "TLETC/Core/Types.h"
#include "TLETC/Core/Math.h"
#include <string>

namespace TLETC::Render
{

/**
 * Shader
 * Base class for shader programs
 */
class Shader
{
public:
    virtual ~Shader() = default;

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    // Uniform setters
    virtual void SetInt(const std::string& name, int value) = 0;
    virtual void SetFloat(const std::string& name, float value) = 0;
    virtual void SetVec2(const std::string& name, const vec2& value) = 0;
    virtual void SetVec3(const std::string& name, const vec3& value) = 0;
    virtual void SetVec4(const std::string& name, const vec4& value) = 0;
    virtual void SetMat4(const std::string& name, const mat4& value) = 0;
};

} // namespace TLETC::Render