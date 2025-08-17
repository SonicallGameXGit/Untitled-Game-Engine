#include "renderer.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <engine/ecs/components/mesh.hpp>
#include <engine/ecs/components/transform.hpp>
#include <engine/ecs/components/gui.hpp>
#include <engine/graphics/gui/text.hpp>

static void drawWorld(const ShaderProgram &shader, const MeshComponent &mesh, const glm::mat4 &projectionViewMatrix, const glm::mat4 &modelMatrix) {
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
static void drawSprite(const VertexArray &vertexArray, const ShaderProgram &shader, const SpriteComponent &sprite, const glm::mat4 &projectionViewModelMatrix) {
    shader.setMat4("u_ProjectionViewModelMatrix", projectionViewModelMatrix);
    shader.setVec4("u_Color", sprite.color);

    if (sprite.texture != nullptr) {
        sprite.texture->bind(0);
        shader.setBool("u_HasTexture", true);
    } else {
        shader.setBool("u_HasTexture", false);
    }

    vertexArray.drawArrays(Topology::TriangleFan);
}
static void drawText(const ShaderProgram &shader, const TextComponent &text, const glm::mat4 &projectionViewModelMatrix) {
    const Font *font = text.getFont();
    if (font == nullptr) {
        return;
    }

    shader.setMat4("u_ProjectionViewModelMatrix", projectionViewModelMatrix);
    shader.setVec4("u_Color", text.color);

    font->getTexture().bind(0);
    text.getMesh().draw();
}

Renderer::Renderer() {
    this->worldShader.bind();
    this->worldShader.setInt("u_ColorSampler", 0);

    this->spriteShader.bind();
    this->spriteShader.setInt("u_ColorSampler", 0);

    this->textShader.bind();
    this->textShader.setInt("u_ColorSampler", 0);
    this->textShader.setFloat("u_PixelRange", Font::PIXEL_RANGE);

    this->spriteVertexArray.bindVertexBuffer(this->spriteVertexBuffer, {
        {
            .type = VertexAttributeType::Float,
            .size = VertexAttributeSize::Vec2,
        }
    });
}
Renderer::~Renderer() {}

void Renderer::render(const ECS &ecs, const Window &window, const Camera &camera) const {
    // Step 0: Initial setup for rendering
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Step 1: Draw all opaque world objects
    this->worldShader.bind();
    {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glDisable(GL_BLEND);

        glm::mat4 projectionViewMatrix = camera.getProjectionViewMatrix();
        auto transparentObjects = std::vector<std::tuple<MeshComponent, Transform3DComponent>>();
        for (auto [_, mesh, transform] : ecs.getAllEntitiesWith<MeshComponent, Transform3DComponent>().each()) {
            if (mesh.color.a < 1.0f) {
                transparentObjects.emplace_back(mesh, transform);
                continue;
            }

            drawWorld(this->worldShader, mesh, projectionViewMatrix, transform.getModelMatrix());
        }

        glEnable(GL_BLEND);
        // FIXME: Don't just cull the faces of all transparent objects, but try to find way to render polygons from back to front if possible
        // TODO: Add depth sorting for transparent objects
        for (auto &[mesh, transform] : transparentObjects) {
            drawWorld(this->worldShader, mesh, camera.getProjectionViewMatrix(), transform.getModelMatrix());
        }
    }

    // Step 2: Draw all gui elements (no need to sort them by depth or draw transparent ones separately because they're already sorted by spawn order)
    {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);

        float aspect = window.getHorizontalAspect();
        glm::mat4 projectionViewMatrix = glm::orthoLH(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);
        
        // EnTT doesn't guarantee that entt::entity (aka. Entity) would be the same as it's creation order due to optimizations and other stuff.
        // TODO: If possible, optimize it, because that looks like a cringe
        auto entitiesView = ecs.getAllEntitiesWith<EntityIdentifier, Transform2DComponent>().each();
        auto entities = std::vector<std::tuple<Entity, EntityIdentifier, Transform2DComponent>>();
        for (auto [entity, identifier, transform] : entitiesView) {
            entities.emplace_back(entity, identifier, transform);
        }
        std::sort(entities.begin(), entities.end(), [](const auto& a, const auto& b) {
            return std::get<1>(a).index < std::get<1>(b).index;
        });

        for (auto [entity, identifier, transform] : entities) {
            if (ecs.hasComponents<SpriteComponent>(entity)) {
                drawSprite(
                    this->spriteVertexArray,
                    this->spriteShader,
                    ecs.getComponent<SpriteComponent>(entity),
                    projectionViewMatrix * transform.getModelMatrix()
                );
                continue;
            }
            if (ecs.hasComponents<TextComponent>(entity)) {
                drawText(
                    this->textShader,
                    ecs.getComponent<TextComponent>(entity),
                    projectionViewMatrix * transform.getModelMatrix()
                );
                continue;
            }
        }
    }
}