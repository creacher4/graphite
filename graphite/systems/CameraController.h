#pragma once

#include "core/ISystem.h"
#include "input/InputManager.h"

class Camera;
class SceneManager;

class CameraController : public ISystem
{
public:
    explicit CameraController(SceneManager *sceneManager);
    void Init() override{};
    void Update(float deltaTime) override;
    void Shutdown() override{};

private:
    SceneManager *m_SceneManager = nullptr;

    static constexpr float MOVE_SPEED = 5.0f;   // units per second
    static constexpr float LOOK_SPEED = 0.002f; // radians per pixel
};