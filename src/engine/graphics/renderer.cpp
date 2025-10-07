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

static void precomputeWorldMatrices(World &world, std::optional<Entity> parent) {
    for (auto [key, value] : world.getChildren(parent)) {
        if (world.hasComponents<Transform3DComponent>(value)) {
            Transform3DComponent &transform = world.getMutableComponent<Transform3DComponent>(value);
            if (parent.has_value() && world.hasComponents<Transform3DComponent>(parent.value())) {
                transform.worldModelMatrix = world.getMutableComponent<Transform3DComponent>(parent.value()).worldModelMatrix * transform.getModelMatrix();
            } else {
                transform.worldModelMatrix = transform.getModelMatrix();
            }
        }
        if (world.hasComponents<Transform2DComponent>(value)) {
            Transform2DComponent &transform = world.getMutableComponent<Transform2DComponent>(value);
            if (parent.has_value() && world.hasComponents<Transform2DComponent>(parent.value())) {
                transform.worldModelMatrix = world.getMutableComponent<Transform2DComponent>(parent.value()).worldModelMatrix * transform.getModelMatrix();
            } else {
                transform.worldModelMatrix = transform.getModelMatrix();
            }

        }

        precomputeWorldMatrices(world, value);
    }
}

void Renderer::render(const Window &window, World &world) const {
    // Step -1: Precompute world matrices for all entities
    precomputeWorldMatrices(world, std::nullopt);

    // Step 0: Initial setup for rendering
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Step 1: Draw all opaque world objects
    this->worldShader.bind();
    {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glDisable(GL_BLEND);

        glm::mat4 projectionViewMatrix = world.camera.getProjectionViewMatrix();
        auto transparentObjects = std::vector<std::tuple<MeshComponent, Transform3DComponent>>();
        for (auto [_, mesh, transform] : world.getAllEntitiesWith<MeshComponent, Transform3DComponent>().each()) {
            if (mesh.color.a < 1.0f) {
                transparentObjects.emplace_back(mesh, transform);
                continue;
            }

            drawWorld(this->worldShader, mesh, projectionViewMatrix, transform.worldModelMatrix);
        }

        glEnable(GL_BLEND);
        // FIXME: Don't just cull the faces of all transparent objects, but try to find way to render polygons from back to front if possible
        // TODO: Add depth sorting for transparent objects
        for (auto &[mesh, transform] : transparentObjects) {
            drawWorld(this->worldShader, mesh, world.camera.getProjectionViewMatrix(), transform.worldModelMatrix);
        }
    }

    // Step 2: Draw all gui elements (no need to sort them by depth or draw transparent ones separately because they're already sorted by spawn order)
    {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);

        float aspect = window.getHorizontalAspect();
        glm::mat4 projectionViewMatrix = glm::orthoLH(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);
        
        // EnTT doesn't guarantee that entt::entity (aka. Entity) would be the same as its creation order due to optimizations and other stuff.
        // TODO: If possible, optimize it, because that looks cringe
        auto entitiesView = world.getAllEntitiesWith<EntityIdentifier, Transform2DComponent>().each();
        auto entities = std::vector<std::tuple<Entity, EntityIdentifier, Transform2DComponent>>();
        for (auto [entity, identifier, transform] : entitiesView) {
            entities.emplace_back(entity, identifier, transform);
        }
        std::sort(entities.begin(), entities.end(), [](const auto& a, const auto& b) {
            return std::get<1>(a).index < std::get<1>(b).index;
        });

        for (auto [entity, identifier, transform] : entities) {
            if (world.hasComponents<SpriteComponent>(entity)) {
                drawSprite(
                    this->spriteVertexArray,
                    this->spriteShader,
                    world.getComponent<SpriteComponent>(entity),
                    projectionViewMatrix * transform.worldModelMatrix
                );
                continue;
            }
            if (world.hasComponents<TextComponent>(entity)) {
                drawText(
                    this->textShader,
                    world.getComponent<TextComponent>(entity),
                    projectionViewMatrix * transform.worldModelMatrix
                );
                continue;
            }
        }
    }
}