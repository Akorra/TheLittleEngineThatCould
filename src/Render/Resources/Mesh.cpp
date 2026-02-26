#include "TLETC/Render/Resources/Mesh.h"

#include "TLETC/Core/Math.h"
#include "TLETC/Core/Assert.h"

#include <algorithm>

namespace TLETC::Render
{

void Mesh::AddVertex(const vec3& position, const vec3& normal, const vec2& uv, const vec4& color) 
{
    positions_.push_back(position);
    normals_.push_back(normal);
    uvs_.push_back(uv);
    colors_.push_back(color);
}

void Mesh::SetVertexPosition(const size_t index, const vec3 &position)
{
    TLETC_ASSERT(index < positions_.size(), "Vertex index does not exist!");
    positions_[index] = position;
}

void Mesh::SetVertexNormal(const size_t index, const vec3 &normal)
{
    TLETC_ASSERT(index < normals_.size(), "Vertex index does not exist!");
    normals_[index] =normal;
}

void Mesh::SetVertexUV(const size_t index, const vec2 &uv)
{
    TLETC_ASSERT(index < uvs_.size(), "Vertex index does not exist!");
    uvs_[index] = uv;
}

void Mesh::SetVertexColor(const size_t index, const vec4 &color)
{
    TLETC_ASSERT(index < colors_.size(), "Vertex index does not exist!");
    colors_[index] = color;
}

void Mesh::AddIndex(uint32 index) 
{
    indices_.push_back(index);
}

void Mesh::AddIndices(const std::vector<uint32>& indices) 
{
    indices_.insert(indices_.end(), indices.begin(), indices.end());
}

void Mesh::AddTriangle(uint32 i0, uint32 i1, uint32 i2) 
{
    indices_.push_back(i0);
    indices_.push_back(i1);
    indices_.push_back(i2);
}

void Mesh::Clear() 
{
    positions_.clear();
    normals_.clear();
    uvs_.clear();
    colors_.clear();
    indices_.clear();
}

void Mesh::Reserve(size_t vertexCount, size_t indexCount) 
{
    positions_.reserve(vertexCount);
    normals_.reserve(vertexCount);
    uvs_.reserve(vertexCount);
    colors_.reserve(vertexCount);
    
    if (indexCount > 0)
        indices_.reserve(indexCount);
}

BoundingBox Mesh::CalculateBoundingBox() const 
{
    if (positions_.empty())
        return BoundingBox();
    
    vec3 pmin = positions_[0];
    vec3 pmax = positions_[0];
    
    for (const auto& vertex : positions_) 
    {
        pmin = min(pmin, vertex);
        pmax = max(pmax, vertex);
    }
    
    return BoundingBox(pmin, pmax);
}

void Mesh::RecalculateNormals() 
{
    // Reset all normals to zero
    normals_.clear();
    normals_.resize(positions_.size(), glm::vec3(0.0f));
    
    // Calculate face normals and accumulate
    if (IsIndexed()) 
    {
        for (size_t i = 0; i < indices_.size(); i += 3) 
        {
            uint32 i0 = indices_[i];
            uint32 i1 = indices_[i + 1];
            uint32 i2 = indices_[i + 2];
            
            vec3 v0 = positions_[i0];
            vec3 v1 = positions_[i1];
            vec3 v2 = positions_[i2];
            
            vec3 edge1 = v1 - v0;
            vec3 edge2 = v2 - v0;
            vec3 normal = normalize(cross(edge1, edge2));
            
            normals_[i0] += normal;
            normals_[i1] += normal;
            normals_[i2] += normal;
        }
    } 
    else 
    {
        for (size_t i = 0; i < positions_.size(); i += 3) 
        {
            vec3 v0 = positions_[i];
            vec3 v1 = positions_[i + 1];
            vec3 v2 = positions_[i + 2];
            
            vec3 edge1 = v1 - v0;
            vec3 edge2 = v2 - v0;
            vec3 normal = normalize(cross(edge1, edge2));
            
            normals_[i]     += normal;
            normals_[i + 1] += normal;
            normals_[i + 2] += normal;
        }
    }
    
    // Normalize all normals
    for (size_t i=0; i<normals_.size(); ++i)
        normals_[i] = normalize(normals_[i]);
}

void Mesh::RecalculateTangents() 
{
    // TODO: Implement tangent calculation for normal mapping
}

} // namespace TLETC::Render
