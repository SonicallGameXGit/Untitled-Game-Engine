#pragma once
#include <framework/graphics/shader.hpp>
#include <framework/graphics/buffers.hpp>
#include <framework/graphics/window.hpp>
#include <engine/world/world.hpp>
#include <engine/util/defines.hpp>

class Renderer {
private:
    ShaderProgram worldShader = ShaderProgram({
        Shader::fromFile(ASSETS_DIR "/shaders/world.vert", ShaderType::Vertex),
        Shader::fromFile(ASSETS_DIR "/shaders/world.frag", ShaderType::Fragment)
    });
    ShaderProgram spriteShader = ShaderProgram({
        Shader::fromFile(ASSETS_DIR "/shaders/sprite.vert", ShaderType::Vertex),
        Shader::fromFile(ASSETS_DIR "/shaders/sprite.frag", ShaderType::Fragment)
    });
    ShaderProgram textShader = ShaderProgram({
        Shader::fromFile(ASSETS_DIR "/shaders/text.vert", ShaderType::Vertex),
        Shader::fromFile(ASSETS_DIR "/shaders/text.frag", ShaderType::Fragment)
    });

    VertexBuffer spriteVertexBuffer = VertexBuffer(std::vector<float>({
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    }), VertexBufferUsage::Static);
    VertexArray spriteVertexArray = VertexArray();
public:
    Renderer();
    ~Renderer();

    void render(const Window &window, const World &world) const;
};