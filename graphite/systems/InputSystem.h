#pragma once

#include "core/ISystem.h"
#include <Windows.h>

class InputSystem : public ISystem
{
public:
    void Init() override { OutputDebugStringA("[InputSystem] Initialized.\n"); };
    void Update() override { /* OutputDebugStringA("[InputSystem] Updated.\n") */ ; };
    void Shutdown() override { OutputDebugStringA("[InputSystem] Shutdown.\n"); };
};