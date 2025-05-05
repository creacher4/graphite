#pragma once

#include "ISystem.h"
#include <vector>

class SystemManager
{
public:
    void RegisterSystem(ISystem *system);
    void InitAll();
    void UpdateAll(float deltaTime) const;
    void ShutdownAll() const;

private:
    std::vector<ISystem *> m_Systems;
};