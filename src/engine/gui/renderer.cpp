#include "renderer.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "../ecs/components/transform.hpp"
#include "../ecs/components/gui.hpp"

TextRenderer::TextRenderer() {
    this->shader.bind();
    this->shader.setInt("u_ColorSampler", 0);
    this->shader.setFloat("u_PixelRange", Font::PIXEL_RANGE);
}
TextRenderer::~TextRenderer() {}

static void drawText(const ShaderProgram &shader, const TextComponent &text, const glm::mat4 &projectionViewMatrix, const glm::mat4 &modelMatrix) {
    const Font *font = text.getFont();
    if (font == nullptr) {
        return;
    }

    shader.setMat4("u_ProjectionViewModelMatrix", projectionViewMatrix * modelMatrix);
    shader.setMat4("u_ModelMatrix", modelMatrix);
    shader.setVec4("u_Color", text.color);

    font->getTexture().bind(0);
    text.getMesh().draw();
}
static void drawSprite(const VertexArray &vertexArray, const ShaderProgram &shader, const SpriteComponent &sprite, const glm::mat4 &projectionViewMatrix, const glm::mat4 &modelMatrix) {
    shader.setMat4("u_ProjectionViewModelMatrix", projectionViewMatrix * modelMatrix);
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

void TextRenderer::render(const ECS &ecs, const Window &window, const Camera &camera) const {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    this->shader.bind();
    {
        glDisable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

        glm::mat4 projectionViewMatrix = camera.getProjectionViewMatrix();
        std::vector<std::tuple<TextComponent, Transform3DComponent>> transparentObjects = {};

        for (auto [_, text, transform] : ecs.listAllEntitiesWith<TextComponent, Transform3DComponent>().each()) {
            if (text.color.a < 1.0f) {
                transparentObjects.emplace_back(text, transform);
                continue;
            }

            drawText(this->shader, text, projectionViewMatrix, transform.getModelMatrix());
        }
        
        // TODO: Use depth sorting for transparent objects
        for (auto &[text, transform] : transparentObjects) {
            drawText(this->shader, text, projectionViewMatrix, transform.getModelMatrix());
        }
    }
    {
        glDisable(GL_DEPTH_TEST);

        std::vector<std::tuple<TextComponent, Transform2DComponent>> transparentObjects = {};
        float aspect = window.getHorizontalAspect();
        glm::mat4 projectionViewMatrix = glm::orthoLH(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);

        for (auto [_, text, transform] : ecs.listAllEntitiesWith<TextComponent, Transform2DComponent>().each()) {
            if (text.color.a < 1.0f) {
                transparentObjects.emplace_back(text, transform);
            }

            drawText(this->shader, text, projectionViewMatrix, transform.getModelMatrix());
        }
        for (auto &[text, transform] : transparentObjects) {
            drawText(this->shader, text, projectionViewMatrix, transform.getModelMatrix());
        }
    }
}

SpriteRenderer::SpriteRenderer() {
    this->shader.bind();
    this->shader.setInt("u_ColorSampler", 0);

    this->vertexArray.bindVertexBuffer(this->vertexBuffer, {
        {
            .type = VertexAttributeType::Float,
            .size = VertexAttributeSize::Vec2,
        }
    });
}
SpriteRenderer::~SpriteRenderer() {}

void SpriteRenderer::render(const ECS &ecs, const Window &window, const Camera &camera) const {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    
    this->shader.bind();
    {
        glEnable(GL_DEPTH_TEST);
        glm::mat4 projectionViewMatrix = camera.getProjectionViewMatrix();
        for (auto [_, sprite, transform] : ecs.listAllEntitiesWith<SpriteComponent, Transform3DComponent>().each()) {
            drawSprite(this->vertexArray, this->shader, sprite, projectionViewMatrix, transform.getModelMatrix());
        }
    }
    {
        glEnable(GL_DEPTH_TEST);
        float aspect = window.getHorizontalAspect();
        glm::mat4 projectionViewMatrix = glm::orthoLH(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);
        for (auto [_, sprite, transform] : ecs.listAllEntitiesWith<SpriteComponent, Transform2DComponent>().each()) {
            drawSprite(this->vertexArray, this->shader, sprite, projectionViewMatrix, transform.getModelMatrix());
        }
    }
}