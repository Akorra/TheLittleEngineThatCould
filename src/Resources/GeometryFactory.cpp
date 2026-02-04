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
        
        for (int v = 0; v < 4; ++v) 
            mesh.AddVertex(vertices[faces[f].indices[v]], faces[f].normal, uvs[v], Vec4(1.0f));
        
        // Add two triangles for this face
        mesh.AddTriangle(baseIndex + 0, baseIndex + 2, baseIndex + 1);
        mesh.AddTriangle(baseIndex + 0, baseIndex + 3, baseIndex + 2);
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
            
            Vec3 position(0.0f);
            position.x = radius * std::sin(phi) * std::cos(theta);
            position.y = radius * std::cos(phi);
            position.z = radius * std::sin(phi) * std::sin(theta);
            
            mesh.AddVertex(position, normalize(position), Vec2(u, v), Vec4(1.0f));
        }
    }
    
    // Generate indices
    for (uint32 ring = 0; ring < rings; ++ring) {
        for (uint32 segment = 0; segment < segments; ++segment) {
            uint32 current = ring * (segments + 1) + segment;
            uint32 next = current + segments + 1;
            
            mesh.AddTriangle(current, current + 1, next);
            mesh.AddTriangle(current + 1, next + 1, next);
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
            
            mesh.AddVertex(Vec3(xPos, 0.0f, zPos), Vec3(0.0f, 1.0f, 0.0f), Vec2(u, v), Vec4(1.0f));
        }
    }
    
    // Generate indices
    for (uint32 z = 0; z < heightSegments; ++z) {
        for (uint32 x = 0; x < widthSegments; ++x) {
            uint32 topLeft = z * (widthSegments + 1) + x;
            uint32 topRight = topLeft + 1;
            uint32 bottomLeft = (z + 1) * (widthSegments + 1) + x;
            uint32 bottomRight = bottomLeft + 1;
            
            mesh.AddTriangle(topLeft,  bottomLeft, topRight);
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
    
    mesh.AddVertex(Vec3(0.0f, halfHeight, 0.0f), Vec3(0.0f, 1.0f, 0.0f), Vec2(0.5f, 0.5f));
    mesh.AddVertex(Vec3(0.0f, -halfHeight, 0.0f), Vec3(0.0f, -1.0f, 0.0f), Vec2(0.5f, 0.5f));
    
    // Generate side vertices
    uint32 sideTopStart = static_cast<uint32>(mesh.GetVertexCount());
    for (uint32 i = 0; i <= segments; ++i) {
        float u = static_cast<float>(i) / static_cast<float>(segments);
        float theta = u * TWO_PI;
        
        float x = radius * std::cos(theta);
        float z = radius * std::sin(theta);
        
        Vec3 normal = normalize(Vec3(x, 0.0f, z));
        
        // Top vertex
        mesh.AddVertex(Vec3(x, halfHeight, z), normal, Vec2(u, 1.0f));
        
        // Bottom vertex
        mesh.AddVertex(Vec3(x, -halfHeight, z), normal, Vec2(u, 0.0f));
    }
    
    uint32 sideBottomStart = sideTopStart + 1;
    
    // Generate cap vertices
    uint32 topCapStart = static_cast<uint32>(mesh.GetVertexCount());
    for (uint32 i = 0; i <= segments; ++i) {
        float u = static_cast<float>(i) / static_cast<float>(segments);
        float theta = u * TWO_PI;
        
        float x = radius * std::cos(theta);
        float z = radius * std::sin(theta);
        
        mesh.AddVertex(Vec3(x, halfHeight, z), Vec3(0.0f, 1.0f, 0.0f), Vec2(x / radius * 0.5f + 0.5f, z / radius * 0.5f + 0.5f));
    }
    
    uint32 bottomCapStart = static_cast<uint32>(mesh.GetVertexCount());
    for (uint32 i = 0; i <= segments; ++i) {
        float u = static_cast<float>(i) / static_cast<float>(segments);
        float theta = u * TWO_PI;
        
        float x = radius * std::cos(theta);
        float z = radius * std::sin(theta);
        
        mesh.AddVertex(Vec3(x, -halfHeight, z), Vec3(0.0f, -1.0f, 0.0f), Vec2(x / radius * 0.5f + 0.5f, z / radius * 0.5f + 0.5f));
    }
    
    // Generate side indices
    for (uint32 i = 0; i < segments; ++i) {
        uint32 top1 = sideTopStart + i * 2;
        uint32 bottom1 = sideBottomStart + i * 2;
        uint32 top2 = sideTopStart + (i + 1) * 2;
        uint32 bottom2 = sideBottomStart + (i + 1) * 2;
        
        mesh.AddTriangle(top1, top2, bottom1);
        mesh.AddTriangle(top2, bottom2, bottom1);
    }
    
    // Generate cap indices
    for (uint32 i = 0; i < segments; ++i) {
        mesh.AddTriangle(topCenter, topCapStart + i + 1, topCapStart + i);
        mesh.AddTriangle(bottomCenter, bottomCapStart + i, bottomCapStart + i + 1);
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
    mesh.AddVertex(Vec3(0.0f, halfHeight, 0.0f), Vec3(0.0f, 1.0f, 0.0f), Vec2(0.5f, 1.0f));
    
    // Base center
    uint32 baseCenter = 1;
    mesh.AddVertex(Vec3(0.0f, -halfHeight, 0.0f), Vec3(0.0f, -1.0f, 0.0f), Vec2(0.5f, 0.5f));
    
    // Side vertices (rim)
    uint32 sideStart = static_cast<uint32>(mesh.GetVertexCount());
    for (uint32 i = 0; i <= segments; ++i) {
        float u = static_cast<float>(i) / static_cast<float>(segments);
        float theta = u * TWO_PI;
        
        float x = radius * std::cos(theta);
        float z = radius * std::sin(theta);
        
        // Calculate normal for cone side
        Vec3 normal = normalize(Vec3(x, radius / height, z));
        mesh.AddVertex(Vec3(x, -halfHeight, z), normal, Vec2(u, 0.0f));
    }
    
    // Base cap vertices
    uint32 capStart = static_cast<uint32>(mesh.GetVertexCount());
    for (uint32 i = 0; i <= segments; ++i) {
        float u = static_cast<float>(i) / static_cast<float>(segments);
        float theta = u * TWO_PI;
        
        float x = radius * std::cos(theta);
        float z = radius * std::sin(theta);
        
        mesh.AddVertex(Vec3(x, -halfHeight, z), Vec3(0.0f, -1.0f, 0.0f), Vec2(x / radius * 0.5f + 0.5f, z / radius * 0.5f + 0.5f));
    }
    
    // Generate side indices (triangles from apex to rim)
    for (uint32 i = 0; i < segments; ++i) {
        mesh.AddTriangle(apexIndex, sideStart + i + 1, sideStart + i);
    }
    
    // Generate base cap indices
    for (uint32 i = 0; i < segments; ++i) {
        mesh.AddTriangle(baseCenter, capStart + i, capStart + i + 1);
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
            
            // Position
            Vec3 position;
            position.x = (majorRadius + minorRadius * cosPhi) * cosTheta;
            position.y = minorRadius * sinPhi;
            position.z = (majorRadius + minorRadius * cosPhi) * sinTheta;
            
            // Normal
            Vec3 center(majorRadius * cosTheta, 0.0f, majorRadius * sinTheta);
            mesh.AddVertex(position, normalize(position - center), Vec2(u, v), Vec4(1.0f));
        }
    }
    
    // Generate indices
    for (uint32 i = 0; i < majorSegments; ++i) {
        for (uint32 j = 0; j < minorSegments; ++j) {
            uint32 current = i * (minorSegments + 1) + j;
            uint32 next = current + minorSegments + 1;
            
            mesh.AddTriangle(current, current + 1, next);
            mesh.AddTriangle(current + 1, next + 1, next);
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
            
            Vec3 position;
            position.x = radius * std::sin(phi) * std::cos(theta);
            position.y = halfCylinderHeight + radius * std::cos(phi);
            position.z = radius * std::sin(phi) * std::sin(theta);
            
            mesh.AddVertex(position, normalize(position - Vec3(0.0f, halfCylinderHeight, 0.0f)), Vec2(u, v * 0.25f + 0.75f), Vec4(1.0f));
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
        mesh.AddVertex(Vec3(x, halfCylinderHeight, z), normal, Vec2(u, 0.75f));
        
        // Bottom of cylinder
        mesh.AddVertex(Vec3(x, -halfCylinderHeight, z), normal, Vec2(u, 0.25f));
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
            
            Vec3 position;
            position.x = radius * std::sin(phi) * std::cos(theta);
            position.y = -halfCylinderHeight + radius * std::cos(phi);
            position.z = radius * std::sin(phi) * std::sin(theta);
            
            mesh.AddVertex(position, normalize(position - Vec3(0.0f, -halfCylinderHeight, 0.0f)), Vec2(u, v * 0.25f), Vec4(1.0f));
        }
    }
    
    // Generate indices for top hemisphere
    for (uint32 ring = 0; ring < rings; ++ring) {
        for (uint32 segment = 0; segment < segments; ++segment) {
            uint32 current = ring * (segments + 1) + segment;
            uint32 next = current + segments + 1;
            
            mesh.AddTriangle(current, current + 1, next);
            mesh.AddTriangle(current + 1, next + 1, next);
        }
    }
    
    // Generate indices for cylinder
    for (uint32 i = 0; i < segments; ++i) {
        uint32 top1 = cylinderStart + i * 2;
        uint32 bottom1 = cylinderStart + i * 2 + 1;
        uint32 top2 = cylinderStart + (i + 1) * 2;
        uint32 bottom2 = cylinderStart + (i + 1) * 2 + 1;
        
        mesh.AddTriangle(top1, top2, bottom1);
        mesh.AddTriangle(top2, bottom2, bottom1);
    }
    
    // Generate indices for bottom hemisphere
    for (uint32 ring = 0; ring < rings; ++ring) {
        for (uint32 segment = 0; segment < segments; ++segment) {
            uint32 current = bottomHemisphereStart + ring * (segments + 1) + segment;
            uint32 next = current + segments + 1;
            
            mesh.AddTriangle(current, current + 1, next);
            mesh.AddTriangle(current + 1, next + 1, next);
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
    for (int i = 0; i < 12; ++i)
        mesh.AddVertex(vertices[i], normalize(vertices[i]), Vec4(1.0f));
    
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
    (void) subdivisions;
    
    return mesh;
}

} // namespace TLETC
