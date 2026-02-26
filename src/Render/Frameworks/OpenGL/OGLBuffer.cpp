#include "TLETC/Render/Frameworks/OpenGL/OGLBuffer.h"
#include "TLETC/Core/Assert.h"

namespace TLETC::Render
{

// --- OGLVertexBuffer ---

OGLVertexBuffer::OGLVertexBuffer(const void* data, size_t size, BufferUsage usage)
{
    usage_ = (usage == BufferUsage::Static)  ? GL_STATIC_DRAW :
             (usage == BufferUsage::Dynamic) ? GL_DYNAMIC_DRAW :
                                               GL_STREAM_DRAW;

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, size, data, usage_);
}

OGLVertexBuffer::~OGLVertexBuffer()
{
    glDeleteBuffers(1, &vbo_);
    glDeleteVertexArrays(1, &vao_);
}

void OGLVertexBuffer::Bind() const
{
    glBindVertexArray(vao_);
    
    // Setup vertex attributes based on layout
    uint32 index = 0;
    for (auto& attr : layout_)
    {
        glEnableVertexAttribArray(index);
        
        GLenum type = GL_FLOAT;  // TODO: Support other types
        GLint size = 0;
        
        switch (attr.type)
        {
            case VertexAttributeType::Float:  size = 1; break;
            case VertexAttributeType::Float2: size = 2; break;
            case VertexAttributeType::Float3: size = 3; break;
            case VertexAttributeType::Float4: size = 4; break;
            default: TLETC_ASSERT(false, "Unsupported vertex attribute type");
        }
        
        glVertexAttribPointer(index, size, type,
                            attr.normalized ? GL_TRUE : GL_FALSE,
                            stride_, (void*)(uintptr_t)attr.offset);
        index++;
    }
}

void OGLVertexBuffer::Unbind() const
{
    glBindVertexArray(0);
}

void OGLVertexBuffer::SetData(const void* data, size_t size)
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

// --- OGLIndexBuffer ---

OGLIndexBuffer::OGLIndexBuffer(const uint32* data, size_t count, BufferUsage usage)
    : usage_(usage == BufferUsage::Static  ? GL_STATIC_DRAW :
             usage == BufferUsage::Dynamic ? GL_DYNAMIC_DRAW :
                                             GL_STREAM_DRAW)
{
    count_ = count;
    
    glGenBuffers(1, &ibo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32), data, usage_);
}

OGLIndexBuffer::~OGLIndexBuffer()
{
    glDeleteBuffers(1, &ibo_);
}

void OGLIndexBuffer::Bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);
}

void OGLIndexBuffer::Unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void OGLIndexBuffer::SetData(const uint32* data, size_t count)
{
    count_ = count;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, count * sizeof(uint32), data);
}

} // namespace TLETC::Render