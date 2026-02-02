#pragma once

#include "TLETC/Resources/Mesh.h"
#include "TLETC/Core/Types.h"

namespace TLETC 
{
/**
 * @brief GeometryFactory - Utility class for generating primitive mesh shapes
 * 
 * This class provides static methods for creating common 3D primitive geometries.
 * All generated meshes include proper normals, UVs, and are indexed for efficient rendering.
 */
class GeometryFactory 
{
public:
    // No instances needed - all methods are static
    GeometryFactory() = delete;

    /**
     * Create a cube mesh
     * @param size The size of the cube (default: 1.0)
     * @return A mesh representing a cube centered at origin
     * 
     * The cube has 24 vertices (4 per face) and 36 indices (6 faces * 2 triangles * 3 indices).
     * Each face has its own vertices to ensure proper normals and UVs.
     */
    static Mesh CreateCube(float size = 1.0f);
    
    /**
     * Create a sphere mesh
     * @param radius The radius of the sphere (default: 0.5)
     * @param segments Number of horizontal segments (default: 32)
     * @param rings Number of vertical rings (default: 16)
     * @return A mesh representing a UV sphere centered at origin
     * 
     * Higher segment/ring counts create smoother spheres but increase vertex count.
     */
    static Mesh CreateSphere(float radius = 0.5f, uint32 segments = 32, uint32 rings = 16);
    
    /**
     * Create a plane mesh lying on the XZ plane
     * @param width Width along the X axis (default: 1.0)
     * @param height Height along the Z axis (default: 1.0)
     * @param widthSegments Number of subdivisions along width (default: 1)
     * @param heightSegments Number of subdivisions along height (default: 1)
     * @return A mesh representing a plane centered at origin, facing up (+Y)
     * 
     * Useful for terrain, water, or ground planes. Higher segment counts
     * allow for better vertex-based deformations.
     */
    static Mesh CreatePlane(float width = 1.0f, float height = 1.0f, uint32 widthSegments = 1, uint32 heightSegments = 1);
    
    /**
     * Create a cylinder mesh
     * @param radius Radius of the cylinder (default: 0.5)
     * @param height Height of the cylinder (default: 1.0)
     * @param segments Number of segments around the circumference (default: 32)
     * @return A mesh representing a cylinder centered at origin, aligned with Y axis
     * 
     * The cylinder includes top and bottom caps.
     */
    static Mesh CreateCylinder(float radius = 0.5f, float height = 1.0f, uint32 segments = 32);
    
    /**
     * Create a cone mesh
     * @param radius Radius of the base (default: 0.5)
     * @param height Height of the cone (default: 1.0)
     * @param segments Number of segments around the circumference (default: 32)
     * @return A mesh representing a cone with base centered at origin, pointing up (+Y)
     */
    static Mesh CreateCone(float radius = 0.5f, float height = 1.0f, uint32 segments = 32);
    
    /**
     * Create a torus (donut) mesh
     * @param majorRadius Distance from center to tube center (default: 0.5)
     * @param minorRadius Radius of the tube (default: 0.2)
     * @param majorSegments Number of segments around major radius (default: 32)
     * @param minorSegments Number of segments around minor radius (default: 16)
     * @return A mesh representing a torus centered at origin, lying in XZ plane
     */
    static Mesh CreateTorus(float majorRadius = 0.5f, float minorRadius = 0.2f, uint32 majorSegments = 32, uint32 minorSegments = 16);
    
    /**
     * Create a simple quad (two triangles)
     * @param width Width along the X axis (default: 1.0)
     * @param height Height along the Z axis (default: 1.0)
     * @return A mesh representing a quad (same as CreatePlane with 1 segment)
     * 
     * Useful for UI elements, sprites, or simple billboards.
     */
    static Mesh CreateQuad(float width = 1.0f, float height = 1.0f);
    
    /**
     * Create a capsule mesh (cylinder with hemisphere caps)
     * @param radius Radius of the capsule (default: 0.5)
     * @param height Height of the cylindrical section (default: 1.0)
     * @param segments Number of segments around circumference (default: 32)
     * @param rings Number of rings in each hemisphere (default: 8)
     * @return A mesh representing a capsule centered at origin, aligned with Y axis
     */
    static Mesh CreateCapsule(float radius = 0.5f, float height = 1.0f, uint32 segments = 32, uint32 rings = 8);
    
    /**
     * Create an icosphere (geodesic sphere) mesh
     * @param radius Radius of the sphere (default: 0.5)
     * @param subdivisions Number of subdivision iterations (default: 2)
     * @return A mesh representing an icosphere centered at origin
     * 
     * Icospheres have more uniform triangle distribution than UV spheres.
     * Each subdivision quadruples the triangle count.
     */
    static Mesh CreateIcosphere(float radius = 0.5f, uint32 subdivisions = 2);
};

}