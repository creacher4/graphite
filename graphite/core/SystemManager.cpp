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
void SystemManager::UpdateAll(float deltaTime) const
{
    for (auto sys : m_Systems)
    {
        sys->Update(deltaTime);
    }
}

void SystemManager::ShutdownAll() const
{
    for (auto sys : m_Systems)
    {
        sys->Shutdown();
    }
}