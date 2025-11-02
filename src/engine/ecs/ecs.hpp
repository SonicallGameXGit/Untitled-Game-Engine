#pragma once
#include <entt/entt.hpp>
#include <framework/graphics/window.hpp>

typedef entt::entity Entity;

class ECS {
private:
    entt::registry registry = entt::registry();
public:
    ECS();
    ~ECS();

    Entity spawn();
    void destroy(Entity entity);

    template<typename T, typename ...Args>
    T &addComponent(Entity entity, Args &&...args) {
        return this->registry.emplace<T>(entity, std::forward<Args>(args)...);
    }
    template<typename T, typename ...Other>
    void removeComponents(Entity entity) {
        this->registry.erase<T, Other...>(entity);
    }

    template<typename T, typename ...Other>
    bool hasComponents(Entity entity) const {
        return this->registry.any_of<T, Other...>(entity);
    }
    template<typename ...T>
    auto &getMutableComponent(Entity entity) {
        return this->registry.get<T...>(entity);
    }
    template<typename ...T>
    auto &getComponent(Entity entity) const {
        return this->registry.get<T...>(entity);
    }

    template<typename T, typename ...Other>
    auto getAllEntitiesWith() const {
        return this->registry.view<T, Other...>();
    }
    template<typename T, typename ...Other>
    auto getAllMutableEntitiesWith() {
        return this->registry.view<T, Other...>();
    }
};