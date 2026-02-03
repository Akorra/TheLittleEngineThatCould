#pragma once

#include "TLETC/Core/Types.h"
#include "TLETC/Core/Math.h"

#include <vector>

namespace TLETC 
{
// Mesh class - holds geometry data
class Mesh 
{
public:
    Mesh();
    ~Mesh();

    // Vertex data management
    void AddVertex(const Vec3& position = Vec3(0.0f), const Vec3& normal = Vec3(0.0f, 1.0f, 0.0f), const Vec2& uv = Vec2(0.0f), const Vec4& color = Vec4(1.0f));

    void SetVertexPosition(const size_t vId, const Vec3& position);
    void SetVertexNormal(const size_t vId, const Vec3& normal);
    void SetVertexUV(const size_t vId, const Vec2& uv);
    void SetVertexColor(const size_t vId, const Vec4& color);

    void SetVertexPositions(const std::vector<Vec3>& positions);
    void SetVertexNormals(const std::vector<Vec3>& normals);
    void SetVertexUVs(const std::vector<Vec2>& uvs);
    void SetVertexColors(const std::vector<Vec4>& colors);

    const Vec3& GetVertexPosition(const size_t vId) const { return positions_[vId]; }
    const Vec3& GetVertexNormal(const size_t vId)   const { return normals_[vId]; }
    const Vec2& GetVertexUV(const size_t vId)       const { return uvs_[vId]; }
    const Vec4& GetVertexColor(const size_t vId)    const { return colors_[vId]; }

    const std::vector<Vec3>& GetVertexPositions() const { return positions_; }
    const std::vector<Vec3>& GetVertexNormals()   const { return normals_; }
    const std::vector<Vec2>& GetVertexUVs()       const { return uvs_; }
    const std::vector<Vec4>& GetVertexColors()    const { return colors_; }

    std::vector<Vec3>& GetVertexPositions() { return positions_; }
    std::vector<Vec3>& GetVertexNormals()   { return normals_; }
    std::vector<Vec2>& GetVertexUVs()       { return uvs_; }
    std::vector<Vec4>& GetVertexColors()    { return colors_; }
    
    // Index data management
    void AddIndex(uint32 index);
    void AddIndices(const std::vector<uint32>& indices);
    void AddTriangle(uint32 i0, uint32 i1, uint32 i2);
    
    void SetIndices(const std::vector<uint32>& indices);
    const std::vector<uint32>& GetIndices() const { return indices_; }
    std::vector<uint32>& GetIndices() { return indices_; }
    
    // Queries
    size_t GetVertexCount() const   { return positions_.size(); }
    size_t GetIndexCount() const    { return indices_.size(); }
    size_t GetTriangleCount() const { return indices_.size() / 3; }
    
    bool IsEmpty()   const { return positions_.empty(); }
    bool IsIndexed() const { return !indices_.empty(); }
    
    // Utility
    void Clear();
    void Reserve(size_t vertexCount, size_t indexCount = 0);
    
    // Calculate mesh properties
    BoundingBox CalculateBoundingBox() const;
    void RecalculateNormals();
    void RecalculateTangents();
    
    // Transformation
    void Transform(const Mat4& transform);
    void Translate(const Vec3& offset);
    void Scale(const Vec3& scale);
    void Rotate(const Quat& rotation);

protected:
    // vertex data
    std::vector<Vec3> positions_;
    std::vector<Vec3> normals_;
    std::vector<Vec2> uvs_;
    std::vector<Vec4> colors_;

    // mesh indices
    std::vector<uint32> indices_;
};

// ============================================================================
// Railroad-Themed Aliases 
// ============================================================================

// The station is where you observe the train (window shows the game)
using Blueprint = Mesh;

};