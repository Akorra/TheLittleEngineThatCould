// TLETC/Renderer/Mesh.h
#pragma once

#include "TLETC/Core/Types.h"
#include "TLETC/Core/Math.h"
#include "TLETC/Render/Buffer.h"
#include <vector>

namespace TLETC::Render
{

class Mesh
{
public:
    Mesh() = default;
    ~Mesh() = default;

    // Vertex data management
    void AddVertex(const vec3& position = vec3(0.0f), const vec3& normal = vec3(0.0f, 1.0f, 0.0f), const vec2& uv = vec2(0.0f), const vec4& color = vec4(1.0f));

    void SetVertexPosition(const size_t index, const vec3& position);
    void SetVertexNormal(const size_t index, const vec3& normal);
    void SetVertexUV(const size_t index, const vec2& uv);
    void SetVertexColor(const size_t index, const vec4& color);

    const vec3& GetVertexPosition(const size_t index) const { return positions_[index]; }
    const vec3& GetVertexNormal(const size_t index)   const { return normals_[index]; }
    const vec2& GetVertexUV(const size_t index)       const { return uvs_[index]; }
    const vec4& GetVertexColor(const size_t index)    const { return colors_[index]; }

    void SetVertexPositions(const std::vector<vec3>& vertices) { positions_ = vertices; }
    void SetVertexNormals(const std::vector<vec3>& normals)    { normals_ = normals; }
    void SetVertexUVs(const std::vector<vec2>& uvs)            { uvs_ = uvs; }
    void SetVertexColors(const std::vector<vec4>& colors)      { colors_ = colors; }

    const std::vector<vec3>& GetVertexPositions() const { return positions_; }
    const std::vector<vec3>& GetVertexNormals()   const { return normals_; }
    const std::vector<vec2>& GetVertexUVs()       const { return uvs_; }
    const std::vector<vec4>& GetVertexColors()    const { return colors_; }

    std::vector<vec3>& GetVertexPositions() { return positions_; }
    std::vector<vec3>& GetVertexNormals()   { return normals_; }
    std::vector<vec2>& GetVertexUVs()       { return uvs_; }
    std::vector<vec4>& GetVertexColor()     { return colors_; }

    // Index data management
    void AddIndex(uint32 index);
    void AddIndices(const std::vector<uint32>& indices);
    void AddTriangle(uint32 i0, uint32 i1, uint32 i2);
    void SetIndices(const std::vector<uint32>& indices) { indices_ = indices;}
    const std::vector<uint32>& GetIndices() const       { return indices_; }
    std::vector<uint32>& GetIndices()                   { return indices_; }

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

protected:
    // vertex data
    std::vector<vec3>   positions_;
    std::vector<vec3>   normals_;
    std::vector<vec2>   uvs_; 
    std::vector<vec4>   colors_;

    // mesh indices
    std::vector<uint32> indices_;
};

} // namespace TLETC::Render