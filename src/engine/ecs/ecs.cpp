#include "ecs.hpp"

ECS::ECS() {}
ECS::~ECS() {}

static uint64_t latestIndex = 0;

EntityIdentifier::EntityIdentifier() : index(latestIndex) { latestIndex++; }

Entity ECS::spawn() {
    Entity entity = this->registry.create();
    this->addComponent<EntityIdentifier>(entity);

    return entity;
}
void ECS::destroy(Entity entity) {
    this->registry.destroy(entity);
}