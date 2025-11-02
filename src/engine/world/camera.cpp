#include "camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera() {}
Camera::~Camera() {}

void Camera::update(const Window &window) {
    glm::mat4 rotator = glm::mat4(1.0f);
    rotator = glm::rotate(rotator, glm::radians(this->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    rotator = glm::rotate(rotator, glm::radians(this->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    rotator = glm::rotate(rotator, glm::radians(this->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));

    this->front = glm::vec3(rotator * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f));
    this->up = glm::vec3(rotator * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    this->right = glm::vec3(rotator * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

    switch (this->mode) {
        case CameraMode::PERSPECTIVE: {
            this->projectionMatrix = glm::perspective(glm::radians(this->fov), window.getHorizontalAspect(), this->zNear, this->zFar);
            break;
        }
        case CameraMode::ORTHOGRAPHIC: {
            float horizontalSize = this->orthoSize * window.getHorizontalAspect();
            this->projectionMatrix = glm::ortho(-horizontalSize, horizontalSize, -this->orthoSize, this->orthoSize, this->zNear, this->zFar);
            break;
        }
        default: break;
    }
    this->viewMatrix = glm::lookAt(this->position, this->position + this->front, this->up);
    this->projectionViewMatrix = this->projectionMatrix * this->viewMatrix;
}

const glm::mat4 &Camera::getProjectionMatrix() const {
    return this->projectionMatrix;
}
const glm::mat4 &Camera::getViewMatrix() const {
    return this->viewMatrix;
}
const glm::mat4 &Camera::getProjectionViewMatrix() const {
    return this->projectionViewMatrix;
}