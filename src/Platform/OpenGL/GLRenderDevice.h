#pragma once

#include "TLETC/Rendering/RenderDevice.h"
#include <unordered_map>

namespace TLETC {

/**
 * GLRenderDevice - OpenGL implementation of RenderDevice
 */
class GLRenderDevice : public RenderDevice 
{
public:
    GLRenderDevice();
    ~GLRenderDevice() override;
    
    // RenderDevice interface
    bool Initialize() override;
    void Shutdown() override;
    
    // Frame management
    void BeginFrame() override;
    void EndFrame() override;
    void Clear(const Vec4& color) override;
    
    // Buffer operations
    BufferHandle CreateVertexBuffer(const void* data, size_t size, BufferUsage usage) override;
    BufferHandle CreateIndexBuffer(const void* data, size_t size, BufferUsage usage) override;
    void UpdateBuffer(BufferHandle buffer, const void* data, size_t size, size_t offset = 0) override;
    void DestroyBuffer(BufferHandle buffer) override;
    
    // Shader operations
    ShaderHandle CreateShader(ShaderType type, const std::string& source) override;

    // Create shader program with vertex + fragment (most common)
    ShaderHandle CreateShaderProgram(ShaderHandle vertexShader, ShaderHandle fragmentShader) override;

    // Create shader program with vertex + geometry + fragment
    ShaderHandle CreateShaderProgram(ShaderHandle vertexShader, ShaderHandle geometryShader, ShaderHandle fragmentShader) override;

    // Create shader program with full tessellation pipeline
    ShaderHandle CreateShaderProgram(ShaderHandle vertexShader, ShaderHandle tessControlShader, ShaderHandle tessEvalShader, ShaderHandle fragmentShader) override;
    
    // Create shader program with tessellation + geometry
    ShaderHandle CreateShaderProgram(ShaderHandle vertexShader, ShaderHandle tessControlShader, ShaderHandle tessEvalShader, ShaderHandle geometryShader, ShaderHandle fragmentShader) override;
   
    // Create compute shader program
    ShaderHandle CreateComputeProgram(ShaderHandle computeShader) override;
    
    void DestroyShader(ShaderHandle shader) override;
    void UseShader(ShaderHandle shader) override;
    
    // Shader uniforms
    void SetUniformInt(ShaderHandle shader, const std::string& name, int value) override;
    void SetUniformFloat(ShaderHandle shader, const std::string& name, float value) override;
    void SetUniformVec2(ShaderHandle shader, const std::string& name, const Vec2& value) override;
    void SetUniformVec3(ShaderHandle shader, const std::string& name, const Vec3& value) override;
    void SetUniformVec4(ShaderHandle shader, const std::string& name, const Vec4& value) override;
    void SetUniformMat3(ShaderHandle shader, const std::string& name, const Mat3& value) override;
    void SetUniformMat4(ShaderHandle shader, const std::string& name, const Mat4& value) override;

    // Texture operations
    TextureHandle LoadTexture(const char* filepath, int& outWidth, int& outHeight, TextureFormat& outFormat) override;
    TextureHandle CreateTexture(int width, int height, TextureFormat format, const void* data = nullptr) override;
    void DestroyTexture(TextureHandle texture) override;
    void BindTexture(TextureHandle texture, int slot = 0) override;
    void SetTextureFilter(TextureHandle texture, TextureFilter minFilter, TextureFilter magFilter) override;
    void SetTextureWrap(TextureHandle texture, TextureWrap wrapS, TextureWrap wrapT) override;
    void GenerateTextureMipmaps(TextureHandle texture) override;
    
    // Mesh rendering
    void DrawMesh(const Mesh& mesh, const Mat4& transform, PrimitiveType primitiveType = PrimitiveType::Triangles) override;
    void DrawIndexed(BufferHandle vertexBuffer, BufferHandle indexBuffer, uint32 indexCount, PrimitiveType primitiveType = PrimitiveType::Triangles) override;
    
    // Compute shader operations
    void DispatchCompute(uint32 groupsX, uint32 groupsY, uint32 groupsZ) override;
    void MemoryBarrier() override;

    // Tessellation control
    void SetPatchVertices(uint32 count) override; // Set number of vertices per patch (for tessellation)
    
    // State management
    void SetViewport(uint32 x, uint32 y, uint32 width, uint32 height) override;
    void EnableDepthTest(bool enable) override;
    void EnableBlending(bool enable) override;
    void EnableCulling(bool enable) override;
    void SetWireframeMode(bool enable) override;
    
    // Query
    const int   GetMaxTessLevel() const override;
    const char* GetRendererName() const override;
    const char* GetAPIVersion() const override;
    
private:
    // Helper functions
    uint32 GetGLUsage(BufferUsage usage);
    uint32 GetGLShaderType(ShaderType type);
    uint32 GetGLPrimitiveType(PrimitiveType type);
    uint32 GetGLTextureFormat(TextureFormat format);
    uint32 GetGLTextureFilter(TextureFilter filter);
    uint32 GetGLTextureWrap(TextureWrap wrap);
    int    GetUniformLocation(ShaderHandle shader, const std::string& name);
    
    // Mesh VAO cache - stores VAO for each mesh to avoid recreating
    struct MeshData {
        uint32 vao;
        BufferHandle posVBO, nrmVBO, uvsVBO, clrVBO;
        BufferHandle ibo;
        uint32 indexCount;
    };
    std::unordered_map<const Mesh*, MeshData> meshCache_;
    std::unordered_map<TextureHandle, uint32, TextureHandle::Hash> textureCache_;
    
    // Current state
    ShaderHandle currentShader_;
    
    // Track if initialized
    bool initialized_;
};

} // namespace TLETC