// TLETC/Renderer/Mesh.h
#pragma once

#include "TLETC/Core/Types.h"
#include "TLETC/Core/Math.h"
#include "TLETC/Render/Buffer.h"
#include <vector>

namespace TLETC::Renderer
{

class Mesh
{
public:
    Mesh() = default;
    ~Mesh() = default;

    void SetVertexPositions(const std::vector<vec3>& vertices) { positions_ = vertices; }
    void SetVertexNormals(const std::vector<vec3>& normals)    { normals_ = normals; }
    void SetVertexUVs(const std::vector<vec2>& uvs)            { uvs_ = uvs; }
    void SetVertexColor(const std::vector<vec4>& colors)       { colors_ = colors; }
    void SetIndices(const std::vector<uint32>& indices)        { indices_ = indices;}

    const std::vector<vec3>& GetVertexPositions() const { return positions_; }
    const std::vector<vec3>& GetVertexNormals()   const { return normals_; }
    const std::vector<vec2>& GetVertexUVs()       const { return uvs_; }
    const std::vector<vec4>& GetVertexColor()     const { return colors_; }
    const std::vector<uint32>& GetIndices()       const { return indices_; }

    std::vector<vec3>& GetVertexPositions() { return positions_; }
    std::vector<vec3>& GetVertexNormals()   { return normals_; }
    std::vector<vec2>& GetVertexUVs()       { return uvs_; }
    std::vector<vec4>& GetVertexColor()     { return colors_; }
    std::vector<uint32>& GetIndices()       { return indices_; }

private:
    std::vector<vec3>   positions_;
    std::vector<vec3>   normals_;
    std::vector<vec2>   uvs_; 
    std::vector<vec4>   colors_;
    std::vector<uint32> indices_;
};

} // namespace TLETC::Renderer