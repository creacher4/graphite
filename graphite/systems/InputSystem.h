#pragma once

#include "core/ISystem.h"
#include "input/InputManager.h"
#include <Windows.h>

class InputSystem : public ISystem
{
public:
    void Init() override {};
    void Update(float /*deltaTime*/) override {
        // InputManager::Get().NewFrame();
    };
    void Shutdown() override {};
};