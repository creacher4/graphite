#pragma once

#include "core/ISystem.h"
#include "rendering/Camera.h"
#include "input/InputManager.h"

class CameraController : public ISystem
{
public:
    void Init() override {};
    void Update(float deltaTime) override;
    void Shutdown() override {};

    void SetCamera(Camera *camera) { m_Camera = camera; }

private:
    Camera *m_Camera = nullptr;
    float m_moveSpeed = 5.0f;   // units per second
    float m_lookSpeed = 0.002f; // radians per pixel
};
