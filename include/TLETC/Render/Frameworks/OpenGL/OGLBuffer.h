#pragma once

#include "TLETC/Render/Buffer.h"
#include <glad/gl.h>

namespace TLETC::Render
{

class OGLVertexBuffer : public VertexBuffer
{
public:
    OGLVertexBuffer(const void* data, size_t size, BufferUsage usage);
    ~OGLVertexBuffer() override;

    void Bind() const override;
    void Unbind() const override;
    void SetData(const void* data, size_t size) override;

private:
    GLuint vbo_ = 0;
    GLuint vao_ = 0;
    GLenum usage_;
};

class OGLIndexBuffer : public IndexBuffer
{
public:
    OGLIndexBuffer(const uint32* data, size_t count, BufferUsage usage);
    ~OGLIndexBuffer() override;

    void Bind() const override;
    void Unbind() const override;
    void SetData(const uint32* data, size_t count) override;

private:
    GLuint ibo_ = 0;
    GLenum usage_;
};

} // namespace TLETC::Render