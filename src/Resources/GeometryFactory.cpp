#include "TLETC/Resources/GeometryFactory.h"
#include <cmath>

namespace TLETC {

Mesh GeometryFactory::CreateCube(float size) 
{
    Mesh mesh;
    float halfSize = size * 0.5f;
    
    // Define the 8 vertices of a cube
    Vec3 vertices[8] = {
        Vec3(-halfSize, -halfSize, -halfSize), // 0
        Vec3( halfSize, -halfSize, -halfSize), // 1
        Vec3( halfSize,  halfSize, -halfSize), // 2
        Vec3(-halfSize,  halfSize, -halfSize), // 3
        Vec3(-halfSize, -halfSize,  halfSize), // 4
        Vec3( halfSize, -halfSize,  halfSize), // 5
        Vec3( halfSize,  halfSize,  halfSize), // 6
        Vec3(-halfSize,  halfSize,  halfSize)  // 7
    };
    
    // Define UVs for each face
    Vec2 uvs[4] = {
        Vec2(0.0f, 0.0f),
        Vec2(1.0f, 0.0f),
        Vec2(1.0f, 1.0f),
        Vec2(0.0f, 1.0f)
    };
    
    // Define each face with its normal and vertices
    struct Face {
        Vec3 normal;
        int indices[4];
    };
    
    Face faces[6] = {
        { Vec3( 0.0f,  0.0f, -1.0f), {0, 1, 2, 3} }, // Front
        { Vec3( 0.0f,  0.0f,  1.0f), {5, 4, 7, 6} }, // Back
        { Vec3( 0.0f,  1.0f,  0.0f), {3, 2, 6, 7} }, // Top
        { Vec3( 0.0f, -1.0f,  0.0f), {4, 5, 1, 0} }, // Bottom
        { Vec3( 1.0f,  0.0f,  0.0f), {1, 5, 6, 2} }, // Right
        { Vec3(-1.0f,  0.0f,  0.0f), {4, 0, 3, 7} }  // Left
    };
    
    mesh.Reserve(24, 36); // 6 faces * 4 vertices, 6 faces * 6 indices
    
    // Build each face
    for (int f = 0; f < 6; ++f) {
        uint32 baseIndex = static_cast<uint32>(mesh.GetVertexCount());
        
        for (int v = 0; v < 4; ++v) {
            Vertex vertex;
            vertex.position = vertices[faces[f].indices[v]];
            vertex.normal = faces[f].normal;
            vertex.uv = uvs[v];
            vertex.color = Vec4(1.0f);
            mesh.AddVertex(vertex);
        }
        
        // Add two triangles for this face
        mesh.AddTriangle(baseIndex + 0, baseIndex + 1, baseIndex + 2);
        mesh.AddTriangle(baseIndex + 0, baseIndex + 2, baseIndex + 3);
    }
    
    return mesh;
}

Mesh GeometryFactory::CreateSphere(float radius, uint32 segments, uint32 rings) 
{
    Mesh mesh;
    
    mesh.Reserve((rings + 1) * (segments + 1), rings * segments * 6);
    
    // Generate vertices
    for (uint32 ring = 0; ring <= rings; ++ring) {
        float v = static_cast<float>(ring) / static_cast<float>(rings);
        float phi = v * PI;
        
        for (uint32 segment = 0; segment <= segments; ++segment) {
            float u = static_cast<float>(segment) / static_cast<float>(segments);
            float theta = u * TWO_PI;
            
            Vertex vertex;
            vertex.position.x = radius * std::sin(phi) * std::cos(theta);
            vertex.position.y = radius * std::cos(phi);
            vertex.position.z = radius * std::sin(phi) * std::sin(theta);
            
            vertex.normal = normalize(vertex.position);
            vertex.uv = Vec2(u, v);
            vertex.color = Vec4(1.0f);
            
            mesh.AddVertex(vertex);
        }
    }
    
    // Generate indices
    for (uint32 ring = 0; ring < rings; ++ring) {
        for (uint32 segment = 0; segment < segments; ++segment) {
            uint32 current = ring * (segments + 1) + segment;
            uint32 next = current + segments + 1;
            
            mesh.AddTriangle(current, next, current + 1);
            mesh.AddTriangle(current + 1, next, next + 1);
        }
    }
    
    return mesh;
}

Mesh GeometryFactory::CreatePlane(float width, float height, uint32 widthSegments, uint32 heightSegments) 
{
    Mesh mesh;
    
    float halfWidth = width * 0.5f;
    float halfHeight = height * 0.5f;
    
    uint32 vertexCount = (widthSegments + 1) * (heightSegments + 1);
    uint32 indexCount = widthSegments * heightSegments * 6;
    
    mesh.Reserve(vertexCount, indexCount);
    
    // Generate vertices
    for (uint32 z = 0; z <= heightSegments; ++z) {
        float v = static_cast<float>(z) / static_cast<float>(heightSegments);
        float zPos = -halfHeight + height * v;
        
        for (uint32 x = 0; x <= widthSegments; ++x) {
            float u = static_cast<float>(x) / static_cast<float>(widthSegments);
            float xPos = -halfWidth + width * u;
            
            Vertex vertex;
            vertex.position = Vec3(xPos, 0.0f, zPos);
            vertex.normal = Vec3(0.0f, 1.0f, 0.0f);
            vertex.uv = Vec2(u, v);
            vertex.color = Vec4(1.0f);
            
            mesh.AddVertex(vertex);
        }
    }
    
    // Generate indices
    for (uint32 z = 0; z < heightSegments; ++z) {
        for (uint32 x = 0; x < widthSegments; ++x) {
            uint32 topLeft = z * (widthSegments + 1) + x;
            uint32 topRight = topLeft + 1;
            uint32 bottomLeft = (z + 1) * (widthSegments + 1) + x;
            uint32 bottomRight = bottomLeft + 1;
            
            mesh.AddTriangle(topLeft, bottomLeft, topRight);
            mesh.AddTriangle(topRight, bottomLeft, bottomRight);
        }
    }
    
    return mesh;
}

Mesh GeometryFactory::CreateCylinder(float radius, float height, uint32 segments) 
{
    Mesh mesh;
    
    float halfHeight = height * 0.5f;
    
    // Reserve space
    mesh.Reserve((segments + 1) * 4 + 2, segments * 12);
    
    // Top and bottom center vertices
    uint32 topCenter = 0;
    uint32 bottomCenter = 1;
    
    Vertex topCenterVertex;
    topCenterVertex.position = Vec3(0.0f, halfHeight, 0.0f);
    topCenterVertex.normal = Vec3(0.0f, 1.0f, 0.0f);
    topCenterVertex.uv = Vec2(0.5f, 0.5f);
    mesh.AddVertex(topCenterVertex);
    
    Vertex bottomCenterVertex;
    bottomCenterVertex.position = Vec3(0.0f, -halfHeight, 0.0f);
    bottomCenterVertex.normal = Vec3(0.0f, -1.0f, 0.0f);
    bottomCenterVertex.uv = Vec2(0.5f, 0.5f);
    mesh.AddVertex(bottomCenterVertex);
    
    // Generate side vertices
    uint32 sideTopStart = static_cast<uint32>(mesh.GetVertexCount());
    for (uint32 i = 0; i <= segments; ++i) {
        float u = static_cast<float>(i) / static_cast<float>(segments);
        float theta = u * TWO_PI;
        
        float x = radius * std::cos(theta);
        float z = radius * std::sin(theta);
        
        Vec3 normal = normalize(Vec3(x, 0.0f, z));
        
        // Top vertex
        Vertex topVertex;
        topVertex.position = Vec3(x, halfHeight, z);
        topVertex.normal = normal;
        topVertex.uv = Vec2(u, 1.0f);
        mesh.AddVertex(topVertex);
        
        // Bottom vertex
        Vertex bottomVertex;
        bottomVertex.position = Vec3(x, -halfHeight, z);
        bottomVertex.normal = normal;
        bottomVertex.uv = Vec2(u, 0.0f);
        mesh.AddVertex(bottomVertex);
    }
    
    uint32 sideBottomStart = sideTopStart + 1;
    
    // Generate cap vertices
    uint32 topCapStart = static_cast<uint32>(mesh.GetVertexCount());
    for (uint32 i = 0; i <= segments; ++i) {
        float u = static_cast<float>(i) / static_cast<float>(segments);
        float theta = u * TWO_PI;
        
        float x = radius * std::cos(theta);
        float z = radius * std::sin(theta);
        
        Vertex topCapVertex;
        topCapVertex.position = Vec3(x, halfHeight, z);
        topCapVertex.normal = Vec3(0.0f, 1.0f, 0.0f);
        topCapVertex.uv = Vec2(x / radius * 0.5f + 0.5f, z / radius * 0.5f + 0.5f);
        mesh.AddVertex(topCapVertex);
    }
    
    uint32 bottomCapStart = static_cast<uint32>(mesh.GetVertexCount());
    for (uint32 i = 0; i <= segments; ++i) {
        float u = static_cast<float>(i) / static_cast<float>(segments);
        float theta = u * TWO_PI;
        
        float x = radius * std::cos(theta);
        float z = radius * std::sin(theta);
        
        Vertex bottomCapVertex;
        bottomCapVertex.position = Vec3(x, -halfHeight, z);
        bottomCapVertex.normal = Vec3(0.0f, -1.0f, 0.0f);
        bottomCapVertex.uv = Vec2(x / radius * 0.5f + 0.5f, z / radius * 0.5f + 0.5f);
        mesh.AddVertex(bottomCapVertex);
    }
    
    // Generate side indices
    for (uint32 i = 0; i < segments; ++i) {
        uint32 top1 = sideTopStart + i * 2;
        uint32 bottom1 = sideBottomStart + i * 2;
        uint32 top2 = sideTopStart + (i + 1) * 2;
        uint32 bottom2 = sideBottomStart + (i + 1) * 2;
        
        mesh.AddTriangle(top1, bottom1, top2);
        mesh.AddTriangle(top2, bottom1, bottom2);
    }
    
    // Generate cap indices
    for (uint32 i = 0; i < segments; ++i) {
        mesh.AddTriangle(topCenter, topCapStart + i, topCapStart + i + 1);
        mesh.AddTriangle(bottomCenter, bottomCapStart + i + 1, bottomCapStart + i);
    }
    
    return mesh;
}

Mesh GeometryFactory::CreateCone(float radius, float height, uint32 segments) 
{
    Mesh mesh;
    
    float halfHeight = height * 0.5f;
    
    // Reserve space for apex, base center, rim vertices, and base cap
    mesh.Reserve(segments + 3, segments * 6);
    
    // Apex at top
    uint32 apexIndex = 0;
    Vertex apex;
    apex.position = Vec3(0.0f, halfHeight, 0.0f);
    apex.normal = Vec3(0.0f, 1.0f, 0.0f);
    apex.uv = Vec2(0.5f, 1.0f);
    mesh.AddVertex(apex);
    
    // Base center
    uint32 baseCenter = 1;
    Vertex baseCenterVertex;
    baseCenterVertex.position = Vec3(0.0f, -halfHeight, 0.0f);
    baseCenterVertex.normal = Vec3(0.0f, -1.0f, 0.0f);
    baseCenterVertex.uv = Vec2(0.5f, 0.5f);
    mesh.AddVertex(baseCenterVertex);
    
    // Side vertices (rim)
    uint32 sideStart = static_cast<uint32>(mesh.GetVertexCount());
    for (uint32 i = 0; i <= segments; ++i) {
        float u = static_cast<float>(i) / static_cast<float>(segments);
        float theta = u * TWO_PI;
        
        float x = radius * std::cos(theta);
        float z = radius * std::sin(theta);
        
        // Calculate normal for cone side
        Vec3 normal = normalize(Vec3(x, radius / height, z));
        
        Vertex sideVertex;
        sideVertex.position = Vec3(x, -halfHeight, z);
        sideVertex.normal = normal;
        sideVertex.uv = Vec2(u, 0.0f);
        mesh.AddVertex(sideVertex);
    }
    
    // Base cap vertices
    uint32 capStart = static_cast<uint32>(mesh.GetVertexCount());
    for (uint32 i = 0; i <= segments; ++i) {
        float u = static_cast<float>(i) / static_cast<float>(segments);
        float theta = u * TWO_PI;
        
        float x = radius * std::cos(theta);
        float z = radius * std::sin(theta);
        
        Vertex capVertex;
        capVertex.position = Vec3(x, -halfHeight, z);
        capVertex.normal = Vec3(0.0f, -1.0f, 0.0f);
        capVertex.uv = Vec2(x / radius * 0.5f + 0.5f, z / radius * 0.5f + 0.5f);
        mesh.AddVertex(capVertex);
    }
    
    // Generate side indices (triangles from apex to rim)
    for (uint32 i = 0; i < segments; ++i) {
        mesh.AddTriangle(apexIndex, sideStart + i, sideStart + i + 1);
    }
    
    // Generate base cap indices
    for (uint32 i = 0; i < segments; ++i) {
        mesh.AddTriangle(baseCenter, capStart + i + 1, capStart + i);
    }
    
    return mesh;
}

Mesh GeometryFactory::CreateTorus(float majorRadius, float minorRadius, uint32 majorSegments, uint32 minorSegments) 
{
    Mesh mesh;
    
    mesh.Reserve((majorSegments + 1) * (minorSegments + 1), majorSegments * minorSegments * 6);
    
    // Generate vertices
    for (uint32 i = 0; i <= majorSegments; ++i) {
        float u = static_cast<float>(i) / static_cast<float>(majorSegments);
        float theta = u * TWO_PI;
        
        float cosTheta = std::cos(theta);
        float sinTheta = std::sin(theta);
        
        for (uint32 j = 0; j <= minorSegments; ++j) {
            float v = static_cast<float>(j) / static_cast<float>(minorSegments);
            float phi = v * TWO_PI;
            
            float cosPhi = std::cos(phi);
            float sinPhi = std::sin(phi);
            
            Vertex vertex;
            
            // Position
            vertex.position.x = (majorRadius + minorRadius * cosPhi) * cosTheta;
            vertex.position.y = minorRadius * sinPhi;
            vertex.position.z = (majorRadius + minorRadius * cosPhi) * sinTheta;
            
            // Normal
            Vec3 center(majorRadius * cosTheta, 0.0f, majorRadius * sinTheta);
            vertex.normal = normalize(vertex.position - center);
            
            // UV
            vertex.uv = Vec2(u, v);
            vertex.color = Vec4(1.0f);
            
            mesh.AddVertex(vertex);
        }
    }
    
    // Generate indices
    for (uint32 i = 0; i < majorSegments; ++i) {
        for (uint32 j = 0; j < minorSegments; ++j) {
            uint32 current = i * (minorSegments + 1) + j;
            uint32 next = current + minorSegments + 1;
            
            mesh.AddTriangle(current, next, current + 1);
            mesh.AddTriangle(current + 1, next, next + 1);
        }
    }
    
    return mesh;
}

Mesh GeometryFactory::CreateQuad(float width, float height) 
{
    return CreatePlane(width, height, 1, 1);
}

Mesh GeometryFactory::CreateCapsule(float radius, float height, uint32 segments, uint32 rings) 
{
    Mesh mesh;
    
    float cylinderHeight = height;
    float halfCylinderHeight = cylinderHeight * 0.5f;
    
    // Reserve approximate space
    mesh.Reserve((segments + 1) * (rings * 2 + 2), segments * (rings * 2 + 2) * 6);
    
    // Generate top hemisphere
    for (uint32 ring = 0; ring <= rings; ++ring) {
        float v = static_cast<float>(ring) / static_cast<float>(rings);
        float phi = v * HALF_PI; // 0 to PI/2
        
        for (uint32 segment = 0; segment <= segments; ++segment) {
            float u = static_cast<float>(segment) / static_cast<float>(segments);
            float theta = u * TWO_PI;
            
            Vertex vertex;
            vertex.position.x = radius * std::sin(phi) * std::cos(theta);
            vertex.position.y = halfCylinderHeight + radius * std::cos(phi);
            vertex.position.z = radius * std::sin(phi) * std::sin(theta);
            
            vertex.normal = normalize(vertex.position - Vec3(0.0f, halfCylinderHeight, 0.0f));
            vertex.uv = Vec2(u, v * 0.25f + 0.75f);
            vertex.color = Vec4(1.0f);
            
            mesh.AddVertex(vertex);
        }
    }
    
    uint32 topHemisphereVertices = (rings + 1) * (segments + 1);
    
    // Generate cylinder middle section
    for (uint32 segment = 0; segment <= segments; ++segment) {
        float u = static_cast<float>(segment) / static_cast<float>(segments);
        float theta = u * TWO_PI;
        
        float x = radius * std::cos(theta);
        float z = radius * std::sin(theta);
        Vec3 normal = normalize(Vec3(x, 0.0f, z));
        
        // Top of cylinder
        Vertex topVertex;
        topVertex.position = Vec3(x, halfCylinderHeight, z);
        topVertex.normal = normal;
        topVertex.uv = Vec2(u, 0.75f);
        mesh.AddVertex(topVertex);
        
        // Bottom of cylinder
        Vertex bottomVertex;
        bottomVertex.position = Vec3(x, -halfCylinderHeight, z);
        bottomVertex.normal = normal;
        bottomVertex.uv = Vec2(u, 0.25f);
        mesh.AddVertex(bottomVertex);
    }
    
    uint32 cylinderStart = topHemisphereVertices;
    
    // Generate bottom hemisphere
    uint32 bottomHemisphereStart = static_cast<uint32>(mesh.GetVertexCount());
    for (uint32 ring = 0; ring <= rings; ++ring) {
        float v = static_cast<float>(ring) / static_cast<float>(rings);
        float phi = HALF_PI + v * HALF_PI; // PI/2 to PI
        
        for (uint32 segment = 0; segment <= segments; ++segment) {
            float u = static_cast<float>(segment) / static_cast<float>(segments);
            float theta = u * TWO_PI;
            
            Vertex vertex;
            vertex.position.x = radius * std::sin(phi) * std::cos(theta);
            vertex.position.y = -halfCylinderHeight + radius * std::cos(phi);
            vertex.position.z = radius * std::sin(phi) * std::sin(theta);
            
            vertex.normal = normalize(vertex.position - Vec3(0.0f, -halfCylinderHeight, 0.0f));
            vertex.uv = Vec2(u, v * 0.25f);
            vertex.color = Vec4(1.0f);
            
            mesh.AddVertex(vertex);
        }
    }
    
    // Generate indices for top hemisphere
    for (uint32 ring = 0; ring < rings; ++ring) {
        for (uint32 segment = 0; segment < segments; ++segment) {
            uint32 current = ring * (segments + 1) + segment;
            uint32 next = current + segments + 1;
            
            mesh.AddTriangle(current, next, current + 1);
            mesh.AddTriangle(current + 1, next, next + 1);
        }
    }
    
    // Generate indices for cylinder
    for (uint32 i = 0; i < segments; ++i) {
        uint32 top1 = cylinderStart + i * 2;
        uint32 bottom1 = cylinderStart + i * 2 + 1;
        uint32 top2 = cylinderStart + (i + 1) * 2;
        uint32 bottom2 = cylinderStart + (i + 1) * 2 + 1;
        
        mesh.AddTriangle(top1, bottom1, top2);
        mesh.AddTriangle(top2, bottom1, bottom2);
    }
    
    // Generate indices for bottom hemisphere
    for (uint32 ring = 0; ring < rings; ++ring) {
        for (uint32 segment = 0; segment < segments; ++segment) {
            uint32 current = bottomHemisphereStart + ring * (segments + 1) + segment;
            uint32 next = current + segments + 1;
            
            mesh.AddTriangle(current, next, current + 1);
            mesh.AddTriangle(current + 1, next, next + 1);
        }
    }
    
    return mesh;
}

Mesh GeometryFactory::CreateIcosphere(float radius, uint32 subdivisions) 
{
    Mesh mesh;
    
    // Start with icosahedron
    const float t = (1.0f + std::sqrt(5.0f)) / 2.0f;
    
    // 12 vertices of icosahedron
    Vec3 vertices[12] = {
        Vec3(-1,  t,  0), Vec3( 1,  t,  0), Vec3(-1, -t,  0), Vec3( 1, -t,  0),
        Vec3( 0, -1,  t), Vec3( 0,  1,  t), Vec3( 0, -1, -t), Vec3( 0,  1, -t),
        Vec3( t,  0, -1), Vec3( t,  0,  1), Vec3(-t,  0, -1), Vec3(-t,  0,  1)
    };
    
    // Normalize and scale
    for (int i = 0; i < 12; ++i) {
        vertices[i] = normalize(vertices[i]) * radius;
    }
    
    // Add initial vertices
    for (int i = 0; i < 12; ++i) {
        Vertex v;
        v.position = vertices[i];
        v.normal = normalize(vertices[i]);
        v.color = Vec4(1.0f);
        mesh.AddVertex(v);
    }
    
    // 20 faces of icosahedron
    uint32 indices[60] = {
        0, 11, 5,   0, 5, 1,    0, 1, 7,    0, 7, 10,   0, 10, 11,
        1, 5, 9,    5, 11, 4,   11, 10, 2,  10, 7, 6,   7, 1, 8,
        3, 9, 4,    3, 4, 2,    3, 2, 6,    3, 6, 8,    3, 8, 9,
        4, 9, 5,    2, 4, 11,   6, 2, 10,   8, 6, 7,    9, 8, 1
    };
    
    for (int i = 0; i < 60; i += 3) {
        mesh.AddTriangle(indices[i], indices[i+1], indices[i+2]);
    }
    
    // Subdivide (TODO: implement subdivision)
    // Each subdivision replaces each triangle with 4 triangles
    // For now, return the base icosahedron
    
    return mesh;
}

} // namespace TLETC
