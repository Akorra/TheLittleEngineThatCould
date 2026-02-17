#pragma once

#include "TLETC/Core/Types.h"
#include <typeindex>
#include <string>

namespace TLETC::ECS
{
/**
 * ComponentTypeID
 * Unique identifier for each component type
 */
using ComponentTypeID = std::type_index;

/**
 * Get type ID for component type T
 */
template<typename T>
ComponentTypeID GetComponentTypeID()
{
    return std::type_index(typeid(T));
}

/**
 * Get type name for debugging
 */
template<typename T>
const char* GetComponentTypeName()
{
    return typeid(T).name();
}

// TODO: Manual type registration for faster lookups

} // namespace TLETC::ECS
