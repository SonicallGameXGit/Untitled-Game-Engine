#include "world.hpp"

Script::Script() {}
Script::~Script() {}

World::ScriptComponent::ScriptComponent() {}
World::ScriptComponent::~ScriptComponent() {}

static uint64_t latestIndex = 0;
EntityIdentifier::EntityIdentifier() : index(latestIndex) { latestIndex++; }

World::NodeComponent::NodeComponent(const std::string &name) : name(name), parent(entt::null) {}
World::NodeComponent::NodeComponent(const std::string &name, Entity parent) : name(name), parent(parent) {}
World::NodeComponent::~NodeComponent() {}

void Script::onLoad(World &world, Entity self) {}
void Script::onUpdate(const Window& window, World &world, Entity self, float deltaTime) {}
void Script::onDestroy(World &world, Entity self) {}

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

std::optional<Entity> World::spawn(const std::string &name, std::optional<Entity> parent) {
    if (!parent.has_value()) {
        if (this->hierarchy.find(name) != this->hierarchy.end()) {
            // TODO: Instead of just giving std::nullopt, generate unique name like "{name} (1)", etc.
            return std::nullopt; // Dalbayob obnaruzhen
        }

        Entity entity = this->ecs.spawn();
        this->addComponent<EntityIdentifier>(entity);
        this->addComponent<ScriptComponent>(entity);
        this->addComponent<NodeComponent>(entity, name);
        this->hierarchy[name] = entity;
        return entity;
    } else {
        NodeComponent &nodeComponent = this->getMutableComponent<NodeComponent>(parent.value());
        if (nodeComponent.children.find(name) != nodeComponent.children.end()) {
            return std::nullopt; // Dalbayob naiden
        }

        Entity entity = this->ecs.spawn();
        this->addComponent<EntityIdentifier>(entity);
        this->addComponent<ScriptComponent>(entity);
        this->addComponent<NodeComponent>(entity, name, parent.value());
        nodeComponent.children[name] = entity;

        return entity;
    }

    return std::nullopt;
}

void World::destroy(Entity entity) {
    std::unordered_map<std::string, Entity> children = this->getChildren(entity);
    for (auto &[name, child] : children) {
        this->destroy(child);
    }
    
    ScriptComponent &component = this->getMutableComponent<ScriptComponent>(entity);
    if (component.script != nullptr) {
        component.script->onDestroy(*this, entity); // And here even more so :)
        delete component.script;
        component.script = nullptr;
    }

    std::optional<Entity> parent = this->getParent(entity);
    if (!parent.has_value()) {
        this->hierarchy.erase(this->getName(entity));
    } else {
        NodeComponent &nodeComponent = this->getMutableComponent<NodeComponent>(parent.value());
        nodeComponent.children.erase(this->getName(entity));
    }
    
    this->ecs.destroy(entity);
}

const std::string &World::getName(Entity entity) const {
    return this->getComponent<NodeComponent>(entity).name;
}
void World::rename(Entity entity, const std::string &newName) {
    std::optional<Entity> parent = this->getParent(entity);
    if (parent.has_value()) {
        NodeComponent &nodeComponent = this->getMutableComponent<NodeComponent>(parent.value());
        nodeComponent.children.erase(this->getName(entity));
        nodeComponent.children[newName] = entity;
    } else {
        this->hierarchy.erase(this->getName(entity));
        this->hierarchy[newName] = entity;
    }
    this->getMutableComponent<NodeComponent>(entity).name = newName;
}

std::optional<Entity> World::find(const std::string &name, std::optional<Entity> parent) const {
    if (parent.has_value()) {
        const NodeComponent &nodeComponent = this->getComponent<NodeComponent>(parent.value());
        auto it = nodeComponent.children.find(name);
        if (it == nodeComponent.children.end()) return std::nullopt;
        return it->second;
    }

    auto it = this->hierarchy.find(name);
    if (it == this->hierarchy.end()) return std::nullopt;
    return it->second;
}
std::optional<Entity> World::getParent(Entity child) const {
    const NodeComponent &nodeComponent = this->getComponent<NodeComponent>(child);
    if (nodeComponent.parent == entt::null) return std::nullopt;
    return nodeComponent.parent;
}
const std::unordered_map<std::string, Entity> &World::getChildren(std::optional<Entity> parent) const {
    if (!parent.has_value()) return this->hierarchy;
    return this->getComponent<NodeComponent>(parent.value()).children;
}

// FIXME: Calling remove script in the script itself to destroy self could lead to issues if something is accessed/called after that
// Maybe, move it to the next tick?
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