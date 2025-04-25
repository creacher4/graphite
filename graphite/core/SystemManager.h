#pragma once

#include "ISystem.h"
#include <vector>

class SystemManager
{
public:
    void RegisterSystem(ISystem *system);
    void InitAll();
    void UpdateAll();
    void ShutdownAll();

private:
    std::vector<ISystem *> m_Systems;
};