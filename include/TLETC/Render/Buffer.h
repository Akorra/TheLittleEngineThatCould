// TLETC/Renderer/Buffer.h
#pragma once

#include "TLETC/Core/Types.h"

#include <vector>

namespace TLETC::Render
{

// Vertex attribute types
enum class VertexAttributeType
{
    Float,
    Float2,
    Float3,
    Float4,
    Int,
    Int2,
    Int3,
    Int4
};

struct VertexAttribute
{
    VertexAttributeType type;
    uint32 offset;
    bool normalized = false;
    
    static uint32 GetSize(VertexAttributeType type)
    {
        switch (type)
        {
            case VertexAttributeType::Float:  return 4;
            case VertexAttributeType::Float2: return 8;
            case VertexAttributeType::Float3: return 12;
            case VertexAttributeType::Float4: return 16;
            case VertexAttributeType::Int:    return 4;
            case VertexAttributeType::Int2:   return 8;
            case VertexAttributeType::Int3:   return 12;
            case VertexAttributeType::Int4:   return 16;
        }
        return 0;
    }
};

/**
 * VertexBuffer
 * Base class for platform-specific implementations
 */
class VertexBuffer
{
public:
    virtual ~VertexBuffer() = default;

    virtual void Bind(uint32 baseAttrbIndex) const = 0;
    virtual void Unbind() const = 0;
    
    virtual void SetData(const void* data, size_t size) = 0;
    
    void SetLayout(const std::vector<VertexAttribute>& attributes)
    {
        layout_ = attributes;
        
        // Calculate stride
        stride_ = 0;
        for (auto& attr : layout_)
            stride_ += VertexAttribute::GetSize(attr.type);
    }
    
    const std::vector<VertexAttribute>& GetLayout() const { return layout_; }
    uint32 GetStride() const { return stride_; }

protected:
    std::vector<VertexAttribute> layout_;
    uint32 stride_ = 0;
};

/**
 * IndexBuffer
 * Base class for platform-specific implementations
 */
class IndexBuffer
{
public:
    virtual ~IndexBuffer() = default;

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;
    
    virtual void SetData(const uint32* data, size_t count) = 0;
    
    size_t GetCount() const { return count_; }

protected:
    size_t count_ = 0;
};

} // namespace TLETC::Render