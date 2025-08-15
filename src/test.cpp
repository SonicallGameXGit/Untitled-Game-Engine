#include "includes.hpp"

int main() {
    if (hasCriticalErrors()) return 1;
    Window window = Window({
        .title = "Example",
        .width = 1280,
        .height = 720,
    });

    glClearColor(0.3f, 0.6f, 1.0f, 1.0f);

    ECS ecs = ECS();
    Camera camera = Camera();
    camera.position.z = 2.0f;

    WorldRenderer worldRenderer = WorldRenderer();
    TextRenderer textRenderer = TextRenderer();
    SpriteRenderer spriteRenderer = SpriteRenderer();

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

    Entity seryoha = ecs.spawn();
    ecs.addComponent<MeshComponent>(seryoha, mesh, seryohaTexture, glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
    ecs.addComponent<Transform3DComponent>(seryoha);

    FreeType freeType = FreeType();
    Font tnrFont = Font(freeType, ASSETS_DIR "/fonts/Times New Roman.ttf");
    Font dosFont = Font(freeType, ASSETS_DIR "/fonts/dos2000-ru-en.otf");
    Font sansFont = Font(freeType, ASSETS_DIR "/fonts/Blogger Sans.ttf");
    
    Entity textEntity = ecs.spawn();
    const wchar_t *sourceText = L"Hello, World! Добро пожаловать, епта!";
    ecs.addComponent<TextComponent>(textEntity, tnrFont, sourceText, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
    ecs.addComponent<Transform2DComponent>(textEntity, glm::vec2(), 0.0f, glm::vec2(0.1f));

    Entity flyingTextEntity = ecs.spawn();
    ecs.addComponent<TextComponent>(flyingTextEntity, dosFont, L"Exe-Boi - пиши на C++ блять", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    ecs.addComponent<Transform3DComponent>(flyingTextEntity, glm::vec3(), glm::vec3(), glm::vec3(0.1f));

    Entity fpsTextEntity = ecs.spawn();
    ecs.addComponent<TextComponent>(fpsTextEntity, sansFont, L"FPS: 0", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    ecs.addComponent<Transform2DComponent>(fpsTextEntity, glm::vec2(), 0.0f, glm::vec2(0.2f));

    Entity svoEntity = ecs.spawn();
    ecs.addComponent<SpriteComponent>(svoEntity, svoTexture);
    ecs.addComponent<Transform2DComponent>(svoEntity, glm::vec2(0.55f), 0.0f, glm::vec2(0.4f));

    uint32_t fps = 0;
    uint64_t fpsTimer = 0;

    uint64_t lastTime = 0;
    while (window.isRunning()) {
        uint64_t currentTime = SDL_GetTicksNS();
        uint64_t deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        fpsTimer += deltaTime;
        fps++;
        if (fpsTimer >= 1e9) { // 1 second
            TextComponent &fpsText = ecs.getComponent<TextComponent>(fpsTextEntity);
            fpsText.setText(L"FPS: " + std::to_wstring(fps));

            if (fps < 15) {
                fpsText.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
            } else if (fps < 60) {
                fpsText.color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
            } else if (fps < 144) {
                fpsText.color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
            } else {
                fpsText.color = glm::vec4(1.0f);
            }

            fps = 0;
            fpsTimer -= 1e9;
        }

        window.pollEvents();
        camera.update(window);

        Transform3DComponent &transform = ecs.getComponent<Transform3DComponent>(seryoha);
        transform.rotation.x = SDL_GetTicks() / 1000.0f * 45.0f;
        transform.rotation.y = SDL_GetTicks() / 1000.0f * 60.0f;

        constexpr float speed = 0.2f;
        Transform3DComponent &flyingTextTransform = ecs.getComponent<Transform3DComponent>(flyingTextEntity);
        flyingTextTransform.position.x = std::sin(SDL_GetTicks() / 1000.0f * SDL_PI_F * speed) * 1.1f;
        flyingTextTransform.position.z = std::cos(SDL_GetTicks() / 1000.0f * SDL_PI_F * speed) * 1.1f;
        flyingTextTransform.rotation.y = SDL_GetTicks() / 1000.0f * 180.0f * speed + 45.0f;

        const TextComponent &textComponent = ecs.getComponent<TextComponent>(textEntity);
        Transform2DComponent &textTransform = ecs.getComponent<Transform2DComponent>(textEntity);
        textTransform.position.x = -textComponent.getFont()->getTextWidth(textComponent.getText()) * 0.5f * textTransform.scale.x;
        textTransform.position.y = -0.95f + textTransform.scale.y;

        Transform2DComponent &fpsTextTransform = ecs.getComponent<Transform2DComponent>(fpsTextEntity);
        fpsTextTransform.position.x = 0.05f - window.getHorizontalAspect();
        fpsTextTransform.position.y = 0.95f;

        Transform2DComponent &svoTransform = ecs.getComponent<Transform2DComponent>(svoEntity);
        svoTransform.position.x = window.getHorizontalAspect() - 0.45f;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // TODO: Right now all renderers are work one after another, but if text will be logically in front of the sprite, it would be any ways drawn behind, just because it's impossible to draw above like that...
        // Maybe join them at once somehow, or idk, I don't want to make a huge and fat piece of shit called "Reeeeeeendeeereeeeer", that's stupid!
        worldRenderer.render(ecs, camera);
        textRenderer.render(ecs, window, camera);
        spriteRenderer.render(ecs, window, camera);
        window.swapBuffers();
    }

    return 0;
}