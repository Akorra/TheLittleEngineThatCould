#include "GLRenderDevice.h"
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>

namespace TLETC {

GLRenderDevice::GLRenderDevice() : currentShader_(), initialized_(false)
{
}

GLRenderDevice::~GLRenderDevice() 
{
    Shutdown();
}

bool GLRenderDevice::Initialize() 
{
    if (initialized_)
        return true;
    
    // GLAD should already be initialized by the window system
    // We just check if we have OpenGL
    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }
    
    std::cout << "OpenGL Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    
    // Set default OpenGL state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // Enable multisampling (MSAA) for anti-aliasing
    glEnable(GL_MULTISAMPLE);
    
    initialized_ = true;
    return true;
}

void GLRenderDevice::Shutdown() 
{
    if (!initialized_)
        return;
    
    // Clean up mesh cache
    for (auto& pair : meshCache_) 
    {
        glDeleteVertexArrays(1, &pair.second.vao);
        DestroyBuffer(pair.second.posVBO);
        DestroyBuffer(pair.second.nrmVBO);
        DestroyBuffer(pair.second.uvsVBO);
        DestroyBuffer(pair.second.clrVBO);
        if (pair.second.ibo.IsValid())
            DestroyBuffer(pair.second.ibo);
    }
    meshCache_.clear();
    
    initialized_ = false;
}

void GLRenderDevice::BeginFrame() 
{
    // Nothing specific needed for OpenGL begin frame
}

void GLRenderDevice::EndFrame() 
{
    // Nothing specific needed for OpenGL end frame
    // Actual swap buffers is handled by the window system
}

void GLRenderDevice::Clear(const Vec4& color) 
{
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

BufferHandle GLRenderDevice::CreateVertexBuffer(const void* data, size_t size, BufferUsage usage) 
{
    uint32 vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, size, data, GetGLUsage(usage));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    return BufferHandle(vbo);
}

BufferHandle GLRenderDevice::CreateIndexBuffer(const void* data, size_t size, BufferUsage usage) 
{
    uint32 ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GetGLUsage(usage));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    return BufferHandle(ibo);
}

void GLRenderDevice::UpdateBuffer(BufferHandle buffer, const void* data, size_t size, size_t offset) 
{
    if (!buffer.IsValid()) return;
    
    glBindBuffer(GL_ARRAY_BUFFER, buffer.GetID());
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GLRenderDevice::DestroyBuffer(BufferHandle buffer) 
{
    if (!buffer.IsValid()) return;
    
    uint32 id = buffer.GetID();
    glDeleteBuffers(1, &id);
}

ShaderHandle GLRenderDevice::CreateShader(ShaderType type, const std::string& source) 
{
    uint32 shader = glCreateShader(GetGLShaderType(type));
    
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    
    // Check compilation
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) 
    {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation failed: " << infoLog << std::endl;
        glDeleteShader(shader);
        return ShaderHandle();
    }
    
    return ShaderHandle(shader);
}

ShaderHandle GLRenderDevice::CreateShaderProgram(ShaderHandle vertexShader, ShaderHandle fragmentShader) 
{
    if (!vertexShader.IsValid() || !fragmentShader.IsValid()) 
        return ShaderHandle();
    
    uint32 program = glCreateProgram();
    glAttachShader(program, vertexShader.GetID());
    glAttachShader(program, fragmentShader.GetID());
    glLinkProgram(program);
    
    // Check linking
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) 
    {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Shader program linking failed: " << infoLog << std::endl;
        glDeleteProgram(program);
        return ShaderHandle();
    }
    
    // Shaders can be deleted after linking
    glDeleteShader(vertexShader.GetID());
    glDeleteShader(fragmentShader.GetID());
    
    return ShaderHandle(program);
}

ShaderHandle GLRenderDevice::CreateShaderProgram(ShaderHandle vertexShader, ShaderHandle geometryShader, ShaderHandle fragmentShader) 
{
    if (!vertexShader.IsValid() || !geometryShader.IsValid() || !fragmentShader.IsValid()) 
        return ShaderHandle();
    
    uint32 program = glCreateProgram();
    glAttachShader(program, vertexShader.GetID());
    glAttachShader(program, geometryShader.GetID());
    glAttachShader(program, fragmentShader.GetID());
    glLinkProgram(program);
    
    // Check linking
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Shader program linking failed: " << infoLog << std::endl;
        glDeleteProgram(program);
        return ShaderHandle();
    }
    
    // Shaders can be deleted after linking
    glDeleteShader(vertexShader.GetID());
    glDeleteShader(geometryShader.GetID());
    glDeleteShader(fragmentShader.GetID());
    
    return ShaderHandle(program);
}

ShaderHandle GLRenderDevice::CreateShaderProgram(ShaderHandle vertexShader, ShaderHandle tessControlShader, ShaderHandle tessEvalShader, ShaderHandle fragmentShader) 
{
    if (!vertexShader.IsValid() || !tessControlShader.IsValid() || !tessEvalShader.IsValid() || !fragmentShader.IsValid()) 
        return ShaderHandle();

    uint32 program = glCreateProgram();
    glAttachShader(program, vertexShader.GetID());
    glAttachShader(program, tessControlShader.GetID());
    glAttachShader(program, tessEvalShader.GetID());
    glAttachShader(program, fragmentShader.GetID());
    glLinkProgram(program);
    
    // Check linking
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) 
    {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Shader program linking failed: " << infoLog << std::endl;
        glDeleteProgram(program);
        return ShaderHandle();
    }
    
    // Shaders can be deleted after linking
    glDeleteShader(vertexShader.GetID());
    glDeleteShader(tessControlShader.GetID());
    glDeleteShader(tessEvalShader.GetID());
    glDeleteShader(fragmentShader.GetID());
    
    return ShaderHandle(program);
}

ShaderHandle GLRenderDevice::CreateShaderProgram(ShaderHandle vertexShader, ShaderHandle tessControlShader, ShaderHandle tessEvalShader, ShaderHandle geometryShader, ShaderHandle fragmentShader)
{
    if (!vertexShader.IsValid() || !tessControlShader.IsValid() ||  !tessEvalShader.IsValid() || !geometryShader.IsValid() || !fragmentShader.IsValid())
        return ShaderHandle();
    
    uint32 program = glCreateProgram();
    glAttachShader(program, vertexShader.GetID());
    glAttachShader(program, tessControlShader.GetID());
    glAttachShader(program, tessEvalShader.GetID());
    glAttachShader(program, geometryShader.GetID());
    glAttachShader(program, fragmentShader.GetID());
    glLinkProgram(program);
    
    // Check linking
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Shader program linking failed: " << infoLog << std::endl;
        glDeleteProgram(program);
        return ShaderHandle();
    }
    
    // Shaders can be deleted after linking
    glDeleteShader(vertexShader.GetID());
    glDeleteShader(tessControlShader.GetID());
    glDeleteShader(tessEvalShader.GetID());
    glDeleteShader(geometryShader.GetID());
    glDeleteShader(fragmentShader.GetID());
    
    return ShaderHandle(program);
}

ShaderHandle GLRenderDevice::CreateComputeProgram(ShaderHandle computeShader) 
{
    if (!computeShader.IsValid())
        return ShaderHandle();
    
    uint32 program = glCreateProgram();
    glAttachShader(program, computeShader.GetID());
    glLinkProgram(program);
    
    // Check linking
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) 
    {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Compute program linking failed: " << infoLog << std::endl;
        glDeleteProgram(program);
        return ShaderHandle();
    }
    
    // Shader can be deleted after linking
    glDeleteShader(computeShader.GetID());
    
    return ShaderHandle(program);
}

void GLRenderDevice::DestroyShader(ShaderHandle shader) 
{
    if (!shader.IsValid()) return;
    glDeleteProgram(shader.GetID());
}

void GLRenderDevice::UseShader(ShaderHandle shader) 
{
    if (shader.IsValid()) 
    {
        glUseProgram(shader.GetID());
        currentShader_ = shader;
    } 
    else 
    {
        glUseProgram(0);
        currentShader_ = ShaderHandle();
    }
}

void GLRenderDevice::SetUniformInt(ShaderHandle shader, const std::string& name, int value) 
{
    int location = GetUniformLocation(shader, name);
    if (location >= 0)
        glUniform1i(location, value);
}

void GLRenderDevice::SetUniformFloat(ShaderHandle shader, const std::string& name, float value) 
{
    int location = GetUniformLocation(shader, name);
    if (location >= 0)
        glUniform1f(location, value);
}

void GLRenderDevice::SetUniformVec2(ShaderHandle shader, const std::string& name, const Vec2& value) 
{
    int location = GetUniformLocation(shader, name);
    if (location >= 0)
        glUniform2fv(location, 1, glm::value_ptr(value));
}

void GLRenderDevice::SetUniformVec3(ShaderHandle shader, const std::string& name, const Vec3& value) 
{
    int location = GetUniformLocation(shader, name);
    if (location >= 0)
        glUniform3fv(location, 1, glm::value_ptr(value));
}

void GLRenderDevice::SetUniformVec4(ShaderHandle shader, const std::string& name, const Vec4& value) 
{
    int location = GetUniformLocation(shader, name);
    if (location >= 0)
        glUniform4fv(location, 1, glm::value_ptr(value));
}

void GLRenderDevice::SetUniformMat3(ShaderHandle shader, const std::string& name, const Mat3& value) 
{
    int location = GetUniformLocation(shader, name);
    if (location >= 0)
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
}


void GLRenderDevice::SetUniformMat4(ShaderHandle shader, const std::string& name, const Mat4& value) 
{
    int location = GetUniformLocation(shader, name);
    if (location >= 0)
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

// Texture operations
TextureHandle GLRenderDevice::CreateTexture(const char* filepath) 
{
    (void)filepath;
    // TODO: Load image from file using stb_image
    // For now, just create empty handle
    return TextureHandle();
}

TextureHandle GLRenderDevice::CreateTexture(int width, int height, const void* data) 
{
    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    
    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    // Create handle and cache
    TextureHandle handle = TextureHandle(textureId);
    textureCache_[handle] = textureId;
    return handle;
}

void GLRenderDevice::DestroyTexture(TextureHandle texture) 
{
    auto it = textureCache_.find(texture);
    if (it != textureCache_.end()) 
    {
        GLuint textureId = it->second;
        glDeleteTextures(1, &textureId);
        textureCache_.erase(it);
    }
}

void GLRenderDevice::BindTexture(TextureHandle texture, int slot) 
{
    auto it = textureCache_.find(texture);
    if (it != textureCache_.end()) 
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, it->second);
    }
}

void GLRenderDevice::DrawMesh(const Mesh& mesh, const Mat4& transform, PrimitiveType primitiveType) 
{
    if (mesh.IsEmpty()) return;
    
    // Check if we have this mesh cached
    auto it = meshCache_.find(&mesh);
    if (it == meshCache_.end()) 
    {
        // Create VAO for this mesh
        MeshData meshData;
        glGenVertexArrays(1, &meshData.vao);
        glBindVertexArray(meshData.vao);

        // TODO: let material and/or mesh handle this and primitive type
        // Now we have SetPatchVertices 
        // if (primitiveType == PrimitiveType::Patches) 
        //    glPatchParameteri(GL_PATCH_VERTICES, 3); // each patch has 3 vertices (triangle)

        const std::vector<Vec3>& positions = mesh.GetVertexPositions();
        const std::vector<Vec3>& normals   = mesh.GetVertexNormals();
        const std::vector<Vec2>& uvs       = mesh.GetVertexUVs();
        const std::vector<Vec4>& colors    = mesh.GetVertexColors();

        meshData.posVBO = CreateVertexBuffer(positions.data(), positions.size() * sizeof(Vec3), BufferUsage::Static);
        meshData.nrmVBO = CreateVertexBuffer(normals.data(), normals.size() * sizeof(Vec3), BufferUsage::Static);
        meshData.uvsVBO = CreateVertexBuffer(uvs.data(), uvs.size() * sizeof(Vec2), BufferUsage::Static);
        meshData.clrVBO = CreateVertexBuffer(colors.data(), colors.size() * sizeof(Vec4), BufferUsage::Static);
        
        // Position attribute (location = 0)
        glBindBuffer(GL_ARRAY_BUFFER, meshData.posVBO.GetID());
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        
        // Normal attribute (location = 1)
        glBindBuffer(GL_ARRAY_BUFFER, meshData.nrmVBO.GetID());
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        
        // UV attribute (location = 2)
        glBindBuffer(GL_ARRAY_BUFFER, meshData.uvsVBO.GetID());
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        
        // Color attribute (location = 3)
        glBindBuffer(GL_ARRAY_BUFFER, meshData.clrVBO.GetID());
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
        
        // Create and upload index buffer if mesh is indexed
        if (mesh.IsIndexed()) 
        {
            const auto& indices = mesh.GetIndices();
            meshData.ibo = CreateIndexBuffer(indices.data(), indices.size() * sizeof(uint32), BufferUsage::Static);
            meshData.indexCount = static_cast<uint32>(indices.size());
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData.ibo.GetID());
        }
        else 
        {
            meshData.indexCount = 0;
        }
        
        glBindVertexArray(0);
        
        // Cache it
        meshCache_[&mesh] = meshData;
        it = meshCache_.find(&mesh);
    }
    
    // Set transform uniform if we have a current shader
    if (currentShader_.IsValid())
        SetUniformMat4(currentShader_, "u_model", transform);
    
    // Draw the mesh
    const MeshData& meshData = it->second;
    glBindVertexArray(meshData.vao);
    
    if (meshData.indexCount > 0)
        glDrawElements(GetGLPrimitiveType(primitiveType), meshData.indexCount, GL_UNSIGNED_INT, nullptr);
    else
        glDrawArrays(GetGLPrimitiveType(primitiveType), 0, static_cast<GLsizei>(mesh.GetVertexCount()));
    
    glBindVertexArray(0);
}

void GLRenderDevice::DrawIndexed(BufferHandle vertexBuffer, BufferHandle indexBuffer, uint32 indexCount, PrimitiveType primitiveType)
{
    if (!vertexBuffer.IsValid() || !indexBuffer.IsValid()) return;
    
    // This is a lower-level draw call, requires manual VAO setup
    // For now, we'll mainly use DrawMesh
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer.GetID());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.GetID());
    
    glDrawElements(GetGLPrimitiveType(primitiveType), indexCount, GL_UNSIGNED_INT, nullptr);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void GLRenderDevice::SetViewport(uint32 x, uint32 y, uint32 width, uint32 height) 
{
    glViewport(x, y, width, height);
}

void GLRenderDevice::EnableDepthTest(bool enable) 
{
    if (enable)
        glEnable(GL_DEPTH_TEST);
    else 
        glDisable(GL_DEPTH_TEST);
}

void GLRenderDevice::EnableBlending(bool enable)
{
    if (enable) 
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    } 
    else 
    {
        glDisable(GL_BLEND);
    }
}

void GLRenderDevice::EnableCulling(bool enable) 
{
    if (enable) 
        glEnable(GL_CULL_FACE);
    else 
        glDisable(GL_CULL_FACE);
}

void GLRenderDevice::SetWireframeMode(bool enable) 
{
    if (enable)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}


void GLRenderDevice::DispatchCompute(uint32 groupsX, uint32 groupsY, uint32 groupsZ) 
{
    glDispatchCompute(groupsX, groupsY, groupsZ);
}

void GLRenderDevice::MemoryBarrier() 
{
    // Ensure all memory writes are visible
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void GLRenderDevice::SetPatchVertices(uint32 count) 
{
    glPatchParameteri(GL_PATCH_VERTICES, count);
}

const int GLRenderDevice::GetMaxTessLevel() const
{
    GLint maxTessLevel = 0;
    glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &maxTessLevel);
    return maxTessLevel;
}

const char* GLRenderDevice::GetRendererName() const 
{
    return reinterpret_cast<const char*>(glGetString(GL_RENDERER));
}

const char* GLRenderDevice::GetAPIVersion() const 
{
    return reinterpret_cast<const char*>(glGetString(GL_VERSION));
}

// Helper functions

uint32 GLRenderDevice::GetGLUsage(BufferUsage usage) 
{
    switch (usage) 
    {
        case BufferUsage::Static:  return GL_STATIC_DRAW;
        case BufferUsage::Dynamic: return GL_DYNAMIC_DRAW;
        case BufferUsage::Stream:  return GL_STREAM_DRAW;
        default: return GL_STATIC_DRAW;
    }
}

uint32 GLRenderDevice::GetGLShaderType(ShaderType type) 
{
    switch (type) 
    {
        case ShaderType::Vertex:         return GL_VERTEX_SHADER;
        case ShaderType::Fragment:       return GL_FRAGMENT_SHADER;
        case ShaderType::Geometry:       return GL_GEOMETRY_SHADER;
        case ShaderType::TessControl:    return GL_TESS_CONTROL_SHADER;
        case ShaderType::TessEvaluation: return GL_TESS_EVALUATION_SHADER;
        case ShaderType::Compute:        return GL_COMPUTE_SHADER;
        default: return GL_VERTEX_SHADER;
    }
}

uint32 GLRenderDevice::GetGLPrimitiveType(PrimitiveType type) 
{
    switch (type) 
    {
        case PrimitiveType::Triangles:     return GL_TRIANGLES;
        case PrimitiveType::Lines:         return GL_LINES;
        case PrimitiveType::Points:        return GL_POINTS;
        case PrimitiveType::TriangleStrip: return GL_TRIANGLE_STRIP;
        case PrimitiveType::LineStrip:     return GL_LINE_STRIP;
        case PrimitiveType::Patches:       return GL_PATCHES;
        default: return GL_TRIANGLES;
    }
}

int GLRenderDevice::GetUniformLocation(ShaderHandle shader, const std::string& name) 
{
    if (!shader.IsValid()) 
        return -1;
    return glGetUniformLocation(shader.GetID(), name.c_str());
}

} // namespace TLETC
