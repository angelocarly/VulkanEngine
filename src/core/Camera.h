//
// Created by magnias on 09/03/2021.
//

#ifndef VULKANENGINE_CAMERA_H
#define VULKANENGINE_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Game.h"
#include "../vks/vks_input.h"

class Camera
{

public:
    Camera()
    {
        updateRotation();
    }

    void setInputHandler(VksInput *handler)
    {
        _inputHandler = handler;
    }

    void update()
    {
        if (_inputHandler == nullptr) return;

        glm::vec2 delta = _inputHandler->pollMouseDelta();

        rotatePitch(delta.y / 100.0f);
        rotateYaw(delta.x / 100.0f);

        if (_inputHandler->isKeyDown(GLFW_KEY_A))
        {
            _position -= _right /= 100;
        }

        if (_inputHandler->isKeyDown(GLFW_KEY_D))
        {
            _position += _right /= 100;
        }

        if (_inputHandler->isKeyDown(GLFW_KEY_W))
        {
            _position += _forward /= 100;
        }

        if (_inputHandler->isKeyDown(GLFW_KEY_S))
        {
            _position -= _forward /= 100;
        }

        if (_inputHandler->isKeyDown(GLFW_KEY_SPACE))
        {
            _position.y += 0.01f;
        }

        if (_inputHandler->isKeyDown(GLFW_KEY_LEFT_CONTROL))
        {
            _position.y -= 0.01f;
        }

    }

    glm::mat4 calculateModelMatrix()
    {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), _position);
        return model;
    }

    glm::mat4 calculateViewMatrix()
    {
        glm::normalize(_rotation);
        glm::mat4 rotate = glm::mat4_cast(_rotation);

        return rotate;
    }

    glm::mat4 calculateProjectionMatrix()
    {
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), 800.0f /
                                                               (float) 600.0f, 0.1f,
                                          10.0f);

        return proj;
    }

    void updateRotation()
    {
        _rotation = _qYaw * _qPitch * _qRoll;
        _forward = glm::normalize(_rotation * glm::vec3(0, 0, 1));
        _right = glm::normalize(glm::cross(UP, _forward));
        _up = glm::normalize(glm::cross(_forward, _right));
    }

    /**
     * Rotate the view direction upward/downward
     * @param rads
     */
    void rotatePitch(float rads)
    {
        glm::quat qPitch = glm::angleAxis(-rads, glm::vec3(1.0f, 0, 0));
        _qPitch = _qPitch * qPitch;
        updateRotation();
    }

    /**
     * Rotate the view direction left/right
     * @param rads
     */
    void rotateYaw(float rads) // rotate around cams local X axis
    {
        glm::quat qYaw = glm::angleAxis(rads, glm::vec3(0.0f, 1.0f, 0));
        _qYaw = _qYaw * qYaw;// * _rotation;
        updateRotation();
    }

    glm::vec3 getPosition()
    {
        return _position;
    }

    glm::vec3 getForward()
    {
        return _forward;
    }

    glm::vec3 getRight()
    {
        return _right;
    }

    glm::vec3 getUp()
    {
        return _up;
    }

private:
    VksInput *_inputHandler = nullptr;

    const glm::vec3 UP = glm::vec3(0, 1, 0);
    glm::vec3 _position = glm::vec3();
    glm::vec3 _forward = glm::vec3();
    glm::vec3 _right = glm::vec3();
    glm::vec3 _up = glm::vec3();
    glm::quat _qPitch = glm::angleAxis(0.0f, glm::vec3(1, 0, 0));
    glm::quat _qYaw = glm::angleAxis(0.0f, glm::vec3(0, 1, 0));
    glm::quat _qRoll = glm::angleAxis(0.0f, glm::vec3(0, 0, 1));
    glm::quat _rotation = glm::normalize(_qPitch * _qYaw * _qRoll);

    glm::mat4 _mModel;
    glm::mat4 _mView;
    glm::mat4 _mProj;

};

#endif //VULKANENGINE_CAMERA_H
