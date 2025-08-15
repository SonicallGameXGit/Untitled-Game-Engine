#include "mesh.hpp"

MeshComponent::MeshComponent(Mesh &mesh) : mesh(&mesh) {}
MeshComponent::MeshComponent(Mesh &mesh, Texture &texture) : mesh(&mesh), texture(&texture) {}
MeshComponent::MeshComponent(Mesh &mesh, Texture &texture, const glm::vec4 &color) : mesh(&mesh), texture(&texture), color(color) {}
MeshComponent::MeshComponent(Mesh &mesh, const glm::vec4 &color) : mesh(&mesh), color(color) {}
MeshComponent::~MeshComponent() {}

void MeshComponent::setMesh(Mesh &mesh) {
    this->mesh = &mesh;
}
void MeshComponent::setTexture(Texture *texture) {
    this->texture = texture;
}

const Mesh *MeshComponent::getMesh() const {
    return this->mesh;
}
const Texture *MeshComponent::getTexture() const {
    return this->texture;
}