#pragma once

#include "entt.hpp"

class ECSRegistry
{
public:
    ECSRegistry() = default;

    entt::entity CreateEntity() { return registry.create(); }

    template <typename T, typename... Args>
    void AddComponent(entt::entity e, Args &&...args)
    {
        registry.emplace<T>(e, std::forward<Args>(args)...);
    }

    template <typename... T>
    auto View()
    {
        return registry.view<T...>();
    }

private:
    entt::registry registry;
};
