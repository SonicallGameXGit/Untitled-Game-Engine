#include <includes.hpp>

struct MyScript : public Script {
    static glm::vec3 ROOM_SIZE;
    glm::vec3 velocity = glm::vec3();
    glm::vec3 viewTarget = glm::vec3(0.0f, 0.0f, 10000.0f);
    float nextSwitchTime = 0.0f, switchTimer = 0.0f, sleepTime = 0.0f, sleepTimer = 0.0f;

    bool jumping = false;
    explicit MyScript(bool jumping) : Script(), jumping(jumping) {}

    void switch_() {
        this->switchTimer -= this->nextSwitchTime;
        this->sleepTimer -= this->sleepTime;

        glm::vec2 direction = glm::vec2(
            (static_cast<float>(rand()) / static_cast<float>(RAND_MAX) - 0.5f) * 2.0f,
            (static_cast<float>(rand()) / static_cast<float>(RAND_MAX) - 0.5f) * 2.0f
        );
        if (glm::length(direction) > 1.0e-6f) {
            direction = glm::normalize(direction);
        } else {
            direction = glm::vec2(1.0f, 0.0f);
        }
        this->velocity.x = direction.x * 4.0f;
        this->velocity.z = direction.y * 4.0f;

        this->nextSwitchTime = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 6.0f;
        this->sleepTime = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 4.0f;
        this->switchTimer = 0.0f;

        this->viewTarget = glm::vec3(
            (direction.x + SDL_sinf(static_cast<float>(rand()) / static_cast<float>(RAND_MAX))) * ROOM_SIZE.x * 2.0f,
            (static_cast<float>(rand()) / static_cast<float>(RAND_MAX) - 0.5f) * ROOM_SIZE.y * 2.0f,
            (direction.y + SDL_sinf(static_cast<float>(rand()) / static_cast<float>(RAND_MAX))) * ROOM_SIZE.z * 2.0f
        );
    }

    void onLoad(World &world, Entity self) override {
        // printf("Entity %u created!\n", static_cast<uint32_t>(self));
        printf("Hi, my name is %s!\n", world.getName(self).c_str());
    }
    void onUpdate(const Window&, World &world, Entity self, float deltaTime) override {
        Transform3DComponent &transform = world.getMutableComponent<Transform3DComponent>(self);

        if (this->jumping) {
            this->velocity.y -= 9.81f * deltaTime;
            if (transform.position.y <= -MyScript::ROOM_SIZE.y) {
                transform.position.y = -MyScript::ROOM_SIZE.y;
                this->velocity.y = (5.0f + (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 5.0f) * (this->switchTimer < nextSwitchTime || this->sleepTimer >= sleepTime ? 1.0f : 0.0f);
            }
        } else {
            transform.position.y = -MyScript::ROOM_SIZE.y;
        }
        transform.position += this->velocity * deltaTime;

        std::optional<Entity> leftHand = world.find("Left Hand", self);
        std::optional<Entity> rightHand = world.find("Right Hand", self);
        this->switchTimer += deltaTime;
        if (this->switchTimer >= nextSwitchTime) {
            this->sleepTimer += deltaTime;
            this->velocity.x = 0.0f;
            this->velocity.z = 0.0f;

            if (leftHand.has_value()) {
                Transform3DComponent &leftHandTransform = world.getMutableComponent<Transform3DComponent>(leftHand.value());
                leftHandTransform.rotation.x = SDL_sinf(this->sleepTimer * 5.0f) * 45.0f;
            }
            if (rightHand.has_value()) {
                Transform3DComponent &rightHandTransform = world.getMutableComponent<Transform3DComponent>(rightHand.value());
                rightHandTransform.rotation.x = -SDL_sinf(this->sleepTimer * 5.0f) * 45.0f;
            }

            if (this->sleepTimer >= sleepTime) {
                this->switch_();
            }
        } else {
            if (transform.position.x < -MyScript::ROOM_SIZE.x) {
                transform.position.x = -MyScript::ROOM_SIZE.x;
                this->switch_();
            }
            if (transform.position.x > MyScript::ROOM_SIZE.x) {
                transform.position.x = MyScript::ROOM_SIZE.x;
                this->switch_();
            }
            if (transform.position.z < -MyScript::ROOM_SIZE.z) {
                transform.position.z = -MyScript::ROOM_SIZE.z;
                this->switch_();
            }
            if (transform.position.z > MyScript::ROOM_SIZE.z) {
                transform.position.z = MyScript::ROOM_SIZE.z;
                this->switch_();
            }

            bool goingUp = this->velocity.y > 0.0f;

            std::optional<Entity> leftLeg = world.find("Left Leg", self);
            std::optional<Entity> rightLeg = world.find("Right Leg", self);
            if (leftLeg.has_value()) {
                Transform3DComponent &leftLegTransform = world.getMutableComponent<Transform3DComponent>(leftLeg.value());
                if (glm::abs(this->velocity.x) + glm::abs(this->velocity.z) > 1.0e-6f && !goingUp) {
                    leftLegTransform.rotation.x = SDL_sinf(this->switchTimer * 10.0f) * 15.0f;
                    leftLegTransform.position.y = -SDL_cosf(this->switchTimer * 10.0f) * 0.3f;
                } else {
                    leftLegTransform.rotation.x -= leftLegTransform.rotation.x * deltaTime * 5.0f;
                    leftLegTransform.position.y -= leftLegTransform.position.y * deltaTime * 5.0f;
                }
            }
            if (rightLeg.has_value()) {
                Transform3DComponent &rightLegTransform = world.getMutableComponent<Transform3DComponent>(rightLeg.value());
                if (glm::abs(this->velocity.x) + glm::abs(this->velocity.z) > 1.0e-6f && !goingUp) {
                    rightLegTransform.rotation.x = -SDL_sinf(this->switchTimer * 10.0f) * 15.0f;
                    rightLegTransform.position.y = SDL_cosf(this->switchTimer * 10.0f) * 0.3f;
                } else {
                    rightLegTransform.rotation.x -= rightLegTransform.rotation.x * deltaTime * 5.0f;
                    rightLegTransform.position.y -= rightLegTransform.position.y * deltaTime * 5.0f;
                }
            }

            if (leftHand.has_value()) {
                Transform3DComponent &leftHandTransform = world.getMutableComponent<Transform3DComponent>(leftHand.value());
                if (goingUp) {
                    // leftHandTransform.rotation.x = 0.0f;
                    leftHandTransform.rotation.x = SDL_sinf(this->switchTimer * 5.0f) * 22.5f - 180.0f;
                    leftHandTransform.position.y = 1.5f + (SDL_sinf(this->switchTimer * 3.0f) * 0.5f + 0.5f) * 0.25f;
                } else {
                    leftHandTransform.rotation.x -= leftHandTransform.rotation.x * deltaTime * 5.0f;
                    leftHandTransform.position.y += (1.0f - leftHandTransform.position.y) * deltaTime * 5.0f;
                }
            }
            if (rightHand.has_value()) {
                Transform3DComponent &rightHandTransform = world.getMutableComponent<Transform3DComponent>(rightHand.value());
                if (goingUp) {
                    // rightHandTransform.rotation.x = 0.0f;
                    rightHandTransform.rotation.x = -SDL_sinf(this->switchTimer * 5.0f) * 22.5f - 180.0f;
                    rightHandTransform.position.y = 1.5f + (SDL_sinf(this->switchTimer * 3.0f) * 0.5f + 0.5f) * 0.25f;
                } else {
                    rightHandTransform.rotation.x -= rightHandTransform.rotation.x * deltaTime * 5.0f;
                    rightHandTransform.position.y += (1.0f - rightHandTransform.position.y) * deltaTime * 5.0f;
                }
            }
        }

        if (glm::length(glm::vec2(this->velocity.x, this->velocity.z)) > 1.0e-6f) {
            transform.rotation.y += (SDL_atan2f(this->velocity.x, this->velocity.z) * 180.0f / SDL_PI_F - transform.rotation.y) * deltaTime * 7.0f;
        }

        std::optional<Entity> head = world.find("Head", self);
        if (head.has_value()) {
            Transform3DComponent &headTransform = world.getMutableComponent<Transform3DComponent>(head.value());
            glm::vec3 lookDir = this->viewTarget - headTransform.position;
            if (glm::length(lookDir) > 1.0e-6f) {
                lookDir = glm::normalize(lookDir);
                float pitch = SDL_asinf(-lookDir.y) * 180.0f / SDL_PI_F - transform.rotation.x;
                float yaw = SDL_atan2f(lookDir.x, lookDir.z) * 180.0f / SDL_PI_F - transform.rotation.y;

                headTransform.rotation.x += (pitch - headTransform.rotation.x) * deltaTime * 3.0f;
                headTransform.rotation.y += (yaw - headTransform.rotation.y) * deltaTime * 3.0f;
            }
        }
    }
    void onDestroy(World&, Entity entity) override {
        printf("Entity %u destroyed!\n", static_cast<uint32_t>(entity));
    }
};
glm::vec3 MyScript::ROOM_SIZE = glm::vec3(4.0f, 2.0f, 4.0f) * 10.0f;

struct FpsCounterScript : public Script {
    uint32_t fps = 0;
    float fpsTimer = 0.0f;

    void onLoad(World&, Entity) override {
        printf("FpsCounterScript loaded!\n");
    }
    void onUpdate(const Window& window, World& world, Entity self, float deltaTime) override {
        this->fpsTimer += deltaTime;
        this->fps++;
        if (this->fpsTimer >= 1.0f) {
            // TextComponent &fpsText = world.getMutableComponent<TextComponent>(self);
            // fpsText.setText(L"FPS: " + std::to_wstring(this->fps));

            // if (this->fps < 15) {
            //     fpsText.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
            // } else if (this->fps < 60) {
            //     fpsText.color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
            // } else if (this->fps < 144) {
            //     fpsText.color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
            // } else {
            //     fpsText.color = glm::vec4(1.0f);
            // }

            printf("FPS: %u\n", this->fps);

            this->fps = 0;
            this->fpsTimer -= 1.0f;
        }
    }
    void onDestroy(World&, Entity) override {
        printf("FpsCounterScript destroyed!\n");
    }
};
struct TextScript : public Script {
    void onLoad(World&, Entity) override {
        printf("TextScript loaded!\n");
    }
    void onUpdate(const Window&, World& world, Entity self, float) override {
        const TextComponent &textComponent = world.getComponent<TextComponent>(self);
        Transform2DComponent &transform = world.getMutableComponent<Transform2DComponent>(self);
        transform.position.x = -textComponent.getFont()->getTextWidth(textComponent.getText()) * 0.5f * transform.scale.x;
        transform.position.y = -0.95f + transform.scale.y;
    }
    void onDestroy(World&, Entity) override {
        printf("TextScript destroyed!\n");
    }
};
class PanelScript : public Script {
    float time = 0.0f;
    void onUpdate(const Window&, World& world, Entity self, float deltaTime) override {
        time += deltaTime;

        size_t i = 0;
        for (auto [_, child] : world.getChildren(self)) {
            if (!world.hasComponents<GuiElementComponent>(child)) {
                continue;
            }
            if (!world.getName(child).starts_with("Child")) {
                continue;
            }
            GuiElementComponent &childElement = world.getMutableComponent<GuiElementComponent>(child);
            float height = childElement.style.getHeight().value_or(50.0f).getConstraint().value_or(50.0f);
            childElement.style.setHeight(height + ((SDL_sinf(time * 2.0f + i) * 0.5f + 0.5f) * 250.0f - height) * 20.0f * deltaTime);
            i++;
        }
    }
};

int main() {
    if (hasCriticalErrors()) return 1;
    Window window = Window({
        .title = "Example",
        .width = 1280,
        .height = 720,
    });

    glClearColor(0.3f, 0.6f, 1.0f, 1.0f);

    Renderer renderer = Renderer();

    World world = World();
    world.camera.position = glm::vec3(32.0f, 10.0f, 32.0f);
    world.camera.rotation = glm::vec3(-30.0f, 45.0f, 0.0f);
    world.camera.mode = CameraMode::ORTHOGRAPHIC;
    world.camera.orthoSize = 32.0f;

    Mesh mesh = Mesh({
        // Front face
        { -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f,  1.0f },
        {  0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  0.0f,  1.0f },
        {  0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f,  1.0f },
        { -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f,  1.0f },
        {  0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f,  1.0f },
        { -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  0.0f,  1.0f },

        // Back face
        {  0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f },
        { -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f,  0.0f, -1.0f },
        { -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f },
        {  0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f },
        { -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f },
        {  0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  0.0f, -1.0f },

        // Left face
        { -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  0.0f,  0.0f },
        { -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f },
        { -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, -1.0f,  0.0f,  0.0f },
        { -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  0.0f,  0.0f },
        { -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, -1.0f,  0.0f,  0.0f },
        { -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f },

        // Right face
        {  0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,  0.0f,  0.0f },
        {  0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f },
        {  0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f,  0.0f,  0.0f },
        {  0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,  0.0f,  0.0f },
        {  0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f,  0.0f,  0.0f },
        {  0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f },

        // Top face
        { -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  1.0f,  0.0f },
        {  0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f },
        {  0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  1.0f,  0.0f },
        { -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  1.0f,  0.0f },
        {  0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  1.0f,  0.0f },
        { -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f },

        // Bottom face
        { -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f, -1.0f,  0.0f },
        {  0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f },
        {  0.5f, -0.5f,  0.5f,  1.0f, 1.0f,  0.0f, -1.0f,  0.0f },
        { -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f, -1.0f,  0.0f },
        {  0.5f, -0.5f,  0.5f,  1.0f, 1.0f,  0.0f, -1.0f,  0.0f },
        { -0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f }
    });
    Mesh frontFaceMesh = Mesh({
        // Front face
        { -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f,  1.0f },
        {  0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  0.0f,  1.0f },
        {  0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f,  1.0f },
        { -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f,  1.0f },
        {  0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f,  1.0f },
        { -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  0.0f,  1.0f },

        // Back face
        {  0.5f, -0.5f, -0.5f,  0.25f, 0.5f,  0.0f,  0.0f, -1.0f },
        { -0.5f, -0.5f, -0.5f,  0.25f, 0.5f,  0.0f,  0.0f, -1.0f },
        { -0.5f,  0.5f, -0.5f,  0.25f, 0.5f,  0.0f,  0.0f, -1.0f },
        {  0.5f, -0.5f, -0.5f,  0.25f, 0.5f,  0.0f,  0.0f, -1.0f },
        { -0.5f,  0.5f, -0.5f,  0.25f, 0.5f,  0.0f,  0.0f, -1.0f },
        {  0.5f,  0.5f, -0.5f,  0.25f, 0.5f,  0.0f,  0.0f, -1.0f },

        // Left face
        { -0.5f, -0.5f, -0.5f,  0.25f, 0.5f, -1.0f,  0.0f,  0.0f },
        { -0.5f, -0.5f,  0.5f,  0.25f, 0.5f, -1.0f,  0.0f,  0.0f },
        { -0.5f,  0.5f,  0.5f,  0.25f, 0.5f, -1.0f,  0.0f,  0.0f },
        { -0.5f, -0.5f, -0.5f,  0.25f, 0.5f, -1.0f,  0.0f,  0.0f },
        { -0.5f,  0.5f,  0.5f,  0.25f, 0.5f, -1.0f,  0.0f,  0.0f },
        { -0.5f,  0.5f, -0.5f,  0.25f, 0.5f, -1.0f,  0.0f,  0.0f },

        // Right face
        {  0.5f, -0.5f,  0.5f,  0.25f, 0.5f,  1.0f,  0.0f,  0.0f },
        {  0.5f, -0.5f, -0.5f,  0.25f, 0.5f,  1.0f,  0.0f,  0.0f },
        {  0.5f,  0.5f, -0.5f,  0.25f, 0.5f,  1.0f,  0.0f,  0.0f },
        {  0.5f, -0.5f,  0.5f,  0.25f, 0.5f,  1.0f,  0.0f,  0.0f },
        {  0.5f,  0.5f, -0.5f,  0.25f, 0.5f,  1.0f,  0.0f,  0.0f },
        {  0.5f,  0.5f,  0.5f,  0.25f, 0.5f,  1.0f,  0.0f,  0.0f },

        // Top face
        { -0.5f,  0.5f,  0.5f,  0.25f, 0.5f,  0.0f,  1.0f,  0.0f },
        {  0.5f,  0.5f,  0.5f,  0.25f, 0.5f,  0.0f,  1.0f,  0.0f },
        {  0.5f,  0.5f, -0.5f,  0.25f, 0.5f,  0.0f,  1.0f,  0.0f },
        { -0.5f,  0.5f,  0.5f,  0.25f, 0.5f,  0.0f,  1.0f,  0.0f },
        {  0.5f,  0.5f, -0.5f,  0.25f, 0.5f,  0.0f,  1.0f,  0.0f },
        { -0.5f,  0.5f, -0.5f,  0.25f, 0.5f,  0.0f,  1.0f,  0.0f },

        // Bottom face
        { -0.5f, -0.5f, -0.5f,  0.25f, 0.5f,  0.0f, -1.0f,  0.0f },
        {  0.5f, -0.5f, -0.5f,  0.25f, 0.5f,  0.0f, -1.0f,  0.0f },
        {  0.5f, -0.5f,  0.5f,  0.25f, 0.5f,  0.0f, -1.0f,  0.0f },
        { -0.5f, -0.5f, -0.5f,  0.25f, 0.5f,  0.0f, -1.0f,  0.0f },
        {  0.5f, -0.5f,  0.5f,  0.25f, 0.5f,  0.0f, -1.0f,  0.0f },
        { -0.5f, -0.5f,  0.5f,  0.25f, 0.5f,  0.0f, -1.0f,  0.0f }
    });
    Mesh roomBack = Mesh({
        // Bottom face
        {  0.5f, -0.5f,  0.5f,  1.0f, 1.0f,  0.0f, 1.0f, 0.0f },
        {  0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f },
        { -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f },
        { -0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f },
        {  0.5f, -0.5f,  0.5f,  1.0f, 1.0f,  0.0f, 1.0f, 0.0f },
        { -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f },

        // Left face
        { -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f },
        { -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f },
        { -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f },
        { -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f },
        { -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f },
        { -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f },
        
        // Front face
        {  0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f, 0.0f, -1.0f },
        {  0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, 0.0f, -1.0f },
        { -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, 0.0f, -1.0f },
        { -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f, 0.0f, -1.0f },
        {  0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f, 0.0f, -1.0f },
        { -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, 0.0f, -1.0f },
    });

    Texture seryohaTexture = Texture::fromFile(ASSETS_DIR "/img/seryoha.png", {
        .internalFormat = TextureInternalFormat::RGBA8,
        .format = TextureFormat::RGBA,
        .type = TextureType::UnsignedByte,
    });
    Texture svoTexture = Texture::fromFile(ASSETS_DIR "/img/svo.png", {
        .internalFormat = TextureInternalFormat::RGBA8,
        .format = TextureFormat::RGBA,
        .type = TextureType::UnsignedByte,
    });

    printf("Loading fonts...\n");
    FreeType freeType = FreeType();
    // Font notoSansFont = Font(freeType, ASSETS_DIR "/fonts/NotoSans-VariableFont_wdth,wght.ttf");
    printf("Fonts loaded!\n");

    std::optional<Entity> room = world.spawn("Room", std::nullopt);
    if (room.has_value()) {
        world.addComponent<MeshComponent>(room.value(), roomBack, glm::vec4(0.5f, 0.8f, 0.5f, 1.0f));
        world.addComponent<Transform3DComponent>(room.value(), glm::vec3(), glm::vec3(0.0f, -90.0f, 0.0f), MyScript::ROOM_SIZE * 2.0f + 1.0f);
    }

    for (size_t i = 0; i < 500; i++) {
        std::optional<Entity> seryoha = world.spawn("Seryoha " + std::to_string(i), std::nullopt);
        if (seryoha.has_value()) {
            constexpr glm::vec4 brown = glm::vec4(0.55f, 0.27f, 0.07f, 1.0f);
            constexpr glm::vec4 vanilla = glm::vec4(1.0f, 0.91f, 0.71f, 1.0f);
            constexpr glm::vec4 warm = glm::vec4(1.0f, 0.8f, 0.6f, 1.0f);

            // choose random number from 0.0f to 2.0f and mix colors accordingly
            float colorChoice = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 2.0f;
            glm::vec4 color = glm::vec4();
            if (colorChoice < 1.0f) {
                color = glm::mix(brown, vanilla, colorChoice);
            } else {
                color = glm::mix(vanilla, warm, colorChoice - 1.0f);
            }

            std::optional<Entity> body = world.spawn("Body", seryoha);
            if (body.has_value()) {
                world.addComponent<MeshComponent>(body.value(), mesh, color);
                world.addComponent<Transform3DComponent>(body.value(), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(), glm::vec3(1.0f, 1.0f, 0.5f));
            }
            world.addComponent<Transform3DComponent>(seryoha.value());
            world.setScript<MyScript>(seryoha.value(), rand() % 10 == 0);

            std::optional<Entity> leftHand = world.spawn("Left Hand", seryoha);
            if (leftHand.has_value()) {
                world.addComponent<MeshComponent>(leftHand.value(), mesh, color * glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
                world.addComponent<Transform3DComponent>(leftHand.value(), glm::vec3(-0.65f, 1.0f, 0.0f), glm::vec3(), glm::vec3(0.3f, 0.7f, 0.3f));
            }
            std::optional<Entity> rightHand = world.spawn("Right Hand", seryoha);
            if (rightHand.has_value()) {
                world.addComponent<MeshComponent>(rightHand.value(), mesh, color * glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
                world.addComponent<Transform3DComponent>(rightHand.value(), glm::vec3(0.65f, 1.0f, 0.0f), glm::vec3(), glm::vec3(0.3f, 0.7f, 0.3f));
            }
            std::optional<Entity> leftLeg = world.spawn("Left Leg", seryoha);
            if (leftLeg.has_value()) {
                world.addComponent<MeshComponent>(leftLeg.value(), mesh, color * glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
                world.addComponent<Transform3DComponent>(leftLeg.value(), glm::vec3(-0.3f, 0.0f, 0.0f), glm::vec3(), glm::vec3(0.3f, 1.0f, 0.3f));
            }
            std::optional<Entity> rightLeg = world.spawn("Right Leg", seryoha);
            if (rightLeg.has_value()) {
                world.addComponent<MeshComponent>(rightLeg.value(), mesh, color * glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
                world.addComponent<Transform3DComponent>(rightLeg.value(), glm::vec3(0.3f, 0.0f, 0.0f), glm::vec3(), glm::vec3(0.3f, 1.0f, 0.3f));
            }
            std::optional<Entity> head = world.spawn("Head", seryoha);
            if (head.has_value()) {
                world.addComponent<MeshComponent>(head.value(), frontFaceMesh, seryohaTexture);
                world.addComponent<Transform3DComponent>(head.value(), glm::vec3(0.0f, 1.75f, 0.0f), glm::vec3(), glm::vec3(0.5f, 0.5f, 0.5f));
            }
        }
    }

    std::optional<Entity> fpsTextEntity = world.spawn("FPS Text", std::nullopt);
    if (fpsTextEntity.has_value()) {
        // TextComponent &textComponent = world.addComponent<TextComponent>(fpsTextEntity.value(), notoSansFont, L"FPS: 0", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        // textComponent.textHeight = 32.0f;
        // world.addComponent<CanvasTransformComponent>(fpsTextEntity.value());
        world.setScript<FpsCounterScript>(fpsTextEntity.value());
    }

    std::optional<Entity> panel = world.spawn("Panel", std::nullopt);
    if (panel.has_value()) {
        GuiElementComponent &gui = world.addComponent<GuiElementComponent>(panel.value());
        gui.style.setBackgroundColor(0x222222FF);
        gui.style.setWidth(Sizing::Grow);
        gui.style.setHeight(Sizing::Grow);
        gui.style.setMargin(Edges { 32.0f, 32.0f, 32.0f, 32.0f });
        gui.style.setLayoutDirection(LayoutDirection::Column);
    }
    std::optional<Entity> body = world.spawn("body", panel);
    if (body.has_value()) {
        GuiElementComponent &gui = world.addComponent<GuiElementComponent>(body.value());
        gui.style.setWidth(Sizing::Grow);
        gui.style.setHeight(Sizing::Grow);

        std::optional<Entity> sidebar = world.spawn("sidebar", body);
        if (sidebar.has_value()) {
            GuiElementComponent &gui2 = world.addComponent<GuiElementComponent>(sidebar.value());
            gui2.style.setBackgroundColor(0x444444FF);
            gui2.style.setWidth(200.0f);
            gui2.style.setHeight(Sizing::Grow);
            gui2.style.setPadding(Edges { 8.0f, 8.0f, 8.0f, 8.0f });
            gui2.style.setLayoutDirection(LayoutDirection::Column);
            gui2.style.setContentAlignY(Align::End);
            gui2.style.setGap(8.0f);

            for (size_t i = 0; i < 8; i++) {
                std::optional<Entity> option = world.spawn("option " + std::to_string(i), sidebar);
                if (option.has_value()) {
                    GuiElementComponent &gui3 = world.addComponent<GuiElementComponent>(option.value());
                    gui3.style.setBackgroundColor(0x666666FF);
                    gui3.style.setWidth(static_cast<float>(rand() % 100) + 64.0f);
                    gui3.style.setHeight(32.0f);
                }
            }
        }
        
        std::optional<Entity> content = world.spawn("content", body);
        if (content.has_value()) {
            GuiElementComponent &gui2 = world.addComponent<GuiElementComponent>(content.value());
            gui2.style.setWidth(Sizing::Grow);
            gui2.style.setHeight(Sizing::Grow);
            gui2.style.setMargin(Edges { 8.0f, 8.0f, 8.0f, 8.0f });

            std::optional<Entity> graphsRows = world.spawn("graphs rows", content);
            if (graphsRows.has_value()) {
                GuiElementComponent &gui3 = world.addComponent<GuiElementComponent>(graphsRows.value());
                gui3.style.setWidth(Sizing::Grow);
                gui3.style.setHeight(Sizing::Grow);
                gui3.style.setLayoutDirection(LayoutDirection::Column);
                gui3.style.setGap(8.0f);

                for (size_t i = 0; i < 2; i++) {
                    std::optional<Entity> graphRow = world.spawn("graph row " + std::to_string(i), graphsRows);
                    if (graphRow.has_value()) {
                        GuiElementComponent &gui4 = world.addComponent<GuiElementComponent>(graphRow.value());
                        gui4.style.setWidth(Sizing::Grow);
                        gui4.style.setHeight(Sizing::Grow);
                        gui4.style.setLayoutDirection(LayoutDirection::Row);
                        gui4.style.setGap(8.0f);

                        for (size_t j = 0; j < 3; j++) {
                            std::optional<Entity> graph = world.spawn("graph " + std::to_string(j), graphRow);
                            if (graph.has_value()) {
                                GuiElementComponent &gui5 = world.addComponent<GuiElementComponent>(graph.value());
                                gui5.style.setBackgroundColor(0x555555FF);
                                gui5.style.setWidth(Sizing::Grow);
                                gui5.style.setHeight(Sizing::Grow);
                            }
                        }
                        std::optional<Entity> graph = world.spawn("graph 3", graphRow);
                        if (graph.has_value()) {
                            GuiElementComponent &gui5 = world.addComponent<GuiElementComponent>(graph.value());
                            gui5.style.setBackgroundColor(0x555555FF);
                            gui5.style.setWidth(256.0f);
                            gui5.style.setHeight(Sizing::Grow);
                        }
                    }
                }
            }
        }
    }

    std::optional<Entity> titlebar = world.spawn("Titlebar", panel);
    if (titlebar.has_value()) {
        GuiElementComponent &gui = world.addComponent<GuiElementComponent>(titlebar.value());
        gui.style.setBackgroundColor(0x333333FF);
        gui.style.setWidth(Sizing::Grow);
        gui.style.setPadding(Edges { 8.0f, 8.0f, 8.0f, 8.0f });

        std::optional<Entity> icon = world.spawn("icon", titlebar);
        if (icon.has_value()) {
            GuiElementComponent &gui2 = world.addComponent<GuiElementComponent>(icon.value());
            gui2.style.setBackgroundImage(svoTexture);
            gui2.style.setWidth(48.0f);
            gui2.style.setHeight(48.0f);
        }
        std::optional<Entity> spacing = world.spawn("spacing", titlebar);
        if (spacing.has_value()) {
            GuiElementComponent &gui2 = world.addComponent<GuiElementComponent>(spacing.value());
            gui2.style.setWidth(Sizing::Grow);
            gui2.style.setHeight(Sizing::Grow);
        }
        std::optional<Entity> buttons = world.spawn("buttons", titlebar);
        if (buttons.has_value()) {
            GuiElementComponent &gui2 = world.addComponent<GuiElementComponent>(buttons.value());
            gui2.style.setWidth(Sizing::Fit);
            gui2.style.setHeight(Sizing::Grow);
            gui2.style.setGap(8.0f);
            gui2.style.setPadding(Edges { .right = 8.0f });
            gui2.style.setContentAlignY(Align::Center);

            std::optional<Entity> maximizeButton = world.spawn("maximizeButton", buttons);
            if (maximizeButton.has_value()) {
                GuiElementComponent &gui3 = world.addComponent<GuiElementComponent>(maximizeButton.value());
                gui3.style.setBackgroundColor(0x66EE55FF);
                gui3.style.setWidth(16.0f);
                gui3.style.setHeight(16.0f);
            }
            std::optional<Entity> minimizeButton = world.spawn("minimizeButton", buttons);
            if (minimizeButton.has_value()) {
                GuiElementComponent &gui3 = world.addComponent<GuiElementComponent>(minimizeButton.value());
                gui3.style.setBackgroundColor(0xEECC44FF);
                gui3.style.setWidth(16.0f);
                gui3.style.setHeight(16.0f);
            }
            std::optional<Entity> closeButton = world.spawn("closeButton", buttons);
            if (closeButton.has_value()) {
                GuiElementComponent &gui3 = world.addComponent<GuiElementComponent>(closeButton.value());
                gui3.style.setBackgroundColor(0xEE5544FF);
                gui3.style.setWidth(16.0f);
                gui3.style.setHeight(16.0f);
            }
        }
    }

    float zoomTime = 0.0f;

    uint64_t lastTime = SDL_GetTicksNS();
    while (window.isRunning()) {
        uint64_t currentTime = SDL_GetTicksNS();
        uint64_t deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        float dt = static_cast<float>(static_cast<double>(deltaTime) / 1.0e9);

        zoomTime += dt;
        world.camera.orthoSize += (5.0f + glm::clamp(SDL_sinf(zoomTime * 0.1f) * 2.0f, 0.0f, 1.0f) * 27.0f - world.camera.orthoSize) * dt;

        window.pollEvents();
        world.update(window, dt);

        renderer.render(window, world);
        window.swapBuffers();
    }

    return 0;
}