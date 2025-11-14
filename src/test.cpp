#include <includes.hpp>

struct MyScript : public Script {
    static glm::vec3 ROOM_SIZE;
    glm::vec3 velocity = glm::vec3();
    glm::vec3 viewTarget = glm::vec3(0.0f, 0.0f, 10000.0f);
    float nextSwitchTime = 0.0f, switchTimer = 0.0f, sleepTime = 0.0f, sleepTimer = 0.0f;

    bool jumping = false, paused = false;
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
    void onUpdate(const Window &window, World &world, Entity self, float deltaTime) override {
        if (window.isMouseJustPressed(SDL_BUTTON_LEFT)) {
            this->paused = !this->paused;
        }
        deltaTime *= this->paused ? 0.2f : 1.0f;

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
    void onUpdate(const Window &window, World &world, Entity self, float deltaTime) override {
        this->fpsTimer += deltaTime;
        this->fps++;
        if (this->fpsTimer >= 1.0f) {
            GuiElementComponent &fpsText = world.getMutableComponent<GuiElementComponent>(self);
            fpsText.setText(L"FPS: " + std::to_wstring(this->fps));

            if (this->fps < 15) {
                fpsText.style.setTextColor(0xFF0000FF);
            } else if (this->fps < 60) {
                fpsText.style.setTextColor(0xFFFF00FF);
            } else if (this->fps < 144) {
                fpsText.style.setTextColor(0x00FF00FF);
            } else {
                fpsText.style.setTextColor(0xFFFFFFFF);
            }

            this->fps = 0;
            this->fpsTimer -= 1.0f;
        }
    }
    void onDestroy(World&, Entity) override {
        printf("FpsCounterScript destroyed!\n");
    }
};
struct QCounterScript : public Script {
    uint64_t count = 0;
    void onLoad(World&, Entity) override {}
    void onUpdate(const Window &window, World &world, Entity self, float) override {
        GuiElementComponent &text = world.getMutableComponent<GuiElementComponent>(self);
        if (window.isKeyJustPressed(SDLK_Q)) {
            if (window.isKeyPressed(SDLK_LSHIFT) || window.isKeyPressed(SDLK_RSHIFT)) {
                this->count--;
            } else {
                this->count++;
            }
            text.setText(L"Q Count: " + std::to_wstring(this->count));
        }
    }
    void onDestroy(World&, Entity) override {}
};
struct MousePositionScript : public Script {
    uint64_t count = 0;
    void onLoad(World&, Entity) override {}
    void onUpdate(const Window &window, World &world, Entity self, float deltaTime) override {
        GuiElementComponent &text = world.getMutableComponent<GuiElementComponent>(self);
        const glm::vec2 &mousePos = window.getMousePosition();
        std::wstring buttonPressed = std::wstring();
        if (window.isMousePressed(SDL_BUTTON_LEFT)) {
            buttonPressed = L" (Left)";
        } else if (window.isMousePressed(SDL_BUTTON_RIGHT)) {
            buttonPressed = L" (Right)";
        } else if (window.isMousePressed(SDL_BUTTON_MIDDLE)) {
            buttonPressed = L" (Middle)";
        }
        text.setText(L"Mouse Position" + buttonPressed + L": (" + std::to_wstring(static_cast<int32_t>(mousePos.x)) + L", " + std::to_wstring(static_cast<int32_t>(mousePos.y)) + L")");

        if (window.isKeyPressed(SDLK_W)) {
            world.camera.orthoSize *= (1.0f - deltaTime);
        }
        if (window.isKeyPressed(SDLK_S)) {
            world.camera.orthoSize *= (1.0f + deltaTime);
        }
        if (window.getScrollDeltaY() != 0.0f) {
            world.camera.orthoSize *= (1.0f - window.getScrollDeltaY() * 0.05f);
        }
    }
    void onDestroy(World&, Entity) override {}
};

static void spawnSidebarTree(World &world, std::optional<Entity> parent, std::optional<Entity> sidebar, Font &font, Texture &objectTexture, Texture &downArrowIcon, float depth) {
    auto children = world.getChildren(parent);
    auto sortedChildren = std::vector<std::tuple<Entity, EntityIdentifier>>();
    sortedChildren.reserve(std::distance(children.begin(), children.end()));
    for (auto [_, entity] : children) {
        sortedChildren.emplace_back(entity, world.getComponent<EntityIdentifier>(entity));
    }
    std::sort(sortedChildren.begin(), sortedChildren.end(), [](const auto& a, const auto& b) {
        return std::get<1>(a).index < std::get<1>(b).index;
    });

    size_t numItems = 0;
    for (auto [entity, _] : sortedChildren) {
        if (numItems++ > 32) {
            break;
        }

        std::optional<Entity> child = world.spawn("Child " + std::to_string(static_cast<uint32_t>(entity)), sidebar);
        if (child.has_value()) {
            GuiElementComponent &gui = world.addComponent<GuiElementComponent>(child.value());
            gui.style.setWidth(Sizing::Grow);
            gui.style.setHeight(Sizing::Fit);
            gui.style.setPadding(Edges { .left = 4.0f, .bottom = 2.0f, .right = 4.0f, .top = 2.0f });
            gui.style.setMargin(Edges { .left = depth * 24.0f, .bottom = 0.0f, .right = 0.0f, .top = 0.0f });
            gui.style.setContentAlignY(Align::Center);
            gui.style.setGap(4.0f);

            std::optional<Entity> iconEntity = world.spawn("Icon", child);
            if (iconEntity.has_value()) {
                GuiElementComponent &iconGui = world.addComponent<GuiElementComponent>(iconEntity.value());
                iconGui.style.setWidth(24.0f);
                iconGui.style.setHeight(24.0f);
                iconGui.style.setBackgroundImage(objectTexture);
            }
            std::optional<Entity> titleEntity = world.spawn("Title", child);
            if (titleEntity.has_value()) {
                GuiElementComponent &titleGui = world.addComponent<GuiElementComponent>(titleEntity.value());
                titleGui.style.setWidth(Sizing::Grow);
                titleGui.style.setHeight(Sizing::Fit);

                titleGui.setFont(font);
                titleGui.setFontSize(24.0f);

                const std::string &name = world.getName(entity);
                titleGui.setText(std::wstring(name.begin(), name.end()));
                titleGui.style.setTextColor(0xFFFFFFFF);
            }
            if (!world.getChildren(entity).empty()) {
                std::optional<Entity> expandIconEntity = world.spawn("Expand Icon", child);
                if (expandIconEntity.has_value()) {
                    GuiElementComponent &expandIconGui = world.addComponent<GuiElementComponent>(expandIconEntity.value());
                    expandIconGui.style.setWidth(24.0f);
                    expandIconGui.style.setHeight(24.0f);
                    expandIconGui.style.setBackgroundImage(downArrowIcon);
                }
            }
        }

        spawnSidebarTree(world, entity, sidebar, font, objectTexture, downArrowIcon, depth + 1.0f);
    }
}

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
    Texture objectTexture = Texture::fromFile(ASSETS_DIR "/img/object.png", {
        .internalFormat = TextureInternalFormat::RGBA8,
        .format = TextureFormat::RGBA,
        .type = TextureType::UnsignedByte,
    });
    Texture downArrowIcon = Texture::fromFile(ASSETS_DIR "/img/down_arrow.png", {
        .internalFormat = TextureInternalFormat::RGBA8,
        .format = TextureFormat::RGBA,
        .type = TextureType::UnsignedByte,
    });

    printf("Loading fonts...\n");
    FreeType freeType = FreeType();
    Font notoSansFont = Font(freeType, ASSETS_DIR "/fonts/NotoSans-VariableFont_wdth,wght.ttf");
    printf("Fonts loaded!\n");

    std::optional<Entity> room = world.spawn("Room", std::nullopt);
    if (room.has_value()) {
        world.addComponent<MeshComponent>(room.value(), roomBack, glm::vec4(0.5f, 0.8f, 0.5f, 1.0f));
        world.addComponent<Transform3DComponent>(room.value(), glm::vec3(), glm::vec3(0.0f, -90.0f, 0.0f), MyScript::ROOM_SIZE * 2.0f + 1.0f);
    }

    std::optional<Entity> seryohas = world.spawn("Seryohas", std::nullopt);
    if (seryohas.has_value()) {
        for (size_t i = 0; i < 500; i++) {
            std::optional<Entity> seryoha = world.spawn("Seryoha " + std::to_string(i), seryohas);
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
    }

    std::optional<Entity> panel = world.spawn("Panel", std::nullopt);
    if (panel.has_value()) {
        GuiElementComponent &gui = world.addComponent<GuiElementComponent>(panel.value());
        gui.style.setWidth(Sizing::Grow);
        gui.style.setHeight(Sizing::Grow);
    }

    std::optional<Entity> horizontalLayout = world.spawn("Horizontal Layout", panel);
    if (horizontalLayout.has_value()) {
        GuiElementComponent &gui = world.addComponent<GuiElementComponent>(horizontalLayout.value());
        gui.style.setHeight(Sizing::Grow);

        std::optional<Entity> sidebar = world.spawn("Sidebar", horizontalLayout);
        if (sidebar.has_value()) {
            GuiElementComponent &gui = world.addComponent<GuiElementComponent>(sidebar.value());
            gui.style.setWidth(318.0f);
            gui.style.setHeight(Sizing::Grow);
            gui.style.setBackgroundColor(0x333333FF);
            gui.style.setPadding(Edges { 4.0f, 4.0f, 4.0f, 4.0f });
            gui.style.setLayoutDirection(LayoutDirection::Column);
            gui.style.setGap(4.0f);
        }
    }

    std::optional<Entity> topRightContainer = world.spawn("TopRightContainer", std::nullopt);
    if (topRightContainer.has_value()) {
        GuiElementComponent &gui = world.addComponent<GuiElementComponent>(topRightContainer.value());
        gui.style.setWidth(Sizing::Grow);
        gui.style.setHeight(Sizing::Grow);
        gui.style.setPadding(Edges { 8.0f, 8.0f, 8.0f, 8.0f });
        gui.style.setGap(8.0f);
        gui.style.setContentAlignX(Align::End);

        std::optional<Entity> qCounter = world.spawn("QCounter", topRightContainer);
        if (qCounter.has_value()) {
            GuiElementComponent &gui2 = world.addComponent<GuiElementComponent>(qCounter.value());
            gui2.style.setWidth(Sizing::Fit);
            gui2.style.setHeight(Sizing::Fit);
            gui2.style.setPadding(Edges { .left = 8.0f, .bottom = 4.0f, .right = 8.0f, .top = 4.0f });
            gui2.style.setBackgroundColor(0x00000080);
            gui2.style.setTextColor(0xFF44FFFF);
            gui2.setFont(notoSansFont);
            gui2.setFontSize(24.0f);
            gui2.setText(L"Q: 0");
            world.setScript<QCounterScript>(qCounter.value());
        }
        std::optional<Entity> fps = world.spawn("FPS Text", topRightContainer);
        if (fps.has_value()) {
            GuiElementComponent &gui2 = world.addComponent<GuiElementComponent>(fps.value());
            gui2.style.setWidth(Sizing::Fit);
            gui2.style.setHeight(Sizing::Fit);
            gui2.style.setPadding(Edges { .left = 8.0f, .bottom = 4.0f, .right = 8.0f, .top = 4.0f });
            gui2.style.setBackgroundColor(0x00000080);
            gui2.style.setTextColor(0xFFFFFFFF);
            gui2.setFont(notoSansFont);
            gui2.setFontSize(24.0f);
            gui2.setText(L"FPS: 0");
            world.setScript<FpsCounterScript>(fps.value());
        }
    }
    std::optional<Entity> bottomRightContainer = world.spawn("Bottom Right Container", std::nullopt);
    if (bottomRightContainer.has_value()) {
        GuiElementComponent &gui = world.addComponent<GuiElementComponent>(bottomRightContainer.value());
        gui.style.setWidth(Sizing::Grow);
        gui.style.setHeight(Sizing::Grow);
        gui.style.setPadding(Edges { 8.0f, 8.0f, 8.0f, 8.0f });
        gui.style.setGap(8.0f);
        gui.style.setContentAlignX(Align::End);
        gui.style.setContentAlignY(Align::End);

        std::optional<Entity> mousePosition = world.spawn("MousePosition", bottomRightContainer);
        if (mousePosition.has_value()) {
            GuiElementComponent &gui2 = world.addComponent<GuiElementComponent>(mousePosition.value());
            gui2.style.setWidth(Sizing::Fit);
            gui2.style.setHeight(Sizing::Fit);
            gui2.style.setPadding(Edges { .left = 8.0f, .bottom = 4.0f, .right = 8.0f, .top = 4.0f });
            gui2.style.setBackgroundColor(0x00000080);
            gui2.style.setTextColor(0xFF44FFFF);
            gui2.setFont(notoSansFont);
            gui2.setFontSize(24.0f);
            world.setScript<MousePositionScript>(mousePosition.value());
        }

        std::optional<Entity> instructions = world.spawn("Instructions", bottomRightContainer);
        if (instructions.has_value()) {
            GuiElementComponent &gui2 = world.addComponent<GuiElementComponent>(instructions.value());
            gui2.style.setWidth(Sizing::Fit);
            gui2.style.setHeight(Sizing::Fit);
            gui2.style.setPadding(Edges { .left = 8.0f, .bottom = 4.0f, .right = 8.0f, .top = 4.0f });
            gui2.style.setBackgroundColor(0x00000080);
            gui2.style.setTextColor(0xFFFFFFFF);
            gui2.setFont(notoSansFont);
            gui2.setFontSize(18.0f);
            gui2.setText(L"W/S or Mouse Y Wheel: Zoom In/Out\nQ/(Shift + Q): Change \"Q\" Counter Value\nLeft Click: Pause/Unpause Seryohas");
        }
    }

    std::optional<Entity> sidebar = world.find("Sidebar", horizontalLayout);
    if (sidebar.has_value()) {
        size_t numItems = 0;
        spawnSidebarTree(world, std::nullopt, sidebar, notoSansFont, objectTexture, downArrowIcon, 0.0f);
    }

    float zoomTime = 0.0f;
    uint64_t lastTime = SDL_GetTicksNS();
    while (window.isRunning()) {
        uint64_t currentTime = SDL_GetTicksNS();
        uint64_t deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        float dt = static_cast<float>(static_cast<double>(deltaTime) / 1.0e9);

        window.pollEvents();
        world.update(window, dt);
        renderer.render(window, world);
        window.swapBuffers();
    }

    return 0;
}