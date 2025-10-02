#pragma once
#include <engine/ecs/ecs.hpp>
#include <engine/world/camera.hpp>

struct EntityIdentifier {
    uint64_t index = 0;
    EntityIdentifier();
};

struct World; // C++, I promise, it will be declared in a second, are you ok with it?
struct Script {
    Script();
    virtual ~Script();

    virtual void onLoad(World &world, Entity self) = 0;
    virtual void onUpdate(const Window& window, World &world, Entity self, float deltaTime) = 0;
    virtual void onDestroy(World &world, Entity self) = 0;
};

class World {
private:
    ECS ecs = ECS();
    struct ScriptComponent {
        Script *script = nullptr;
        ScriptComponent();
        ~ScriptComponent();
    };
public:
    Camera camera = Camera();

    World();
    ~World();

    Entity spawn();
    void destroy(Entity entity);

    template<typename T, typename ...Args>
    T &addComponent(Entity entity, Args &&...args) {
        return this->ecs.addComponent<T>(entity, std::forward<Args>(args)...);
    }
    template<typename T, typename ...Other>
    void removeComponents(Entity entity) {
        this->ecs.removeComponents<T, Other...>(entity);
    }

    template<typename T, typename ...Other>
    bool hasComponents(Entity entity) const {
        return this->ecs.hasComponents<T, Other...>(entity);
    }
    template<typename ...T>
    auto &getMutableComponent(Entity entity) {
        return this->ecs.getMutableComponent<T...>(entity);
    }
    template<typename ...T>
    auto &getComponent(Entity entity) const {
        return this->ecs.getComponent<T...>(entity);
    }

    template<typename T, typename ...Other>
    auto getAllEntitiesWith() const {
        return this->ecs.getAllEntitiesWith<T, Other...>();
    }

    template<typename T, typename ...Args>
    void setScript(Entity entity, Args &&...args) {
        ScriptComponent &component = this->getMutableComponent<ScriptComponent>(entity);
        if (component.script) {
            // component.script->onDestroy(*this, entity); --- I don't really think we need this ---
            delete component.script;
        }
        component.script = new T(std::forward<Args>(args)...);
        component.script->onLoad(*this, entity);
    }
    void removeScript(Entity entity);
    Script *getScript(Entity entity) const;

    void update(const Window &window, float deltaTime);
};