#include <includes.hpp>

struct MyScript : public Script {
    static glm::vec3 ROOM_SIZE;
    glm::vec3 velocity = glm::vec3();
    float nextSwitchTime = 0.0f, switchTimer = 0.0f, sleepTime = 0.0f, sleepTimer = 0.0f;
    float viewTargetSwitchTime = 0.0f, viewTargetTimer = 0.0f;
    bool viewTarget = false;

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

        this->nextSwitchTime = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 2.0f;
        this->sleepTime = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 1.5f;
        this->switchTimer = 0.0f;
    }

    void onLoad(World&, Entity entity) override {
        printf("Entity %u created!\n", static_cast<uint32_t>(entity));
    }
    void onUpdate(const Window&, World &world, Entity entity, float deltaTime) override {
        Transform3DComponent &transform = world.getMutableComponent<Transform3DComponent>(entity);

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

        this->switchTimer += deltaTime;
        if (this->switchTimer >= nextSwitchTime) {
            this->sleepTimer += deltaTime;
            this->velocity.x = 0.0f;
            this->velocity.z = 0.0f;
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

        this->viewTargetTimer += deltaTime;
        if (this->viewTargetTimer >= this->viewTargetSwitchTime) {
            this->viewTargetTimer -= this->viewTargetSwitchTime;
            this->viewTarget = !this->viewTarget;
            this->viewTargetSwitchTime = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 3.0f + 1.0f;
        }
        if (this->viewTarget) {
            glm::vec3 toTarget = glm::normalize(world.camera.position - transform.position);

            glm::vec3 targetEuler = glm::vec3(
                -SDL_asinf(toTarget.y) * 180.0f / SDL_PI_F,
                -SDL_atan2f(toTarget.z, toTarget.x) * 180.0f / SDL_PI_F,
                0.0f
            );
            transform.rotation += (targetEuler - transform.rotation) * deltaTime * 7.0f;
        } else {
            transform.rotation.x = 0.0f;
            transform.rotation.y += (SDL_atan2f(this->velocity.x, this->velocity.z) * 180.0f / SDL_PI_F - 90.0f - transform.rotation.y) * deltaTime * 7.0f;
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

    for (size_t i = 0; i < 10; i++) {
        Entity seryoha = world.spawn();
        world.addComponent<MeshComponent>(seryoha, mesh, seryohaTexture, glm::vec4(1.0f));
        world.addComponent<Transform3DComponent>(seryoha);
        world.setScript<MyScript>(seryoha, i == 3);
    }

    Entity room = world.spawn();
    world.addComponent<MeshComponent>(room, roomBack, glm::vec4(0.5f, 0.8f, 0.5f, 1.0f));
    world.addComponent<Transform3DComponent>(room, glm::vec3(), glm::vec3(0.0f, -90.0f, 0.0f), MyScript::ROOM_SIZE * 2.0f + 1.0f);

    FreeType freeType = FreeType();
    Font tnrFont = Font(freeType, ASSETS_DIR "/fonts/Times New Roman.ttf");
    Font dosFont = Font(freeType, ASSETS_DIR "/fonts/dos2000-ru-en.otf");
    Font sansFont = Font(freeType, ASSETS_DIR "/fonts/Blogger Sans.ttf");
    
    Entity textEntity = world.spawn();
    const wchar_t *sourceText = L"Hello, World! Добро пожаловать, епта!";
    world.addComponent<TextComponent>(textEntity, tnrFont, sourceText, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
    world.addComponent<Transform2DComponent>(textEntity, glm::vec2(), 0.0f, glm::vec2(0.1f));
    world.setScript<TextScript>(textEntity);

    Entity fpsTextEntity = world.spawn();
    world.addComponent<TextComponent>(fpsTextEntity, sansFont, L"FPS: 0", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    world.addComponent<Transform2DComponent>(fpsTextEntity, glm::vec2(), 0.0f, glm::vec2(0.2f));
    world.setScript<FpsCounterScript>(fpsTextEntity);

    Entity svoEntity = world.spawn();
    world.addComponent<SpriteComponent>(svoEntity, svoTexture, glm::vec4(1.0f, 1.0f, 1.0f, 0.4f));
    world.addComponent<Transform2DComponent>(svoEntity, glm::vec2(0.55f), 0.0f, glm::vec2(0.4f));
    world.setScript<SvoScript>(svoEntity);

    uint64_t lastTime = 0;
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