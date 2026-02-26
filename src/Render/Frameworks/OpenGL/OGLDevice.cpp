#include "TLETC/Render/Frameworks/OpenGL/OGLDevice.h"

#include "TLETC/Render/Frameworks/OpenGL/OGLBuffer.h"
#include "TLETC/Render/Frameworks/OpenGL/OGLShader.h"
#include "TLETC/Render/Frameworks/OpenGL/OGLTexture.h"

#include "TLETC/Core/Log.h"

namespace TLETC::Render
{

bool OGLDevice::Initialize()
{
    // GLAD should already be initialized by Window
    TLETC_INFO("OpenGL Renderer initialized");
    TLETC_INFO("  Vendor: ", glGetString(GL_VENDOR));
    TLETC_INFO("  Renderer: ", glGetString(GL_RENDERER));
    TLETC_INFO("  Version: ", glGetString(GL_VERSION));
    
    // Enable depth test by default
    SetDepthTest(true);
    SetCullFace(true);
    
    return true;
}

void OGLDevice::Shutdown()
{
    TLETC_INFO("OpenGL Renderer shutdown");
}

void OGLDevice::BeginFrame()
{
    // Nothing needed for OpenGL
}

void OGLDevice::EndFrame()
{
    // Swap buffers handled by Window
}

void OGLDevice::Clear(const vec4& color)
{
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OGLDevice::SetViewport(uint32 x, uint32 y, uint32 width, uint32 height)
{
    glViewport(x, y, width, height);
}

VertexBuffer* OGLDevice::CreateVertexBuffer(const void* data, size_t size, BufferUsage usage)
{
    return new OGLVertexBuffer(data, size, usage);
}

IndexBuffer* OGLDevice::CreateIndexBuffer(const uint32* data, size_t count, BufferUsage usage)
{
    return new OGLIndexBuffer(data, count, usage);
}

Shader* OGLDevice::CreateShader(const char* vertexSrc, const char* fragmentSrc)
{
    return new OGLShader(vertexSrc, fragmentSrc);
}

Texture* OGLDevice::CreateTexture(const void* data, uint32 width, uint32 height)
{
    // TODO:
    // return new OGLTexture(data, width, height);
}

void OGLDevice::DestroyVertexBuffer(VertexBuffer* buffer)
{
    delete buffer;
}

void OGLDevice::DestroyIndexBuffer(IndexBuffer* buffer)
{
    delete buffer;
}

void OGLDevice::DestroyShader(Shader* shader)
{
    delete shader;
}

void OGLDevice::DestroyTexture(Texture* texture)
{
    delete texture;
}

void OGLDevice::BindShader(Shader* shader)
{
    if (shader)
        shader->Bind();
}

void OGLDevice::BindVertexBuffer(VertexBuffer* buffer)
{
    if (buffer)
        buffer->Bind();
}

void OGLDevice::BindIndexBuffer(IndexBuffer* buffer)
{
    if (buffer)
        buffer->Bind();
}

void OGLDevice::BindTexture(Texture* texture, uint32 slot)
{
    if (texture)
    {
        // TODO:
        // glActiveTexture(GL_TEXTURE0 + slot);
        // texture->Bind();
    }
}

void OGLDevice::Draw(PrimitiveType type, uint32 vertexCount, uint32 startVertex)
{
    glDrawArrays(ToOGLPrimitiveType(type), startVertex, vertexCount);
}

void OGLDevice::DrawIndexed(PrimitiveType type, uint32 indexCount, uint32 startIndex)
{
    glDrawElements(ToOGLPrimitiveType(type), indexCount, GL_UNSIGNED_INT,
                   (void*)(startIndex * sizeof(uint32)));
}

void OGLDevice::SetDepthTest(bool enabled)
{
    if (enabled)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
}

void OGLDevice::SetBlend(bool enabled)
{
    if (enabled)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    else
    {
        glDisable(GL_BLEND);
    }
}

void OGLDevice::SetCullFace(bool enabled)
{
    if (enabled)
    {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }
    else
    {
        glDisable(GL_CULL_FACE);
    }
}

GLenum OGLDevice::ToOGLPrimitiveType(PrimitiveType type)
{
    switch (type)
    {
        case PrimitiveType::Triangles: return GL_TRIANGLES;
        case PrimitiveType::Lines:     return GL_LINES;
        case PrimitiveType::Points:    return GL_POINTS;
    }
    return GL_TRIANGLES;
}

GLenum OGLDevice::ToOGLUsage(BufferUsage usage)
{
    switch (usage)
    {
        case BufferUsage::Static:  return GL_STATIC_DRAW;
        case BufferUsage::Dynamic: return GL_DYNAMIC_DRAW;
        case BufferUsage::Stream:  return GL_STREAM_DRAW;
    }
    return GL_STATIC_DRAW;
}

} // namespace TLETC::Render