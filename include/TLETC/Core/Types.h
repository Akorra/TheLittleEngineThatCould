#pragma once

#include <cstdint>
#include <memory>

namespace TLETC 
{
// ------------------------------------------------------------ //
// Basic type aliases                                           
// ------------------------------------------------------------ //

using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;

using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;

using float32 = float;
using float64 = double;

// ------------------------------------------------------------
// Smart pointer factories (inline = safe in headers)
// [[nodiscard]] = warn if the return value is ignored
// ------------------------------------------------------------

// UniquePtr: Exclusive ownership. Not copyable. Movable (std::move).
// Automatically deletes the object when out of scope.
template <typename T>
using UniquePtr = std::unique_ptr<T>;

// SharedPtr: Shared ownership via reference counting.
// Object is deleted when the last SharedPtr goes out of scope.
template <typename T>
using SharedPtr = std::shared_ptr<T>;

// WeakPtr: Non-owning reference to a SharedPtr-managed object.
// Does not increase reference count. Must use .lock() before access.
template <typename T>
using WeakPtr = std::weak_ptr<T>;

// Arrays (unique only)
template <typename T> using UniqueArr = std::unique_ptr<T[]>;

// Helper to create unique pointers
template<typename T, typename... Args>
[[nodiscard]] inline UniquePtr<T> MakeUnique(Args&&... args) { 
    return std::make_unique<T>(std::forward<Args>(args)...); 
}

// If we need unique arrays:
template <typename T>
[[nodiscard]] inline UniqueArr<T> MakeUniqueArray(std::size_t n) {
    return std::make_unique<T[]>(n);
}

// Helper to create shared pointers
template<typename T, typename... Args>
[[nodiscard]] inline SharedPtr<T> MakeShared(Args&&... args) { 
    return std::make_shared<T>(std::forward<Args>(args)...); 
}

// ------------------------------------------------------------
// Optional: uninitialized allocation (C++20).
// Only compiled if the compiler supports it.
// ------------------------------------------------------------

#if defined(__cpp_lib_smart_ptr_for_overwrite) && __cpp_lib_smart_ptr_for_overwrite >= 202002L
template <typename T>
[[nodiscard]] inline UniquePtr<T> MakeUniqueForOverwrite() {
    return std::make_unique_for_overwrite<T>();
}

template <typename T>
[[nodiscard]] inline UniqueArr<T> MakeUniqueArrayForOverwrite(std::size_t n) {
    return std::make_unique_for_overwrite<T[]>(n);
}

template <typename T>
[[nodiscard]] inline SharedPtr<T> MakeSharedForOverwrite() {
    return std::make_shared_for_overwrite<T>();
}
#endif // __cpp_lib_smart_ptr_for_overwrite


// ------------------------------------------------------------
// C-resource wrapper helper (optional pattern).
// Lets you create clean RAII wrappers for C handles.
// Example usage shown below.
// ------------------------------------------------------------

// Generic unique handle with custom deleter
template <typename T, auto Deleter>
using UniqueHandle = std::unique_ptr<T, std::decay_t<decltype(Deleter)>>;

// Example: FILE* RAII wrapper
struct FileCloser {
    void operator()(FILE* f) const noexcept { if (f) std::fclose(f); }
};
using UniqueFILE = std::unique_ptr<FILE, FileCloser>;

} // namespace TLETC