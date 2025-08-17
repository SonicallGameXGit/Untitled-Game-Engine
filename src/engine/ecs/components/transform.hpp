#pragma once
#include <glm/glm.hpp>
#include <framework/graphics/texture.hpp>

struct Transform3DComponent {
    glm::vec3 position = glm::vec3();
    glm::vec3 rotation = glm::vec3();
    glm::vec3 scale = glm::vec3(1.0f);

    Transform3DComponent();
    Transform3DComponent(const glm::vec3 &position);
    Transform3DComponent(const glm::vec3 &position, const glm::vec3 &rotation, const glm::vec3 &scale);
    ~Transform3DComponent();

    glm::mat4 getModelMatrix() const;
};
struct Transform2DComponent {
    glm::vec2 position = glm::vec2();
    float rotation = 0.0f;
    glm::vec2 scale = glm::vec2(1.0f);

    Transform2DComponent();
    Transform2DComponent(const glm::vec2 &position);
    Transform2DComponent(const glm::vec2 &position, float rotation, const glm::vec2 &scale);
    ~Transform2DComponent();

    glm::mat4 getModelMatrix() const;
};
