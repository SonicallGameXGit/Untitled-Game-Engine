#pragma once
#include <engine/world/mesh.hpp>
#include <framework/graphics/texture.hpp>
#include <glm/glm.hpp>

struct MeshComponent {
private:
    Mesh *mesh = nullptr;
    Texture *texture = nullptr;
public:
    glm::vec4 color = glm::vec4(1.0f);

    explicit MeshComponent(Mesh &mesh);
    MeshComponent(Mesh &mesh, Texture &texture);
    MeshComponent(Mesh &mesh, Texture &texture, const glm::vec4 &color);
    MeshComponent(Mesh &mesh, const glm::vec4 &color);
    ~MeshComponent();

    void setMesh(Mesh &mesh);
    void setTexture(Texture *texture);
    
    Mesh *getMesh() const;
    Texture *getTexture() const;
};