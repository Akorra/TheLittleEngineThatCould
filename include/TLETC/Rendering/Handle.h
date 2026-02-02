#pragma once

#include "TLETC/Core/Types.h"

namespace TLETC 
{
// Base handle class
template<typename Tag>
class Handle 
{
public:
    Handle() : id_(0) {}
    explicit Handle(uint32 id) : id_(id) {}
    
    bool IsValid() const { return id_ != 0; }
    uint32 GetID() const { return id_; }
    void Reset() { id_ = 0; }
    
    bool operator==(const Handle& other) const { return id_ == other.id_; }
    bool operator!=(const Handle& other) const { return id_ != other.id_; }
    bool operator<(const Handle& other) const { return id_ < other.id_; }
    
private:
    uint32 id_;
};

// Tag types for type safety
struct BufferTag {};
struct ShaderTag {};
struct TextureTag {};
struct PipelineTag {};

// Specific handle types
using BufferHandle = Handle<BufferTag>;
using ShaderHandle = Handle<ShaderTag>;
using TextureHandle = Handle<TextureTag>;
using PipelineHandle = Handle<PipelineTag>;

} // namespace TLETC
