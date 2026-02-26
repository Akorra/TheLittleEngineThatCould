#pragma once

#include "TLETC/Core/Types.h"
#include "TLETC/Core/Math.h"

namespace TLETC::Render
{

// Forward declarations
class VertexBuffer;
class IndexBuffer;
class Shader;
class Texture;

// Primitive types
enum class PrimitiveType
{
    Triangles,
    Lines,
    Points
};

// Buffer usage hints
enum class BufferUsage
{
    Static,   // Set once, read many
    Dynamic,  // Updated frequently
    Stream    // Updated every frame
};

/**
 * RenderDevice
 *
 * Platform-agnostic rendering interface.
 * Implementations: OpenGL, Vulkan[TODO], D3D12[TODO], Metal[TODO]
 */
class RenderDevice
{
public:
    virtual ~RenderDevice() = default;

    // --- Lifecycle ---
    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;

    // --- Frame ---
    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;
    
    virtual void Clear(const vec4& color = vec4(0.1f, 0.1f, 0.15f, 1.0f)) = 0;
    virtual void SetViewport(uint32 x, uint32 y, uint32 width, uint32 height) = 0;

    // --- Resources ---
    virtual VertexBuffer* CreateVertexBuffer(const void* data, size_t size, BufferUsage usage) = 0;
    virtual IndexBuffer*  CreateIndexBuffer(const uint32* data, size_t count, BufferUsage usage) = 0;
    virtual Shader*       CreateShader(const char* vertexSrc, const char* fragmentSrc) = 0;
    virtual Texture*      CreateTexture(const void* data, uint32 width, uint32 height) = 0;

    virtual void DestroyVertexBuffer(VertexBuffer* buffer) = 0;
    virtual void DestroyIndexBuffer(IndexBuffer* buffer) = 0;
    virtual void DestroyShader(Shader* shader) = 0;
    virtual void DestroyTexture(Texture* texture) = 0;

    // --- Drawing ---
    virtual void BindShader(Shader* shader) = 0;
    virtual void BindVertexBuffer(VertexBuffer* buffer) = 0;
    virtual void BindIndexBuffer(IndexBuffer* buffer) = 0;
    virtual void BindTexture(Texture* texture, uint32 slot = 0) = 0;

    virtual void Draw(PrimitiveType type, uint32 vertexCount, uint32 startVertex = 0) = 0;
    virtual void DrawIndexed(PrimitiveType type, uint32 indexCount, uint32 startIndex = 0) = 0;

    // --- State ---
    virtual void SetDepthTest(bool enabled) = 0;
    virtual void SetBlend(bool enabled) = 0;
    virtual void SetCullFace(bool enabled) = 0;
};

} // namespace TLETC::Render