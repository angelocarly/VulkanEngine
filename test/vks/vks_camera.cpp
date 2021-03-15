#include <catch2/catch.hpp>
#include <glm/vec3.hpp>
#include <glm/gtx/string_cast.hpp>
#include "../../src/core/Camera.h"

/**
 * Helper function to compare vec3 without failing on rounding errors
 */
static bool isVecEqual(glm::vec3 expected, glm::vec3 result, float delta)
{
    if(expected.x < result.x - delta) return false;
    if(expected.x > result.x + delta) return false;
    if(expected.y < result.y - delta) return false;
    if(expected.y > result.y + delta) return false;
    if(expected.z < result.z - delta) return false;
    if(expected.z > result.z + delta) return false;

    return true;
}

//TEST_CASE("Camera pitch rotation")
//{
//
//    Camera camera;
//
//    REQUIRE(camera.calculateLook() == glm::vec3(0, 0, 1));
//
//    camera.rotatePitch(M_PI / 2);
//
//    glm::vec3 expected = glm::vec3(0.0f, 1.0f, 0.0f);
//    glm::vec3 look = camera.calculateLook();
//    if(!isVecEqual(expected, look, 0.01f)) {
//        FAIL("Vector " + glm::to_string(expected) + " is not equal to " + glm::to_string(look));
//    }
//
//    camera.rotatePitch(M_PI / 2);
//
//    expected = glm::vec3(0.0f, 0.0f, -1.0f);
//    look = camera.calculateLook();
//    if(!isVecEqual(expected, look, 0.01f)) {
//        FAIL("Vector " + glm::to_string(expected) + " is not equal to " + glm::to_string(look));
//    }
//
//    camera.rotatePitch(M_PI / 2);
//
//    expected = glm::vec3(0.0f, -1.0f, 0.0f);
//    look = camera.calculateLook();
//    if(!isVecEqual(expected, look, 0.01f)) {
//        FAIL("Vector " + glm::to_string(expected) + " is not equal to " + glm::to_string(look));
//    }
//
//    camera.rotatePitch(M_PI / 2);
//
//    expected = glm::vec3(0.0f, 0.0f, 1.0f);
//    look = camera.calculateLook();
//    if(!isVecEqual(expected, look, 0.01f)) {
//        FAIL("Vector " + glm::to_string(expected) + " is not equal to " + glm::to_string(look));
//    }
//}
//
//TEST_CASE("Camera yaw rotation")
//{
//
//    Camera camera;
//
//    REQUIRE(camera.calculateLook() == glm::vec3(0, 0, 1));
//
//    camera.rotateYaw(M_PI / 2);
//
//    glm::vec3 expected = glm::vec3(1.0f, 0.0f, 0.0f);
//    glm::vec3 look = camera.calculateLook();
//    if(!isVecEqual(expected, look, 0.01f)) {
//        FAIL("Vector " + glm::to_string(expected) + " is not equal to " + glm::to_string(look));
//    }
//
//    camera.rotateYaw(M_PI / 2);
//
//    expected = glm::vec3(0.0f, 0.0f, -1.0f);
//    look = camera.calculateLook();
//    if(!isVecEqual(expected, look, 0.01f)) {
//        FAIL("Vector " + glm::to_string(expected) + " is not equal to " + glm::to_string(look));
//    }
//
//    camera.rotateYaw(M_PI / 2);
//
//    expected = glm::vec3(-1.0f, 0.0f, 0.0f);
//    look = camera.calculateLook();
//    if(!isVecEqual(expected, look, 0.01f)) {
//        FAIL("Vector " + glm::to_string(expected) + " is not equal to " + glm::to_string(look));
//    }
//
//    camera.rotateYaw(M_PI / 2);
//
//    expected = glm::vec3(0.0f, 0.0f, 1.0f);
//    look = camera.calculateLook();
//    if(!isVecEqual(expected, look, 0.01f)) {
//        FAIL("Vector " + glm::to_string(expected) + " is not equal to " + glm::to_string(look));
//    }
//}
