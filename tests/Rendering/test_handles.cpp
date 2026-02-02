#include <catch2/catch_test_macros.hpp>
#include "TLETC/Rendering/Handle.h"

TEST_CASE("Handle creation and validity", "[rendering][handles]") {
    SECTION("Default handle is invalid") {
        TLETC::BufferHandle handle;
        
        REQUIRE_FALSE(handle.IsValid());
    }
    
    SECTION("Create valid handle") {
        TLETC::BufferHandle handle(42);
        
        REQUIRE(handle.IsValid());
        REQUIRE(handle.GetID() == 42);
    }
    
    SECTION("Handle comparison") {
        TLETC::BufferHandle h1(1);
        TLETC::BufferHandle h2(1);
        TLETC::BufferHandle h3(2);
        
        REQUIRE(h1 == h2);
        REQUIRE(h1 != h3);
    }
    
    SECTION("Reset handle") {
        TLETC::BufferHandle handle(42);
        REQUIRE(handle.IsValid());
        
        handle.Reset();
        REQUIRE_FALSE(handle.IsValid());
    }
}

TEST_CASE("Different handle types", "[rendering][handles]") {
    SECTION("BufferHandle") {
        TLETC::BufferHandle handle(1);
        REQUIRE(handle.IsValid());
    }
    
    SECTION("ShaderHandle") {
        TLETC::ShaderHandle handle(2);
        REQUIRE(handle.IsValid());
    }
    
    SECTION("TextureHandle") {
        TLETC::TextureHandle handle(3);
        REQUIRE(handle.IsValid());
    }
    
    SECTION("PipelineHandle") {
        TLETC::PipelineHandle handle(4);
        REQUIRE(handle.IsValid());
    }
    
    SECTION("Handles are type-safe") {
        TLETC::BufferHandle buffer(1);
        TLETC::ShaderHandle shader(1);
        
        // This should not compile if uncommented (type safety)
        // REQUIRE(buffer == shader);
    }
}