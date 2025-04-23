#pragma once

#include "entt.hpp"

class ECSRegistry
{
public:
    ECSRegistry() = default;

    entt::entity CreateEntity() { return m_Registry.create(); }

    template <typename T, typename... Args>
    void AddComponent(entt::entity e, Args &&...args)
    {
        m_Registry.emplace<T>(e, std::forward<Args>(args)...);
    }

    template <typename... Components>
    auto View()
    {
        return m_Registry.view<Components...>();
    }

private:
    entt::registry m_Registry;
};