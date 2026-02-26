#include "TLETC/Render/Frameworks/OpenGL/OGLShader.h"
#include "TLETC/Core/Log.h"
#include "TLETC/Core/Assert.h"

namespace TLETC::Render
{

static GLuint CompileShader(GLenum type, const char* source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    // Check compile status
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        TLETC_ERROR("Shader compilation failed: ", infoLog);
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

OGLShader::OGLShader(const char* vertexSrc, const char* fragmentSrc)
{
    GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSrc);
    GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSrc);

    TLETC_ASSERT(vertexShader && fragmentShader, "Shader compilation failed!");

    // Link program
    program_ = glCreateProgram();
    glAttachShader(program_, vertexShader);
    glAttachShader(program_, fragmentShader);
    glLinkProgram(program_);

    // Check link status
    GLint success;
    glGetProgramiv(program_, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(program_, 512, nullptr, infoLog);
        TLETC_ERROR("Shader linking failed: ", infoLog);
    }

    // Clean up
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

OGLShader::~OGLShader()
{
    glDeleteProgram(program_);
}

void OGLShader::Bind() const
{
    glUseProgram(program_);
}

void OGLShader::Unbind() const
{
    glUseProgram(0);
}

GLint OGLShader::GetUniformLocation(const std::string& name)
{
    // Check cache first
    auto it = uniformCache_.find(name);
    if (it != uniformCache_.end())
        return it->second;

    // Query OpenGL
    GLint location = glGetUniformLocation(program_, name.c_str());
    uniformCache_[name] = location;

    if (location == -1)
        TLETC_WARN("Uniform '", name, "' not found in shader");

    return location;
}

void OGLShader::SetInt(const std::string& name, int value)
{
    glUniform1i(GetUniformLocation(name), value);
}

void OGLShader::SetFloat(const std::string& name, float value)
{
    glUniform1f(GetUniformLocation(name), value);
}

void OGLShader::SetVec2(const std::string& name, const vec2& value)
{
    glUniform2f(GetUniformLocation(name), value.x, value.y);
}

void OGLShader::SetVec3(const std::string& name, const vec3& value)
{
    glUniform3f(GetUniformLocation(name), value.x, value.y, value.z);
}

void OGLShader::SetVec4(const std::string& name, const vec4& value)
{
    glUniform4f(GetUniformLocation(name), value.x, value.y, value.z, value.w);
}

void OGLShader::SetMat4(const std::string& name, const mat4& value)
{
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, value_ptr(value));
}

} // namespace TLETC::Render