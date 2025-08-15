#pragma once
#include <entt/entt.hpp>
#include "../../framework/graphics/shader.hpp"
#include "../util/defines.hpp"
#include "../ecs/ecs.hpp"
#include "mesh.hpp"
#include "camera.hpp"

class WorldRenderer {
private:
    ShaderProgram shader = ShaderProgram({
        Shader::fromFile(ASSETS_DIR "/shaders/world.vert", ShaderType::Vertex),
        Shader::fromFile(ASSETS_DIR "/shaders/world.frag", ShaderType::Fragment)
    });
public:
    WorldRenderer();
    ~WorldRenderer();

    void render(const ECS &ecs, const Camera &camera) const;
};