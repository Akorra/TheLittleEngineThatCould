#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "TLETC/Resources/Mesh.h"
#include "TLETC/Resources/GeometryFactory.h"

using Catch::Approx;

TEST_CASE("Mesh creation and basic operations", "[mesh][resources]") {
    SECTION("Empty mesh creation") {
        TLETC::Mesh mesh;
        
        REQUIRE(mesh.GetVertexCount() == 0);
        REQUIRE(mesh.GetIndexCount() == 0);
        REQUIRE(mesh.IsEmpty());
        REQUIRE_FALSE(mesh.IsIndexed());
    }
    
    SECTION("Adding vertices with position only") {
        TLETC::Mesh mesh;
        
        mesh.AddVertex(TLETC::Vec3(0.0f, 0.0f, 0.0f));
        mesh.AddVertex(TLETC::Vec3(1.0f, 0.0f, 0.0f));
        mesh.AddVertex(TLETC::Vec3(0.0f, 1.0f, 0.0f));
        
        REQUIRE(mesh.GetVertexCount() == 3);
        REQUIRE_FALSE(mesh.IsEmpty());
        
        auto& vertices = mesh.GetVertexPositions();
        REQUIRE(vertices[0].x == Approx(0.0f));
        REQUIRE(vertices[1].x == Approx(1.0f));
        REQUIRE(vertices[2].y == Approx(1.0f));
    }
    
    SECTION("Adding full vertices") {
        TLETC::Mesh mesh;
        
        auto position = TLETC::Vec3(1.0f, 2.0f, 3.0f);
        auto normal   = TLETC::Vec3(0.0f, 1.0f, 0.0f);
        auto uv       = TLETC::Vec2(0.5f, 0.5f);
        auto color    = TLETC::Vec4(1.0f, 0.0f, 0.0f, 1.0f);
        
        mesh.AddVertex(position, normal, uv, color);
        
        REQUIRE(mesh.GetVertexCount() == 1);
        REQUIRE(mesh.GetVertexPosition(0).x == Approx(1.0f));
        REQUIRE(mesh.GetVertexNormal(0).y == Approx(1.0f));
        REQUIRE(mesh.GetVertexUV(0).x == Approx(0.5f));
        REQUIRE(mesh.GetVertexColor(0).r == Approx(1.0f));
    }
    
    SECTION("Adding indices") {
        TLETC::Mesh mesh;
        
        mesh.AddVertex(TLETC::Vec3(0.0f, 0.0f, 0.0f));
        mesh.AddVertex(TLETC::Vec3(1.0f, 0.0f, 0.0f));
        mesh.AddVertex(TLETC::Vec3(0.0f, 1.0f, 0.0f));
        
        mesh.AddIndex(0);
        mesh.AddIndex(1);
        mesh.AddIndex(2);
        
        REQUIRE(mesh.GetIndexCount() == 3);
        REQUIRE(mesh.IsIndexed());
        REQUIRE(mesh.GetTriangleCount() == 1);
        
        auto& indices = mesh.GetIndices();
        REQUIRE(indices[0] == 0);
        REQUIRE(indices[1] == 1);
        REQUIRE(indices[2] == 2);
    }
    
    SECTION("Adding triangle") {
        TLETC::Mesh mesh;
        
        mesh.AddVertex(TLETC::Vec3(0.0f, 0.0f, 0.0f));
        mesh.AddVertex(TLETC::Vec3(1.0f, 0.0f, 0.0f));
        mesh.AddVertex(TLETC::Vec3(0.0f, 1.0f, 0.0f));
        
        mesh.AddTriangle(0, 1, 2);
        
        REQUIRE(mesh.GetIndexCount() == 3);
        REQUIRE(mesh.GetTriangleCount() == 1);
    }
    
    SECTION("Clear mesh") {
        TLETC::Mesh mesh;
        
        mesh.AddVertex(TLETC::Vec3(0.0f, 0.0f, 0.0f));
        mesh.AddIndex(0);
        
        REQUIRE_FALSE(mesh.IsEmpty());
        
        mesh.Clear();
        
        REQUIRE(mesh.IsEmpty());
        REQUIRE(mesh.GetVertexCount() == 0);
        REQUIRE(mesh.GetIndexCount() == 0);
    }
}

TEST_CASE("Mesh bounding box calculation", "[mesh][resources]") {
    SECTION("Simple bounding box") {
        TLETC::Mesh mesh;
        
        mesh.AddVertex(TLETC::Vec3(-1.0f, -1.0f, -1.0f));
        mesh.AddVertex(TLETC::Vec3( 1.0f,  1.0f,  1.0f));
        mesh.AddVertex(TLETC::Vec3( 0.0f,  0.0f,  0.0f));
        
        auto bounds = mesh.CalculateBoundingBox();
        
        REQUIRE(bounds.min.x == Approx(-1.0f));
        REQUIRE(bounds.min.y == Approx(-1.0f));
        REQUIRE(bounds.min.z == Approx(-1.0f));
        REQUIRE(bounds.max.x == Approx( 1.0f));
        REQUIRE(bounds.max.y == Approx( 1.0f));
        REQUIRE(bounds.max.z == Approx( 1.0f));
    }
    
    SECTION("Bounding box center and size") {
        TLETC::Mesh mesh;
        
        mesh.AddVertex(TLETC::Vec3(-2.0f, -2.0f, -2.0f));
        mesh.AddVertex(TLETC::Vec3( 2.0f,  2.0f,  2.0f));
        
        auto bounds = mesh.CalculateBoundingBox();
        auto center = bounds.GetCenter();
        auto size = bounds.GetSize();
        
        REQUIRE(center.x == Approx(0.0f));
        REQUIRE(center.y == Approx(0.0f));
        REQUIRE(center.z == Approx(0.0f));
        
        REQUIRE(size.x == Approx(4.0f));
        REQUIRE(size.y == Approx(4.0f));
        REQUIRE(size.z == Approx(4.0f));
    }
}

TEST_CASE("Mesh transformations", "[mesh][resources]") {
    SECTION("Translate mesh") {
        TLETC::Mesh mesh;
        
        mesh.AddVertex(TLETC::Vec3(0.0f, 0.0f, 0.0f));
        mesh.AddVertex(TLETC::Vec3(1.0f, 0.0f, 0.0f));
        
        mesh.Translate(TLETC::Vec3(1.0f, 2.0f, 3.0f));
        
        auto& vertices = mesh.GetVertexPositions();
        REQUIRE(vertices[0].x == Approx(1.0f));
        REQUIRE(vertices[0].y == Approx(2.0f));
        REQUIRE(vertices[0].z == Approx(3.0f));
        REQUIRE(vertices[1].x == Approx(2.0f));
    }
    
    SECTION("Scale mesh") {
        TLETC::Mesh mesh;
        
        mesh.AddVertex(TLETC::Vec3(1.0f, 1.0f, 1.0f));
        mesh.AddVertex(TLETC::Vec3(2.0f, 2.0f, 2.0f));
        
        mesh.Scale(TLETC::Vec3(2.0f, 3.0f, 4.0f));
        
        auto& vertices = mesh.GetVertexPositions();
        REQUIRE(vertices[0].x == Approx(2.0f));
        REQUIRE(vertices[0].y == Approx(3.0f));
        REQUIRE(vertices[0].z == Approx(4.0f));
        REQUIRE(vertices[1].x == Approx(4.0f));
        REQUIRE(vertices[1].y == Approx(6.0f));
        REQUIRE(vertices[1].z == Approx(8.0f));
    }
}

TEST_CASE("Primitive mesh generation - Cube", "[mesh][resources][primitives]") {
    SECTION("Create default cube") {
        TLETC::Mesh cube = TLETC::GeometryFactory::CreateCube();
        
        REQUIRE_FALSE(cube.IsEmpty());
        REQUIRE(cube.GetVertexCount() == 24); // 6 faces * 4 vertices
        REQUIRE(cube.GetIndexCount() == 36);  // 6 faces * 2 triangles * 3 indices
        REQUIRE(cube.GetTriangleCount() == 12);
        REQUIRE(cube.IsIndexed());
    }
    
    SECTION("Cube has correct bounds") {
        TLETC::Mesh cube = TLETC::GeometryFactory::CreateCube(2.0f);
        
        auto bounds = cube.CalculateBoundingBox();
        
        REQUIRE(bounds.min.x == Approx(-1.0f));
        REQUIRE(bounds.max.x == Approx( 1.0f));
        REQUIRE(bounds.min.y == Approx(-1.0f));
        REQUIRE(bounds.max.y == Approx( 1.0f));
        REQUIRE(bounds.min.z == Approx(-1.0f));
        REQUIRE(bounds.max.z == Approx( 1.0f));
    }
    
    SECTION("Cube has normals and UVs") {
        TLETC::Mesh cube = TLETC::GeometryFactory::CreateCube();
        
        // Check that vertices have valid normals
        for (size_t i=0; i<cube.GetVertexCount(); ++i) {
            float normalLength = TLETC::Length(cube.GetVertexNormal(i));
            REQUIRE(normalLength == Approx(1.0f).margin(0.01f));
            
            auto uv = cube.GetVertexUV(i);
            // Check UVs are in valid range
            REQUIRE(uv.x >= 0.0f);
            REQUIRE(uv.x <= 1.0f);
            REQUIRE(uv.y >= 0.0f);
            REQUIRE(uv.y <= 1.0f);
        }
    }
}

TEST_CASE("Primitive mesh generation - Sphere", "[mesh][resources][primitives]") {
    SECTION("Create sphere") {
        TLETC::Mesh sphere = TLETC::GeometryFactory::CreateSphere(1.0f, 16, 8);
        
        REQUIRE_FALSE(sphere.IsEmpty());
        REQUIRE(sphere.GetVertexCount() > 0);
        REQUIRE(sphere.GetIndexCount() > 0);
        REQUIRE(sphere.IsIndexed());
    }
    
    SECTION("Sphere vertices are on surface") {
        TLETC::Mesh sphere = TLETC::GeometryFactory::CreateSphere(1.0f, 16, 8);
        
        auto& vertices = sphere.GetVertexPositions();
        
        // Check that all vertices are approximately at radius distance
        for (const auto& vertex : vertices) {
            float distance = TLETC::Length(vertex);
            REQUIRE(distance == Approx(1.0f).margin(0.01f));
        }
    }
    
    SECTION("Sphere normals point outward") {
        TLETC::Mesh sphere = TLETC::GeometryFactory::CreateSphere(1.0f, 16, 8);
        
        for (size_t i=0; i<sphere.GetVertexCount(); ++i) {
            auto normal = sphere.GetVertexNormal(i);

            // Normal should be normalized
            float normalLength = TLETC::Length(normal);
            REQUIRE(normalLength == Approx(1.0f).margin(0.01f));
            
            // Normal should point in same direction as position (for unit sphere)
            TLETC::Vec3 normalizedPos = TLETC::Normalize(sphere.GetVertexPosition(i));
            float dotProduct = TLETC::Dot(normal, normalizedPos);
            REQUIRE(dotProduct == Approx(1.0f).margin(0.01f));
        }
    }
}

TEST_CASE("Primitive mesh generation - Plane", "[mesh][resources][primitives]") {
    SECTION("Create simple plane") {
        TLETC::Mesh plane = TLETC::GeometryFactory::CreatePlane(10.0f, 10.0f, 1, 1);
        
        REQUIRE_FALSE(plane.IsEmpty());
        REQUIRE(plane.GetVertexCount() == 4);  // 2x2 grid
        REQUIRE(plane.GetIndexCount() == 6);   // 2 triangles
        REQUIRE(plane.IsIndexed());
    }
    
    SECTION("Plane lies on XZ") {
        TLETC::Mesh plane = TLETC::GeometryFactory::CreatePlane(2.0f, 2.0f, 1, 1);
        
        // All vertices should have y = 0
        // Normals should point up
        for (size_t i=0; i<plane.GetVertexCount(); ++i) 
        {
            REQUIRE(plane.GetVertexPosition(i).y == Approx(0.0f));

            auto normal = plane.GetVertexNormal(i);
            REQUIRE(normal.x == Approx(0.0f));
            REQUIRE(normal.y == Approx(1.0f));
            REQUIRE(normal.z == Approx(0.0f));
        }
    }
    
    SECTION("Subdivided plane") {
        TLETC::Mesh plane = TLETC::GeometryFactory::CreatePlane(10.0f, 10.0f, 10, 10);
        
        REQUIRE(plane.GetVertexCount() == 11 * 11);  // (segments+1)^2
        REQUIRE(plane.GetTriangleCount() == 10 * 10 * 2);  // segments^2 * 2
    }
}

TEST_CASE("Primitive mesh generation - Cylinder", "[mesh][resources][primitives]") {
    SECTION("Create cylinder") {
        TLETC::Mesh cylinder = TLETC::GeometryFactory::CreateCylinder(1.0f, 2.0f, 16);
        
        REQUIRE_FALSE(cylinder.IsEmpty());
        REQUIRE(cylinder.GetVertexCount() > 0);
        REQUIRE(cylinder.GetIndexCount() > 0);
        REQUIRE(cylinder.IsIndexed());
    }
    
    SECTION("Cylinder height") {
        TLETC::Mesh cylinder = TLETC::GeometryFactory::CreateCylinder(1.0f, 2.0f, 16);
        
        auto bounds = cylinder.CalculateBoundingBox();
        
        REQUIRE(bounds.min.y == Approx(-1.0f).margin(0.01f));
        REQUIRE(bounds.max.y == Approx( 1.0f).margin(0.01f));
    }
}

TEST_CASE("Primitive mesh generation - Quad", "[mesh][resources][primitives]") {
    SECTION("Create quad") {
        TLETC::Mesh quad = TLETC::GeometryFactory::CreateQuad(2.0f, 2.0f);
        
        REQUIRE(quad.GetVertexCount() == 4);
        REQUIRE(quad.GetIndexCount() == 6);
        REQUIRE(quad.GetTriangleCount() == 2);
    }
}

TEST_CASE("Mesh normal recalculation", "[mesh][resources]") {
    SECTION("Recalculate normals for triangle") {
        TLETC::Mesh mesh;
        
        // Create a triangle facing up
        mesh.AddVertex(TLETC::Vec3(0.0f, 0.0f, 0.0f));
        mesh.AddVertex(TLETC::Vec3(1.0f, 0.0f, 0.0f));
        mesh.AddVertex(TLETC::Vec3(0.0f, 0.0f, 1.0f));
        mesh.AddTriangle(0, 2, 1);
        
        mesh.RecalculateNormals();
        
        auto& normals = mesh.GetVertexNormals();
        
        // All normals should point up (positive Y)
        for (const auto& normal : normals) {
            REQUIRE(normal.y > 0.5f); // Should be close to 1
            float length = TLETC::Length(normal);
            REQUIRE(length == Approx(1.0f).margin(0.01f));
        }
    }
}