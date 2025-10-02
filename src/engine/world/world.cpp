#include "world.hpp"

Script::Script() {}
Script::~Script() {}

World::ScriptComponent::ScriptComponent() {}
World::ScriptComponent::~ScriptComponent() {}

static uint64_t latestIndex = 0;
EntityIdentifier::EntityIdentifier() : index(latestIndex) { latestIndex++; }

World::World() {}
World::~World() {
    auto view = this->getAllEntitiesWith<ScriptComponent>();
    for (auto entity : view) {
        ScriptComponent &component = this->getMutableComponent<ScriptComponent>(entity);
        if (component.script == nullptr) continue;
        
        component.script->onDestroy(*this, entity); // In this case we probably need it
        delete component.script;
        component.script = nullptr;
    }
}

Entity World::spawn() {
    Entity entity = this->ecs.spawn();
    this->addComponent<EntityIdentifier>(entity);
    this->addComponent<ScriptComponent>(entity);
    return entity;
}
void World::destroy(Entity entity) {
    ScriptComponent &component = this->getMutableComponent<ScriptComponent>(entity);
    if (component.script != nullptr) {
        component.script->onDestroy(*this, entity); // And here even more so :)
        delete component.script;
        component.script = nullptr;
    }
    this->ecs.destroy(entity);
}
void World::removeScript(Entity entity) {
    ScriptComponent &component = this->getMutableComponent<ScriptComponent>(entity);
    if (component.script == nullptr) return;

    // component.script->onDestroy(*this, entity); --- The same thing there ---
    delete component.script;
    component.script = nullptr;
}
Script *World::getScript(Entity entity) const {
    return this->getComponent<ScriptComponent>(entity).script;
}

void World::update(const Window &window, float deltaTime) {
    this->camera.update(window);

    auto view = this->getAllEntitiesWith<ScriptComponent>();
    for (auto entity : view) {
        ScriptComponent &component = this->getMutableComponent<ScriptComponent>(entity);
        if (component.script == nullptr) continue;
        component.script->onUpdate(window, *this, entity, deltaTime);
    }
}