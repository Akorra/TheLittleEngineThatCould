#include <TLETC/Core/Math.h>
#include <TLETC/Resources/Mesh.h>
#include <TLETC/Resources/GeometryFactory.h>
#include <TLETC/Scene/Transform.h>
#include <iostream>
#include <iomanip>

void PrintMeshInfo(const std::string& name, const TLETC::Mesh& mesh) {
    std::cout << "=== " << name << " ===" << std::endl;
    std::cout << "  Vertices: " << mesh.GetVertexCount() << std::endl;
    std::cout << "  Indices:  " << mesh.GetIndexCount() << std::endl;
    std::cout << "  Triangles: " << mesh.GetTriangleCount() << std::endl;
    
    auto bounds = mesh.CalculateBoundingBox();
    std::cout << "  Bounds Min: (" 
              << bounds.min.x << ", " 
              << bounds.min.y << ", " 
              << bounds.min.z << ")" << std::endl;
    std::cout << "  Bounds Max: (" 
              << bounds.max.x << ", " 
              << bounds.max.y << ", " 
              << bounds.max.z << ")" << std::endl;
    
    auto center = bounds.GetCenter();
    std::cout << "  Center: (" 
              << center.x << ", " 
              << center.y << ", " 
              << center.z << ")" << std::endl;
    std::cout << std::endl;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  The Little Engine That Could" << std::endl;
    std::cout << "  Example: Mesh Creation" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    
    // Test 1: Manual mesh creation
    std::cout << "--- Test 1: Manual Mesh Creation ---" << std::endl;
    TLETC::Mesh triangle;
    triangle.AddVertex(TLETC::Vec3( 0.0f,  0.5f, 0.0f));
    triangle.AddVertex(TLETC::Vec3(-0.5f, -0.5f, 0.0f));
    triangle.AddVertex(TLETC::Vec3( 0.5f, -0.5f, 0.0f));
    triangle.AddTriangle(0, 1, 2);
    
    PrintMeshInfo("Triangle", triangle);
    
    // Test 2: Primitive meshes
    std::cout << "--- Test 2: Primitive Meshes ---" << std::endl;
    
    auto cube = TLETC::GeometryFactory::CreateCube(1.0f);
    PrintMeshInfo("Cube", cube);
    
    auto sphere = TLETC::GeometryFactory::CreateSphere(0.5f, 32, 16);
    PrintMeshInfo("Sphere", sphere);
    
    auto plane = TLETC::GeometryFactory::CreatePlane(10.0f, 10.0f, 10, 10);
    PrintMeshInfo("Plane", plane);
    
    auto cylinder = TLETC::GeometryFactory::CreateCylinder(0.5f, 2.0f, 32);
    PrintMeshInfo("Cylinder", cylinder);
    
    // Test 3: Mesh transformations
    std::cout << "--- Test 3: Mesh Transformations ---" << std::endl;
    
    auto transformedCube = TLETC::GeometryFactory::CreateCube(1.0f);
    std::cout << "Original cube bounds:" << std::endl;
    auto originalBounds = transformedCube.CalculateBoundingBox();
    std::cout << "  Min: (" << originalBounds.min.x << ", " << originalBounds.min.y << ", " << originalBounds.min.z << ")" << std::endl;
    std::cout << "  Max: (" << originalBounds.max.x << ", " << originalBounds.max.y << ", " << originalBounds.max.z << ")" << std::endl;
    
    transformedCube.Translate(TLETC::Vec3(2.0f, 3.0f, 4.0f));
    std::cout << "\nAfter translate(2, 3, 4):" << std::endl;
    auto translatedBounds = transformedCube.CalculateBoundingBox();
    std::cout << "  Min: (" << translatedBounds.min.x << ", " << translatedBounds.min.y << ", " << translatedBounds.min.z << ")" << std::endl;
    std::cout << "  Max: (" << translatedBounds.max.x << ", " << translatedBounds.max.y << ", " << translatedBounds.max.z << ")" << std::endl;
    
    auto scaledCube = TLETC::GeometryFactory::CreateCube(1.0f);
    scaledCube.Scale(TLETC::Vec3(2.0f, 0.5f, 3.0f));
    std::cout << "\nAfter scale(2, 0.5, 3):" << std::endl;
    auto scaledBounds = scaledCube.CalculateBoundingBox();
    std::cout << "  Min: (" << scaledBounds.min.x << ", " << scaledBounds.min.y << ", " << scaledBounds.min.z << ")" << std::endl;
    std::cout << "  Max: (" << scaledBounds.max.x << ", " << scaledBounds.max.y << ", " << scaledBounds.max.z << ")" << std::endl;
    std::cout << std::endl;
    
    // Test 4: Normal recalculation
    std::cout << "--- Test 4: Normal Recalculation ---" << std::endl;
    
    TLETC::Mesh quadMesh;
    quadMesh.AddVertex(TLETC::Vec3(-1.0f, 0.0f, -1.0f));
    quadMesh.AddVertex(TLETC::Vec3( 1.0f, 0.0f, -1.0f));
    quadMesh.AddVertex(TLETC::Vec3( 1.0f, 0.0f,  1.0f));
    quadMesh.AddVertex(TLETC::Vec3(-1.0f, 0.0f,  1.0f));
    quadMesh.AddTriangle(0, 1, 2);
    quadMesh.AddTriangle(0, 2, 3);
    
    std::cout << "Before recalculating normals:" << std::endl;
    for (size_t i = 0; i < quadMesh.GetVertexCount(); ++i) {
        auto& normal = quadMesh.GetVertexNormal(i);
        std::cout << "  Vertex " << i << " normal: (" 
                  << normal.x << ", " 
                  << normal.y << ", " 
                  << normal.z << ")" << std::endl;
    }
    
    quadMesh.RecalculateNormals();
    
    std::cout << "\nAfter recalculating normals:" << std::endl;
    for (size_t i = 0; i < quadMesh.GetVertexCount(); ++i) {
        auto& normal = quadMesh.GetVertexNormal(i);
        std::cout << "  Vertex " << i << " normal: (" 
                  << std::fixed << std::setprecision(4)
                  << normal.x << ", " 
                  << normal.y << ", " 
                  << normal.z << ")" << std::endl;
    }
    std::cout << std::endl;
    
    // Test 5: Transform system integration
    std::cout << "--- Test 5: Transform System ---" << std::endl;
    
    TLETC::Transform transform;
    transform.position = TLETC::Vec3(5.0f, 10.0f, 15.0f);
    transform.scale = TLETC::Vec3(2.0f, 2.0f, 2.0f);
    transform.Rotate(TLETC::Vec3(0.0f, 1.0f, 0.0f), 45.0f);
    
    std::cout << "Transform:" << std::endl;
    std::cout << "  Position: (" << transform.position.x << ", " << transform.position.y << ", " << transform.position.z << ")" << std::endl;
    std::cout << "  Scale: (" << transform.scale.x << ", " << transform.scale.y << ", " << transform.scale.z << ")" << std::endl;
    
    auto forward = transform.GetForward();
    std::cout << "  Forward: (" << forward.x << ", " << forward.y << ", " << forward.z << ")" << std::endl;
    
    auto right = transform.GetRight();
    std::cout << "  Right: (" << right.x << ", " << right.y << ", " << right.z << ")" << std::endl;
    
    std::cout << std::endl;
    
    std::cout << "========================================" << std::endl;
    std::cout << "  All tests completed successfully!" << std::endl;
    std::cout << "========================================" << std::endl;
    
    return 0;
}
