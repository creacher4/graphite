#include "SystemManager.h"

void SystemManager::RegisterSystem(std::unique_ptr<ISystem> system)
{
    m_Systems.push_back(std::move(system));
}

void SystemManager::InitAll()
{
    for (auto &sys : m_Systems)
    {
        sys->Init();
    }
}
void SystemManager::UpdateAll(float deltaTime) const
{
    for (const auto &sys : m_Systems)
    {
        sys->Update(deltaTime);
    }
}

void SystemManager::ShutdownAll() const
{
    // shutdown in reverse order of registration
    for (auto it = m_Systems.rbegin(); it != m_Systems.rend(); ++it)
    {
        (*it)->Shutdown();
    }
}

void SystemManager::OnResize(int width, int height)
{
    for (auto &sys : m_Systems)
    {
        sys->OnResize(width, height);
    }
}