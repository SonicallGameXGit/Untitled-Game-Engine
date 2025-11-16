#include "renderer.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <engine/ecs/components/mesh.hpp>
#include <engine/ecs/components/transform.hpp>
#include <engine/ecs/components/world.hpp>
#include <engine/ecs/components/gui.hpp>
#include <engine/graphics/gui/text.hpp>

static void drawWorld(const ShaderProgram &shader, const Camera &camera, const MeshComponent &mesh, const glm::mat4 &projectionViewMatrix, const glm::mat4 &modelMatrix) {
    shader.setMat4("u_ProjectionViewMatrix", projectionViewMatrix);
    shader.setMat4("u_ModelMatrix", modelMatrix);
    shader.setVec3("u_ViewPosition", camera.position);
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
static void drawSprite(const VertexArray &vertexArray, const ShaderProgram &shader, const SpriteComponent &sprite, const glm::mat4 &projectionViewMatrix, const glm::mat4 &modelMatrix) {
    shader.setMat4("u_ProjectionViewMatrix", projectionViewMatrix);
    shader.setMat4("u_ModelMatrix", modelMatrix);
    shader.setVec4("u_Color", sprite.color);

    if (sprite.texture != nullptr) {
        sprite.texture->bind(0);
        shader.setBool("u_HasTexture", true);
    } else {
        shader.setBool("u_HasTexture", false);
    }

    vertexArray.drawArrays(Topology::TriangleFan);
}
static void drawText(const ShaderProgram &shader, const TextComponent &text, const glm::mat4 &projectionViewMatrix, const glm::mat4 &modelMatrix) {
    const Font *font = text.getFont();
    if (font == nullptr) {
        return;
    }

    shader.setMat4("u_ProjectionViewMatrix", projectionViewMatrix);
    shader.setMat4("u_ModelMatrix", modelMatrix);
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
    for (auto [_, value] : world.getChildren(parent)) {
        if (!world.isEnabled(value)) continue;

        if (world.hasComponents<Transform3DComponent>(value)) {
            Transform3DComponent &transform = world.getMutableComponent<Transform3DComponent>(value);
            if (parent.has_value() && world.hasComponents<Transform3DComponent>(parent.value())) {
                transform.worldModelMatrix = world.getMutableComponent<Transform3DComponent>(parent.value()).worldModelMatrix * transform.getModelMatrix();
            } else {
                transform.worldModelMatrix = transform.getModelMatrix();
            }
        } else if (world.hasComponents<Transform2DComponent>(value)) {
            Transform2DComponent &transform = world.getMutableComponent<Transform2DComponent>(value);
            float scale = 1.0f;
            if (world.hasComponents<TextComponent>(value)) {
                scale = world.getComponent<TextComponent>(value).textHeight;
            }
            if (parent.has_value() && world.hasComponents<Transform2DComponent>(parent.value())) {
                transform.worldModelMatrix = world.getMutableComponent<Transform2DComponent>(parent.value()).worldModelMatrix * glm::scale(transform.getModelMatrix(), glm::vec3(scale, scale, 1.0f));
            } else {
                transform.worldModelMatrix = glm::scale(transform.getModelMatrix(), glm::vec3(scale, scale, 1.0f));
            }
        }

        precomputeWorldMatrices(world, value);
    }
}

void Renderer::render(const Window &window, World &world) const {
    // Step 0: Initial setup for rendering and precompute world matrices for all entities
    precomputeWorldMatrices(world, std::nullopt);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Step 1: Draw all opaque world objects
    {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glDisable(GL_BLEND);

        glm::mat4 projectionViewMatrix = world.camera.getProjectionViewMatrix();
        auto transparentObjects = std::vector<std::tuple<MeshComponent, Transform3DComponent>>();
        for (auto [entity, mesh, transform] : world.getAllEntitiesWith<MeshComponent, Transform3DComponent>().each()) {
            if (!world.isEnabled(entity)) continue;
            if (mesh.color.a < 1.0f) {
                transparentObjects.emplace_back(mesh, transform);
                continue;
            }

            drawWorld(this->worldShader, world.camera, mesh, projectionViewMatrix, transform.worldModelMatrix);
        }

        glEnable(GL_BLEND);
        // FIXME: Don't just cull the faces of all transparent objects, but try to find way to render polygons from back to front if possible
        // TODO: Add depth sorting for transparent objects
        for (auto &[mesh, transform] : transparentObjects) {
            drawWorld(this->worldShader, world.camera, mesh, world.camera.getProjectionViewMatrix(), transform.worldModelMatrix);
        }
    } // FIXME: Too slow / [Avg: 0.155663ms | Peak: 1.036083ms]

    // Step 2: Draw all 2D sprites (no need to sort them by depth or draw transparent ones separately because they're already sorted by spawn order)
    {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);

        // EnTT doesn't guarantee that entt::entity (aka. Entity) would be the same as its creation order due to optimizations and other stuff.
        // TODO: If possible, optimize it, because that looks cringe
        // TODO: Try to render all sprites by iterating recursively trough hierarchy instead of recieving all of them and sorting | Ne, chuvak blyat', tak ne srabotaet, a esli odin dolboeb na drugovo zaberetsya?
        auto entitiesView = world.getAllEntitiesWith<EntityIdentifier, Transform2DComponent>().each();
        auto entities = std::vector<std::tuple<Entity, EntityIdentifier, const Transform2DComponent*>>();
        entities.reserve(std::distance(entitiesView.begin(), entitiesView.end()));
        for (auto [entity, identifier, transform] : entitiesView) {
            if (!world.isEnabled(entity)) continue;
            entities.emplace_back(entity, identifier, &transform);
        }
        std::sort(entities.begin(), entities.end(), [](const auto& a, const auto& b) {
            return std::get<1>(a).index < std::get<1>(b).index;
        });

        glm::mat4 projectionViewMatrix = world.camera.getProjectionViewMatrix();
        for (auto [entity, identifier, transform] : entities) {
            if (world.hasComponents<SpriteComponent>(entity)) {
                drawSprite(
                    this->spriteVertexArray,
                    this->spriteShader,
                    world.getComponent<SpriteComponent>(entity),
                    projectionViewMatrix,
                    transform->worldModelMatrix
                );
                continue;
            }
            if (world.hasComponents<TextComponent>(entity)) {
                drawText(
                    this->textShader,
                    world.getComponent<TextComponent>(entity),
                    projectionViewMatrix,
                    transform->worldModelMatrix
                );
                continue;
            }
        }
    }

    // Step 3: Precompute GUI matrices for all entities \
    // Step 3.5: Draw all GUI elements (no need to sort them by depth or draw transparent ones separately because they're already sorted by spawn order)
    // Debug::beginTimeMeasure();
    this->guiController.render(window, world, this->spriteVertexArray, this->guiShader, this->textShader);
    // Debug::endTimeMeasure();
}