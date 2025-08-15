#include "renderer.hpp"
#include "../ecs/components/mesh.hpp"
#include "../ecs/components/transform.hpp"

WorldRenderer::WorldRenderer() {
    this->shader.bind();
    this->shader.setInt("u_ColorSampler", 0);
}
WorldRenderer::~WorldRenderer() {}

static void drawMesh(const ShaderProgram &shader, const glm::mat4 &projectionViewMatrix, const MeshComponent &mesh, const Transform3DComponent &transform) {
    glm::mat4 modelMatrix = transform.getModelMatrix();
    shader.setMat4("u_ProjectionViewModelMatrix", projectionViewMatrix * modelMatrix);
    shader.setMat4("u_ModelMatrix", modelMatrix);
    shader.setVec4("u_Color", mesh.color);

    const Texture *texture = mesh.getTexture();
    if (texture != nullptr) {
        texture->bind(0);
        shader.setBool("u_HasTexture", true);
    } else {
        shader.setBool("u_HasTexture", false);
    }

    mesh.getMesh()->draw();
}

void WorldRenderer::render(const ECS &ecs, const Camera &camera) const {
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    this->shader.bind();
    const glm::mat4 &projectionViewMatrix = camera.getProjectionViewMatrix();

    std::vector<std::tuple<MeshComponent, Transform3DComponent>> transparentObjects = {};
    for (auto [_, mesh, transform] : ecs.listAllEntitiesWith<MeshComponent, Transform3DComponent>().each()) {
        if (mesh.color.a < 1.0f) {
            transparentObjects.emplace_back(mesh, transform);
            continue;
        }

        drawMesh(this->shader, projectionViewMatrix, mesh, transform);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    
    // TODO: Use depth sorting for transparent objects
    for (auto &[mesh, transform] : transparentObjects) {
        drawMesh(this->shader, projectionViewMatrix, mesh, transform);
    }
}