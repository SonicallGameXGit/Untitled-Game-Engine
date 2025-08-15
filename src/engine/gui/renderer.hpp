#pragma once
#include <entt/entt.hpp>
#include "../../framework/graphics/window.hpp"
#include "../../framework/graphics/shader.hpp"
#include "../../framework/graphics/buffers.hpp"
#include "../util/defines.hpp"
#include "../ecs/ecs.hpp"
#include "../world/camera.hpp"

class TextRenderer {
private:
    ShaderProgram shader = ShaderProgram({
        Shader::fromFile(ASSETS_DIR "/shaders/text.vert", ShaderType::Vertex),
        Shader::fromFile(ASSETS_DIR "/shaders/text.frag", ShaderType::Fragment)
    });
public:
    TextRenderer();
    ~TextRenderer();

    void render(const ECS &ecs, const Window &window, const Camera &camera) const;
};
class SpriteRenderer {
private:
    ShaderProgram shader = ShaderProgram({
        Shader::fromFile(ASSETS_DIR "/shaders/sprite.vert", ShaderType::Vertex),
        Shader::fromFile(ASSETS_DIR "/shaders/sprite.frag", ShaderType::Fragment)
    });
    VertexBuffer vertexBuffer = VertexBuffer(std::vector<float>({
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    }), VertexBufferUsage::Static);
    VertexArray vertexArray = VertexArray();
public:
    SpriteRenderer();
    ~SpriteRenderer();

    void render(const ECS &ecs, const Window &window, const Camera &camera) const;
};