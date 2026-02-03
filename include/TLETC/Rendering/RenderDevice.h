#pragma once

#include "TLETC/Core/Types.h"
#include "TLETC/Core/Math.h"
#include "TLETC/Rendering/Handle.h"
#include "TLETC/Resources/Mesh.h"

#include <string>

namespace TLETC 
{
// Shader types
enum class ShaderType {
    Vertex,           // Vertex shader - transforms vertices
    Fragment,         // Fragment/Pixel shader - colors pixels
    Geometry,         // Geometry shader - generates/modifies primitives
    TessControl,      // Tessellation control shader (hull shader in DX)
    TessEvaluation,   // Tessellation evaluation shader (domain shader in DX)
    Compute           // Compute shader - general purpose GPU computing
};

// Buffer usage hints
enum class BufferUsage 
{
    Static,   // Data won't change
    Dynamic,  // Data changes occasionally
    Stream    // Data changes every frame
};

// Primitive topology
enum class PrimitiveType {
    Triangles,
    Lines,
    Points,
    TriangleStrip,
    LineStrip,
    Patches         // For tessellation
};

/**
 * RenderDevice - Abstract interface for rendering APIs
 * 
 * This interface abstracts away the specific graphics API (OpenGL, Vulkan, DirectX).
 * Implementations handle the actual GPU communication.
 */
class RenderDevice {
public:
    virtual ~RenderDevice() = default;
    
    // Initialization
    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;
    
    // Frame management
    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;
    virtual void Clear(const Vec4& color) = 0;
    
    // Buffer operations
    virtual BufferHandle CreateVertexBuffer(const void* data, size_t size, BufferUsage usage) = 0;
    virtual BufferHandle CreateIndexBuffer(const void* data, size_t size, BufferUsage usage) = 0;
    virtual void UpdateBuffer(BufferHandle buffer, const void* data, size_t size, size_t offset = 0) = 0;
    virtual void DestroyBuffer(BufferHandle buffer) = 0;
    
    // Shader operations
    virtual ShaderHandle CreateShader(ShaderType type, const std::string& source) = 0;
    
    // Create shader program with vertex + fragment (most common)
    virtual ShaderHandle CreateShaderProgram(ShaderHandle vertexShader, ShaderHandle fragmentShader) = 0;
    
    // Create shader program with vertex + geometry + fragment
    virtual ShaderHandle CreateShaderProgram(ShaderHandle vertexShader, 
                                            ShaderHandle geometryShader,
                                            ShaderHandle fragmentShader) = 0;
    
    // Create shader program with full tessellation pipeline
    virtual ShaderHandle CreateShaderProgram(ShaderHandle vertexShader,
                                            ShaderHandle tessControlShader,
                                            ShaderHandle tessEvalShader,
                                            ShaderHandle fragmentShader) = 0;
    
    // Create shader program with tessellation + geometry
    virtual ShaderHandle CreateShaderProgram(ShaderHandle vertexShader,
                                            ShaderHandle tessControlShader,
                                            ShaderHandle tessEvalShader,
                                            ShaderHandle geometryShader,
                                            ShaderHandle fragmentShader) = 0;
    
    // Create compute shader program
    virtual ShaderHandle CreateComputeProgram(ShaderHandle computeShader) = 0;
    
    virtual void DestroyShader(ShaderHandle shader) = 0;
    virtual void UseShader(ShaderHandle shader) = 0;
    
    // Shader uniforms
    virtual void SetUniformInt(ShaderHandle shader, const std::string& name, int value) = 0;
    virtual void SetUniformFloat(ShaderHandle shader, const std::string& name, float value) = 0;
    virtual void SetUniformVec3(ShaderHandle shader, const std::string& name, const Vec3& value) = 0;
    virtual void SetUniformVec4(ShaderHandle shader, const std::string& name, const Vec4& value) = 0;
    virtual void SetUniformMat4(ShaderHandle shader, const std::string& name, const Mat4& value) = 0;
    
    // Mesh rendering
    virtual void DrawMesh(const Mesh& mesh, const Mat4& transform, PrimitiveType primitiveType = PrimitiveType::Triangles) = 0;
    virtual void DrawIndexed(BufferHandle vertexBuffer, BufferHandle indexBuffer, uint32 indexCount, PrimitiveType primitiveType = PrimitiveType::Triangles) = 0;
    
    // Compute shader operations
    virtual void DispatchCompute(uint32 groupsX, uint32 groupsY, uint32 groupsZ) = 0;
    virtual void MemoryBarrier() = 0; // Ensure compute writes are visible
    
    // Tessellation control
    virtual void SetPatchVertices(uint32 count) = 0; // Set number of vertices per patch (for tessellation)
    
    // State management
    virtual void SetViewport(uint32 x, uint32 y, uint32 width, uint32 height) = 0;
    virtual void EnableDepthTest(bool enable) = 0;
    virtual void EnableBlending(bool enable) = 0;
    virtual void EnableCulling(bool enable) = 0;
    virtual void SetWireframeMode(bool enable) = 0;
    
    // Query
    virtual const int   GetMaxTessLevel() const = 0;
    virtual const char* GetRendererName() const = 0;
    virtual const char* GetAPIVersion() const = 0;
};

// ============================================================================
// Railroad-Themed Aliases 🔥
// ============================================================================

// The firebox generates power/heat to make the engine run (renders frames)
using Firebox = RenderDevice;

} // namespace TLETC