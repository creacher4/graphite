#include "SystemManager.h"

void SystemManager::RegisterSystem(ISystem *system)
{
    m_Systems.push_back(system);
}

void SystemManager::InitAll()
{
    for (auto sys : m_Systems)
    {
        sys->Init();
    }
}
void SystemManager::UpdateAll(float deltaTime)
{
    for (auto sys : m_Systems)
    {
        sys->Update(deltaTime);
    }
}

void SystemManager::ShutdownAll()
{
    for (auto sys : m_Systems)
    {
        sys->Shutdown();
    }
}