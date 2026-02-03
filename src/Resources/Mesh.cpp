#include "TLETC/Resources/Mesh.h"

#include <algorithm>

namespace TLETC 
{

Mesh::Mesh() 
{ }

Mesh::~Mesh() 
{ }


void Mesh::AddVertex(const Vec3& position, const Vec3& normal, const Vec2& uv, const Vec4& color) 
{
    positions_.push_back(position);
    normals_.push_back(normal);
    uvs_.push_back(uv);
    colors_.push_back(color);
}

void Mesh::SetVertexPosition(const size_t vId, const Vec3& position)
{
    assert(vId < positions_.size());
    positions_[vId] = position;
}

void Mesh::SetVertexNormal(const size_t vId, const Vec3& normal)
{
    assert(vId < normals_.size());
    normals_[vId] = normal;
}
void Mesh::SetVertexUV(const size_t vId, const Vec2& uv)
{
    assert(vId < uvs_.size());
    uvs_[vId] = uv;
}

void Mesh::SetVertexColor(const size_t vId, const Vec4& color)
{
    assert(vId < colors_.size());
    colors_[vId] = color;
}

void Mesh::SetVertexPositions(const std::vector<Vec3>& positions)
{
    positions_ = positions;
}

void Mesh::SetVertexNormals(const std::vector<Vec3>& normals)
{
    normals_ = normals;
}

void Mesh::SetVertexUVs(const std::vector<Vec2>& uvs)
{
    uvs_ = uvs;
}

void Mesh::SetVertexColors(const std::vector<Vec4>& colors)
{
    colors_ = colors;
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

void Mesh::SetIndices(const std::vector<uint32>& indices) 
{
    indices_ = indices;
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
    
    Vec3 min = positions_[0];
    Vec3 max = positions_[0];
    
    for (const auto& vertex : positions_) {
        min.x = std::min(min.x, vertex.x);
        min.y = std::min(min.y, vertex.y);
        min.z = std::min(min.z, vertex.z);
        
        max.x = std::max(max.x, vertex.x);
        max.y = std::max(max.y, vertex.y);
        max.z = std::max(max.z, vertex.z);
    }
    
    return BoundingBox(min, max);
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
            
            Vec3 v0 = positions_[i0];
            Vec3 v1 = positions_[i1];
            Vec3 v2 = positions_[i2];
            
            Vec3 edge1 = v1 - v0;
            Vec3 edge2 = v2 - v0;
            Vec3 normal = normalize(cross(edge1, edge2));
            
            normals_[i0] += normal;
            normals_[i1] += normal;
            normals_[i2] += normal;
        }
    } 
    else 
    {
        for (size_t i = 0; i < positions_.size(); i += 3) 
        {
            Vec3 v0 = positions_[i];
            Vec3 v1 = positions_[i + 1];
            Vec3 v2 = positions_[i + 2];
            
            Vec3 edge1 = v1 - v0;
            Vec3 edge2 = v2 - v0;
            Vec3 normal = normalize(cross(edge1, edge2));
            
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

void Mesh::Transform(const Mat4& transform) 
{
    Mat3 normalMatrix = transpose(inverse(Mat3(transform)));
    for (size_t i = 0; i < positions_.size(); ++i) 
    {
        Vec4 pos = transform * Vec4(positions_[i], 1.0f);
        positions_[i] = Vec3(pos);
        normals_[i]   = normalize(normalMatrix * normals_[i]);
    }
}

void Mesh::Translate(const Vec3& offset) 
{
    for (size_t i = 0; i < positions_.size(); ++i)
        positions_[i] += offset;
}

void Mesh::Scale(const Vec3& scale) 
{
    for (size_t i = 0; i < positions_.size(); ++i)
    {
        positions_[i].x *= scale.x;
        positions_[i].y *= scale.y;
        positions_[i].z *= scale.z;
        normals_[i].x *= scale.x;
        normals_[i].y *= scale.y;
        normals_[i].z *= scale.z;
    }
}

void Mesh::Rotate(const Quat& rotation) 
{
    for (size_t i = 0; i < positions_.size(); ++i)
    {
        positions_[i] = rotation * positions_[i];
        normals_[i]   = rotation * normals_[i];
    }
}

}