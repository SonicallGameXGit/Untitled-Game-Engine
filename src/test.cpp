#include <includes.hpp>

struct MyScript : public Script {
    static glm::vec3 ROOM_SIZE;
    glm::vec3 velocity = glm::vec3();
    float nextSwitchTime = 0.0f, switchTimer = 0.0f, sleepTime = 0.0f, sleepTimer = 0.0f;

    bool jumping = false;
    MyScript(bool jumping) : Script(), jumping(jumping) {}

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
        direction *= 4.0f;
        this->velocity.x = direction.x;
        this->velocity.z = direction.y;

        this->nextSwitchTime = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 6.0f;
        this->sleepTime = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 4.0f;
        this->switchTimer = 0.0f;
    }

    void onLoad(World &world, Entity self) override {
        // printf("Entity %u created!\n", static_cast<uint32_t>(self));
        printf("Hi, my name is %s!\n", world.getName(self).c_str());
    }
    void onUpdate(const Window&, World &world, Entity self, float deltaTime) override {
        Transform3DComponent &transform = world.getMutableComponent<Transform3DComponent>(self);

        if (jumping) {
            this->velocity.y -= 9.81f * deltaTime;
            if (transform.position.y <= -MyScript::ROOM_SIZE.y) {
                transform.position.y = -MyScript::ROOM_SIZE.y;
                this->velocity.y = (5.0f + (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 5.0f) * (this->switchTimer < nextSwitchTime || this->sleepTimer >= sleepTime ? 1.0f : 0.0f);
            }
        } else {
            transform.position.y = -MyScript::ROOM_SIZE.y;
        }
        transform.position += this->velocity * deltaTime;

        // world.camera.rotation.y += deltaTime * 10.0f;

        std::optional<Entity> svoEntity = world.find("SVO", std::nullopt);
        if (svoEntity.has_value()) {
            Transform2DComponent &svo = world.getMutableComponent<Transform2DComponent>(svoEntity.value());
            float rotation = world.camera.rotation.y + 180.0f;
            // svo.position.x = ((rotation - SDL_floorf(rotation / 360.0f) * 360.0f) / 360.0f * 4.0f) - 1.0f;
            // svo.position.x = glm::clamp(svo.position.x, -1.0f, 1.0f - svo.scale.x);
            svo.rotation += deltaTime * 20.0f;
            svo.scale.x = 0.4f + (SDL_sinf(world.camera.rotation.y * SDL_PI_F / 180.0f) + 1.0f) * 0.1f;
            
            std::optional<Entity> svoChildEntity = world.find("SVO", svoEntity.value());
            if (svoChildEntity.has_value()) {
                Transform2DComponent &svo = world.getMutableComponent<Transform2DComponent>(svoChildEntity.value());
                float rotation = world.camera.rotation.y + 180.0f;
                svo.rotation += deltaTime * 20.0f;

                std::optional<Entity> svoEvenMoreChildEntity = world.find("SVO", svoChildEntity.value());
                if (svoEvenMoreChildEntity.has_value()) {
                    Transform2DComponent &svo = world.getMutableComponent<Transform2DComponent>(svoEvenMoreChildEntity.value());
                    float rotation = world.camera.rotation.y + 180.0f;
                    svo.rotation += deltaTime * 20.0f;
                }
            }
        }

        this->switchTimer += deltaTime;
        if (this->switchTimer >= nextSwitchTime) {
            this->sleepTimer += deltaTime;
            this->velocity.x = 0.0f;
            this->velocity.z = 0.0f;

            std::optional<Entity> leftHand = world.find("Left Hand", self);
            if (leftHand.has_value()) {
                Transform3DComponent &leftHandTransform = world.getMutableComponent<Transform3DComponent>(leftHand.value());
                leftHandTransform.rotation.x = SDL_sinf(this->sleepTimer * 5.0f) * 45.0f;
            }
            std::optional<Entity> rightHand = world.find("Right Hand", self);
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
        }

        if (glm::length(glm::vec2(this->velocity.x, this->velocity.z)) > 1.0e-6f) {
            transform.rotation.y += (SDL_atan2f(this->velocity.x, this->velocity.z) * 180.0f / SDL_PI_F - transform.rotation.y) * deltaTime * 7.0f;
        }
    }
    void onDestroy(World&, Entity entity) override {
        printf("Entity %d destroyed!\n", static_cast<uint32_t>(entity));
    }
};
glm::vec3 MyScript::ROOM_SIZE = glm::vec3(4.0f, 2.0f, 4.0f);

struct FpsCounterScript : public Script {
    uint32_t fps = 0;
    float fpsTimer = 0.0f;
    float svoDestroyTimer = 0.0f;
    FpsCounterScript() : Script() {}

    void onLoad(World&, Entity) override {
        printf("FpsCounterScript loaded!\n");
    }
    void onUpdate(const Window& window, World& world, Entity self, float deltaTime) override {
        this->fpsTimer += deltaTime;
        this->fps++;
        if (this->fpsTimer >= 1.0f) {
            TextComponent &fpsText = world.getMutableComponent<TextComponent>(self);
            fpsText.setText(L"FPS: " + std::to_wstring(this->fps));

            if (this->fps < 15) {
                fpsText.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
            } else if (this->fps < 60) {
                fpsText.color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
            } else if (this->fps < 144) {
                fpsText.color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
            } else {
                fpsText.color = glm::vec4(1.0f);
            }

            this->fps = 0;
            this->fpsTimer -= 1.0f;
        }

        Transform2DComponent &transform = world.getMutableComponent<Transform2DComponent>(self);
        transform.position.x = 0.05f - window.getHorizontalAspect();
        transform.position.y = 0.95f;

        svoDestroyTimer += deltaTime;
        if (svoDestroyTimer >= 15.0f) {
            std::optional<Entity> svoEntity = world.find("SVO", std::nullopt);
            if (svoEntity.has_value()) {
                world.destroy(svoEntity.value());
            }
        } else if (svoDestroyTimer >= 10.0f) {
            std::optional<Entity> svoEntity = world.find("SVO", std::nullopt);
            if (svoEntity.has_value()) {
                const std::unordered_map<std::string, Entity> &children = world.getChildren(svoEntity);
                while (!children.empty()) { // Use this way to destroy all children to avoid iterator invalidation
                    world.destroy(children.begin()->second);
                }
            }
        }
    }
    void onDestroy(World&, Entity) override {
        printf("FpsCounterScript destroyed!\n");
    }
};
struct TextScript : public Script {
    TextScript() : Script() {}

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
struct SvoScript : public Script {
    SvoScript() : Script() {}

    void onLoad(World&, Entity) override {
        printf("SvoScript loaded!\n");
    }
    void onUpdate(const Window& window, World& world, Entity self, float) override {
        Transform2DComponent &transform = world.getMutableComponent<Transform2DComponent>(self);
        transform.position.x = window.getHorizontalAspect() - 0.45f;
    }
    void onDestroy(World&, Entity) override {
        printf("SvoScript destroyed!\n");
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
    world.camera.position = glm::vec3(5.0f);
    world.camera.rotation = glm::vec3(-30.0f, 45.0f, 0.0f);

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
    Font tnrFont = Font(freeType, ASSETS_DIR "/fonts/Times New Roman.ttf");
    // Font dosFont = Font(freeType, ASSETS_DIR "/fonts/dos2000-ru-en.otf");
    Font sansFont = Font(freeType, ASSETS_DIR "/fonts/Blogger Sans.ttf");
    printf("Fonts loaded!\n");

    std::optional<Entity> room = world.spawn("Room", std::nullopt);
    if (room.has_value()) {
        world.addComponent<MeshComponent>(room.value(), roomBack, glm::vec4(0.5f, 0.8f, 0.5f, 1.0f));
        world.addComponent<Transform3DComponent>(room.value(), glm::vec3(), glm::vec3(0.0f, -90.0f, 0.0f), MyScript::ROOM_SIZE * 2.0f + 1.0f);
    }

    for (size_t i = 0; i < 10; i++) {
        std::optional<Entity> seryoha = world.spawn("Seryoha " + std::to_string(i), std::nullopt);
        if (seryoha.has_value()) {
            world.addComponent<MeshComponent>(seryoha.value(), mesh, glm::vec4(1.0f));
            world.addComponent<Transform3DComponent>(seryoha.value(), glm::vec3(), glm::vec3(), glm::vec3(1.0f, 1.0f, 0.5f));
            world.setScript<MyScript>(seryoha.value(), i == 3);

            std::optional<Entity> leftHand = world.spawn("Left Hand", seryoha);
            if (leftHand.has_value()) {
                world.addComponent<MeshComponent>(leftHand.value(), mesh, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
                world.addComponent<Transform3DComponent>(leftHand.value(), glm::vec3(-0.75f, 0.0f, 0.0f), glm::vec3(), glm::vec3(0.3f, 0.7f, 0.3f));
            }
            std::optional<Entity> rightHand = world.spawn("Right Hand", seryoha);
            if (rightHand.has_value()) {
                world.addComponent<MeshComponent>(rightHand.value(), mesh, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
                world.addComponent<Transform3DComponent>(rightHand.value(), glm::vec3(0.75f, 0.0f, 0.0f), glm::vec3(), glm::vec3(0.3f, 0.7f, 0.3f));
            }
            std::optional<Entity> head = world.spawn("Head", seryoha);
            if (head.has_value()) {
                world.addComponent<MeshComponent>(head.value(), mesh, seryohaTexture);
                world.addComponent<Transform3DComponent>(head.value(), glm::vec3(0.0f, 0.75f, 0.0f), glm::vec3(), glm::vec3(0.5f, 0.5f, 0.5f));
            }
        }
    }
    
    std::optional<Entity> textEntity = world.spawn("Text", std::nullopt);
    if (textEntity.has_value()) {
        const wchar_t *sourceText = L"Hello, World! Добро пожаловать, епта!";
        world.addComponent<TextComponent>(textEntity.value(), tnrFont, sourceText, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
        world.addComponent<Transform2DComponent>(textEntity.value(), glm::vec2(), 0.0f, glm::vec2(0.1f));
        world.setScript<TextScript>(textEntity.value());
    }

    std::optional<Entity> fpsTextEntity = world.spawn("FPS Text", std::nullopt);
    if (fpsTextEntity.has_value()) {
        world.addComponent<TextComponent>(fpsTextEntity.value(), sansFont, L"FPS: 0", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        world.addComponent<Transform2DComponent>(fpsTextEntity.value(), glm::vec2(), 0.0f, glm::vec2(0.2f));
        world.setScript<FpsCounterScript>(fpsTextEntity.value());
    }

    std::optional<Entity> svoEntity = world.spawn("SVO", std::nullopt);
    if (svoEntity.has_value()) {
        world.addComponent<SpriteComponent>(svoEntity.value(), svoTexture, glm::vec4(1.0f, 1.0f, 1.0f, 0.4f));
        world.addComponent<Transform2DComponent>(svoEntity.value(), glm::vec2(0.55f), 0.0f, glm::vec2(0.4f));
        world.setScript<SvoScript>(svoEntity.value());
    }

    std::optional<Entity> svoChildEntity = world.spawn("SVO", svoEntity);
    if (svoChildEntity.has_value()) {
        world.addComponent<SpriteComponent>(svoChildEntity.value(), svoTexture, glm::vec4(1.0f, 1.0f, 1.0f, 0.4f));
        world.addComponent<Transform2DComponent>(svoChildEntity.value(), glm::vec2(0.15f), 0.0f, glm::vec2(0.4f));
        world.setScript<SvoScript>(svoChildEntity.value());
    }

    std::optional<Entity> svoEvenMoreChildEntity = world.spawn("SVO", svoChildEntity);
    if (svoEvenMoreChildEntity.has_value()) {
        world.addComponent<SpriteComponent>(svoEvenMoreChildEntity.value(), svoTexture, glm::vec4(1.0f, 1.0f, 1.0f, 0.4f));
        world.addComponent<Transform2DComponent>(svoEvenMoreChildEntity.value(), glm::vec2(-0.25f), 0.0f, glm::vec2(0.4f));
        world.setScript<SvoScript>(svoEvenMoreChildEntity.value());
    }

    uint64_t lastTime = SDL_GetTicksNS();
    while (window.isRunning()) {
        uint64_t currentTime = SDL_GetTicksNS();
        uint64_t deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        window.pollEvents();
        world.update(window, static_cast<float>(static_cast<double>(deltaTime) / 1.0e9));

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderer.render(window, world);
        window.swapBuffers();
    }

    return 0;
}