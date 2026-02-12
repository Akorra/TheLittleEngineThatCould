#pragma once

#include <cstdint>
#include <memory>

// =======================================================
// Standard Type Aliases
// =======================================================

namespace TLETC
{

// Integer Types
using int8   = std::int8_t;
using int16  = std::int16_t;
using int32  = std::int32_t;
using int64  = std::int64_t;

using uint8  = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;

using usize  = std::size_t;

// Floating Point
using float32 = float;
using float64 = double;

// =======================================================
// Smart Pointer Aliases
// =======================================================

template<typename T>
using UniquePtr = std::unique_ptr<T>;

template<typename T>
using SharedPtr = std::shared_ptr<T>;

template<typename T>
using WeakPtr = std::weak_ptr<T>;

// Factory Functions
template<typename T, typename... Args>
inline UniquePtr<T> MakeUnique(Args&&... args)
{
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T, typename... Args>
inline SharedPtr<T> MakeShared(Args&&... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

// =======================================================
// Handle Types (for resource management)
// =======================================================

template<typename T>
struct Handle
{
    uint32 index      = UINT32_MAX;
    uint32 generation = 0;
    
    bool IsValid() const { return index != UINT32_MAX; }
    
    bool operator==(const Handle& other) const 
    { 
        return index == other.index && generation == other.generation; 
    }
    
    bool operator!=(const Handle& other) const 
    { 
        return !(*this == other); 
    }
    
    explicit operator bool() const { return IsValid(); }
};

// Common handle types
struct Texture;
struct Mesh;
struct Material;
struct Shader;
struct Buffer;

using TextureHandle  = Handle<Texture>;
using MeshHandle     = Handle<Mesh>;
using MaterialHandle = Handle<Material>;
using ShaderHandle   = Handle<Shader>;
using BufferHandle   = Handle<Buffer>;

} // namespace TLETC

// =======================================================
// Hash Support for Handles (for unordered_map/set)
// =======================================================

namespace std
{
    template<typename T>
    struct hash<TLETC::Handle<T>>
    {
        size_t operator()(const TLETC::Handle<T>& handle) const noexcept
        {
            // Combine index and generation
            size_t h1 = std::hash<TLETC::uint32>{}(handle.index);
            size_t h2 = std::hash<TLETC::uint32>{}(handle.generation);
            return h1 ^ (h2 << 1);
        }
    };
}