#pragma once

#include "TLETC/Render/RenderDevice.h"
#include <glad/gl.h>

namespace TLETC::Render
{

class OGLDevice : public RenderDevice
{
public:
    bool Initialize() override;
    void Shutdown() override;

    void BeginFrame() override;
    void EndFrame() override;
    
    void Clear(const vec4& color) override;
    void SetViewport(uint32 x, uint32 y, uint32 width, uint32 height) override;

    VertexBuffer* CreateVertexBuffer(const void* data, size_t size, BufferUsage usage) override;
    IndexBuffer*  CreateIndexBuffer(const uint32* data, size_t count, BufferUsage usage) override;
    Shader*       CreateShader(const char* vertexSrc, const char* fragmentSrc) override;
    Texture*      CreateTexture(const void* data, uint32 width, uint32 height) override;

    void DestroyVertexBuffer(VertexBuffer* buffer) override;
    void DestroyIndexBuffer(IndexBuffer* buffer) override;
    void DestroyShader(Shader* shader) override;
    void DestroyTexture(Texture* texture) override;

    void BindShader(Shader* shader) override;
    void BindVertexBuffer(VertexBuffer* buffer) override;
    void BindIndexBuffer(IndexBuffer* buffer) override;
    void BindTexture(Texture* texture, uint32 slot) override;

    void Draw(PrimitiveType type, uint32 vertexCount, uint32 startVertex) override;
    void DrawIndexed(PrimitiveType type, uint32 indexCount, uint32 startIndex) override;

    void SetDepthTest(bool enabled) override;
    void SetBlend(bool enabled) override;
    void SetCullFace(bool enabled) override;

private:
    GLenum ToOGLPrimitiveType(PrimitiveType type);
    GLenum ToOGLUsage(BufferUsage usage);
};

} // namespace TLETC::Render