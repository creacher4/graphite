#pragma once

#include "ISystem.h"
#include <vector>
#include <memory>

class SystemManager
{
public:
    void RegisterSystem(std::unique_ptr<ISystem> system);
    void InitAll();
    void UpdateAll(float deltaTime) const;
    void ShutdownAll() const;
    void OnResize(int width, int height);

private:
    std::vector<std::unique_ptr<ISystem>> m_Systems;
};