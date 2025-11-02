#pragma once
#include <glm/glm.hpp>
#include <framework/graphics/window.hpp>

enum class CameraMode {
    PERSPECTIVE,
    ORTHOGRAPHIC
};

class Camera {
private:
    glm::mat4 projectionMatrix = glm::mat4(), viewMatrix = glm::mat4(), projectionViewMatrix = glm::mat4();
    glm::vec3 front = glm::vec3(0.0f, 0.0f, 1.0f), up = glm::vec3(0.0f, 1.0f, 0.0f), right = glm::vec3(1.0f, 0.0f, 0.0f);
public:
    CameraMode mode = CameraMode::PERSPECTIVE;
    glm::vec3 position = glm::vec3(), rotation = glm::vec3();
    
    float fov = 90.0f, zNear = 0.01f, zFar = 1000.0f;
    float orthoSize = 1.0f;

    Camera();
    ~Camera();

    void update(const Window &window);

    const glm::mat4 &getProjectionMatrix() const;
    const glm::mat4 &getViewMatrix() const;
    const glm::mat4 &getProjectionViewMatrix() const;
};