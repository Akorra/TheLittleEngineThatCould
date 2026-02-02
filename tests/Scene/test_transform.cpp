#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <TLETC/Scene/Transform.h>

using Catch::Approx;

TEST_CASE("Transform component", "[scene][transform]") {
    SECTION("Default construction") {
        TLETC::Transform transform;
        
        REQUIRE(transform.position.x == 0.0f);
        REQUIRE(transform.position.y == 0.0f);
        REQUIRE(transform.position.z == 0.0f);
        
        REQUIRE(transform.scale.x == 1.0f);
        REQUIRE(transform.scale.y == 1.0f);
        REQUIRE(transform.scale.z == 1.0f);
    }
    
    SECTION("Get model matrix") {
        TLETC::Transform transform;
        transform.position = {1.0f, 2.0f, 3.0f};
        transform.scale = {2.0f, 2.0f, 2.0f};
        
        TLETC::Mat4 model = transform.GetModelMatrix();
        
        // Transform a point at origin
        TLETC::Vec4 point(0.0f, 0.0f, 0.0f, 1.0f);
        TLETC::Vec4 transformed = model * point;
        
        REQUIRE(transformed.x == Approx(1.0f));
        REQUIRE(transformed.y == Approx(2.0f));
        REQUIRE(transformed.z == Approx(3.0f));
    }
    
    SECTION("Get forward vector") {
        TLETC::Transform transform;
        
        TLETC::Vec3 forward = transform.GetForward();
        
        // Default forward should be -Z
        REQUIRE(forward.x == Approx(0.0f).margin(0.001f));
        REQUIRE(forward.y == Approx(0.0f).margin(0.001f));
        REQUIRE(forward.z == Approx(-1.0f).margin(0.001f));
    }
    
    SECTION("Rotate around axis") {
        TLETC::Transform transform;
        transform.Rotate(TLETC::Vec3(0.0f, 1.0f, 0.0f), 90.0f);
        
        TLETC::Vec3 forward = transform.GetForward();
        
        // After 90 degree rotation around Y, forward should point in +X
        REQUIRE(forward.x == Approx(1.0f).margin(0.001f));
        REQUIRE(forward.y == Approx(0.0f).margin(0.001f));
        REQUIRE(forward.z == Approx(0.0f).margin(0.001f));
    }
    
    SECTION("LookAt") {
        TLETC::Transform transform;
        transform.position = {0.0f, 0.0f, 0.0f};
        
        TLETC::Vec3 target(1.0f, 0.0f, 0.0f);
        TLETC::Vec3 up(0.0f, 1.0f, 0.0f);
        
        transform.LookAt(target, up);
        
        TLETC::Vec3 forward = transform.GetForward();
        
        // Forward should point towards target
        REQUIRE(forward.x == Approx(1.0f).margin(0.001f));
        REQUIRE(forward.y == Approx(0.0f).margin(0.001f));
        REQUIRE(forward.z == Approx(0.0f).margin(0.001f));
    }
}

TEST_CASE("Transform hierarchy", "[scene][transform]") {
    SECTION("Parent-child relationship") {
        TLETC::Transform parent;
        TLETC::Transform child;
        
        parent.position = {1.0f, 0.0f, 0.0f};
        child.position = {1.0f, 0.0f, 0.0f};
        
        child.SetParent(&parent);
        
        TLETC::Vec3 worldPos = child.GetWorldPosition();
        
        // Child's world position should be parent + local
        REQUIRE(worldPos.x == Approx(2.0f));
        REQUIRE(worldPos.y == Approx(0.0f));
        REQUIRE(worldPos.z == Approx(0.0f));
    }
    
    SECTION("World matrix with parent") {
        TLETC::Transform parent;
        TLETC::Transform child;
        
        parent.scale = {2.0f, 2.0f, 2.0f};
        child.position = {1.0f, 0.0f, 0.0f};
        child.SetParent(&parent);
        
        TLETC::Mat4 worldMatrix = child.GetWorldMatrix();
        
        TLETC::Vec4 point(0.0f, 0.0f, 0.0f, 1.0f);
        TLETC::Vec4 transformed = worldMatrix * point;
        
        // Child at (1,0,0) with parent scale (2,2,2) -> world pos (2,0,0)
        REQUIRE(transformed.x == Approx(2.0f));
    }
}