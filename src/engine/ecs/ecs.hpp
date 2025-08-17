#pragma once
#include <entt/entt.hpp>

typedef entt::entity Entity;

struct EntityIdentifier {
    uint64_t index = 0;
    EntityIdentifier();
};

class ECS {
private:
    entt::registry registry = entt::registry();
public:
    ECS();
    ~ECS();

    Entity spawn();
    void destroy(Entity entity);

    template<typename T, typename ...Args>
    void addComponent(Entity entity, Args &&...args) {
        this->registry.emplace<T>(entity, std::forward<Args>(args)...);
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
};