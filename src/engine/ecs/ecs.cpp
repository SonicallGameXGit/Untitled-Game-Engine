#include "ecs.hpp"

ECS::ECS() {}
ECS::~ECS() {}

Entity ECS::spawn() {
    return this->registry.create();
}
void ECS::destroy(Entity entity) {
    this->registry.destroy(entity);
}