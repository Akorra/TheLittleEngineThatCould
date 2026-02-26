#pragma once

#include "TLETC/Render/Shader.h"
#include <glad/gl.h>
#include <unordered_map>

namespace TLETC::Render
{

class OGLShader : public Shader
{
public:
    OGLShader(const char* vertexSrc, const char* fragmentSrc);
    ~OGLShader() override;

    void Bind() const override;
    void Unbind() const override;

    void SetInt(const std::string& name, int value) override;
    void SetFloat(const std::string& name, float value) override;
    void SetVec2(const std::string& name, const vec2& value) override;
    void SetVec3(const std::string& name, const vec3& value) override;
    void SetVec4(const std::string& name, const vec4& value) override;
    void SetMat4(const std::string& name, const mat4& value) override;

private:
    GLint GetUniformLocation(const std::string& name);

private:
    GLuint program_ = 0;
    mutable std::unordered_map<std::string, GLint> uniformCache_;
};

} // namespace TLETC::Render