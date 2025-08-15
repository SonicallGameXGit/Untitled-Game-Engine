#include "transform.hpp"
#include <glm/gtc/matrix_transform.hpp>

Transform3DComponent::Transform3DComponent() {}
Transform3DComponent::Transform3DComponent(const glm::vec3 &position) : position(position) {}
Transform3DComponent::Transform3DComponent(const glm::vec3 &position, const glm::vec3 &rotation, const glm::vec3 &scale) :
    position(position),
    rotation(rotation),
    scale(scale)
{}
Transform3DComponent::~Transform3DComponent() {}

glm::mat4 Transform3DComponent::getModelMatrix() const {
    glm::mat4 matrix = glm::mat4(1.0f);
    matrix = glm::translate(matrix, this->position);
    matrix = glm::rotate(matrix, glm::radians(this->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    matrix = glm::rotate(matrix, glm::radians(this->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    matrix = glm::rotate(matrix, glm::radians(this->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    matrix = glm::scale(matrix, this->scale);

    return matrix;
}

Transform2DComponent::Transform2DComponent() {}
Transform2DComponent::Transform2DComponent(const glm::vec2 &position) : position(position) {}
Transform2DComponent::Transform2DComponent(const glm::vec2 &position, float rotation, const glm::vec2 &scale) :
    position(position),
    rotation(rotation),
    scale(scale)
{}
Transform2DComponent::~Transform2DComponent() {}

glm::mat4 Transform2DComponent::getModelMatrix() const {
    glm::mat4 matrix = glm::mat4(1.0f);
    matrix = glm::translate(matrix, glm::vec3(this->position, 0.0f));
    matrix = glm::rotate(matrix, glm::radians(this->rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    matrix = glm::scale(matrix, glm::vec3(this->scale, 1.0f));

    return matrix;
}