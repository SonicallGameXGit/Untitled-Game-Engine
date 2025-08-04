#include "includes.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <entt/entt.hpp>
#include <optional>

struct Transform3DComponent {
    glm::vec3 position = glm::vec3();
    glm::vec3 rotation = glm::vec3();
    glm::vec3 scale = glm::vec3(1.0f);

    Transform3DComponent() {}
    Transform3DComponent(const glm::vec3 &position) : position(position) {}
    Transform3DComponent(const glm::vec3 &position, const glm::vec3 &rotation, const glm::vec3 &scale) : position(position), rotation(rotation), scale(scale) {}
    ~Transform3DComponent() {}

    glm::mat4 getModelMatrix() const {
        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, this->position);
        matrix = glm::rotate(matrix, glm::radians(this->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        matrix = glm::rotate(matrix, glm::radians(this->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        matrix = glm::rotate(matrix, glm::radians(this->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        matrix = glm::scale(matrix, this->scale);

        return matrix;
    }
};
struct Transform2DComponent {
    glm::vec2 position = glm::vec2();
    glm::vec2 rotation = glm::vec2();
    glm::vec2 scale = glm::vec2(1.0f);

    Transform2DComponent() {}
    Transform2DComponent(const glm::vec2 &position) : position(position) {}
    Transform2DComponent(const glm::vec2 &position, const glm::vec2 &rotation, const glm::vec2 &scale) : position(position), rotation(rotation), scale(scale) {}
    ~Transform2DComponent() {}

    glm::mat4 getModelMatrix() const {
        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, glm::vec3(this->position.x, this->position.y, 0.0f));
        matrix = glm::rotate(matrix, glm::radians(this->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        matrix = glm::rotate(matrix, glm::radians(this->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        matrix = glm::scale(matrix, glm::vec3(this->scale.x, this->scale.y, 1.0f));

        return matrix;
    }
};

struct MeshComponent {
    VertexArray *vertexArray = nullptr;
    TextureView texture = TextureView();
    glm::vec4 color = glm::vec4(1.0f);

    MeshComponent() {}
    MeshComponent(VertexArray *vertexArray) : vertexArray(vertexArray) {}
    MeshComponent(VertexArray *vertexArray, TextureView texture) : vertexArray(vertexArray), texture(texture) {}
    MeshComponent(VertexArray *vertexArray, TextureView texture, const glm::vec4 &color) : vertexArray(vertexArray), texture(texture), color(color) {}
    MeshComponent(VertexArray *vertexArray, const glm::vec4 &color) : vertexArray(vertexArray), color(color) {}
    ~MeshComponent() {}
};

class TextComponent {
private:
    TextMesh mesh = TextMesh();
public:
    Font *font = nullptr;
    glm::vec4 color = glm::vec4(1.0f);

    TextComponent() {}
    TextComponent(Font *font) : font(font) {}
    TextComponent(Font *font, const glm::vec4 &color) : font(font), color(color) {}
    TextComponent(Font *font, const std::wstring &text) : font(font) {
        if (this->font == nullptr) return;
        this->mesh.setText(text, *this->font);
    }
    TextComponent(Font *font, const std::wstring &text, const glm::vec4 &color) : font(font), color(color) {
        if (this->font == nullptr) return;
        this->mesh.setText(text, *this->font);
    }
    ~TextComponent() {}

    void setText(const std::wstring &text) {
        if (this->font == nullptr) return;
        this->mesh.setText(text, *this->font);
    }
    void draw() const {
        this->mesh.draw();
    }
};

class Camera {
private:
    glm::mat4 projectionMatrix = glm::mat4(), viewMatrix = glm::mat4(), projectionViewMatrix = glm::mat4();
    glm::vec3 front = glm::vec3(0.0f, 0.0f, 1.0f), up = glm::vec3(0.0f, 1.0f, 0.0f), right = glm::vec3(1.0f, 0.0f, 0.0f);
public:
    glm::vec3 position = glm::vec3(), rotation = glm::vec3();
    float fov = 90.0f, zNear = 0.01f, zFar = 1000.0f;

    Camera() {}
    ~Camera() {}

    void update(const Window &window) {
        glm::mat4 rotator = glm::mat4(1.0f);
        rotator = glm::rotate(rotator, glm::radians(this->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        rotator = glm::rotate(rotator, glm::radians(this->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotator = glm::rotate(rotator, glm::radians(this->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));

        this->front = glm::vec3(rotator * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f));
        this->up = glm::vec3(rotator * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
        this->right = glm::vec3(rotator * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

        this->projectionMatrix = glm::perspective(glm::radians(this->fov), static_cast<float>(window.getWidth()) / static_cast<float>(window.getHeight()), this->zNear, this->zFar);
        this->viewMatrix = glm::lookAt(this->position, this->position + this->front, this->up);
    	this->projectionViewMatrix = this->projectionMatrix * this->viewMatrix;
    }

    const glm::mat4 &getProjectionMatrix() const {
        return this->projectionMatrix;
    }
    const glm::mat4 &getViewMatrix() const {
        return this->viewMatrix;
    }
    const glm::mat4 &getProjectionViewMatrix() const {
        return this->projectionViewMatrix;
    }
};

class Renderer {
private:
    ShaderProgram worldShader = ShaderProgram(std::array<Shader, 2>{
        Shader::fromFile("./assets/shaders/world.vert", ShaderType::VERTEX),
        Shader::fromFile("./assets/shaders/world.frag", ShaderType::FRAGMENT),
    });
    ShaderProgram textShader = ShaderProgram(std::array<Shader, 2>{
        Shader::fromFile("./assets/shaders/text.vert", ShaderType::VERTEX),
        Shader::fromFile("./assets/shaders/text.frag", ShaderType::FRAGMENT),
    });
public:
    Renderer() {
        this->worldShader.bind();
        this->worldShader.setInt("u_ColorSampler", 0);
        
        this->textShader.bind();
        this->textShader.setInt("u_ColorSampler", 0);
        this->textShader.setFloat("u_PixelRange", Font::PIXEL_RANGE);
    }
    ~Renderer() {}

    const ShaderProgram &getWorldShader() const {
        return this->worldShader;
    }
    const ShaderProgram &getTextShader() const {
        return this->textShader;
    }
};
class Scene {
private:
    entt::registry registry = entt::registry();

    void renderWorld(const Renderer &renderer, const Camera &camera) const {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        const ShaderProgram &worldShader = renderer.getWorldShader();
        worldShader.bind();

        std::vector<std::tuple<MeshComponent, Transform3DComponent>> transparentObjects = {};
        {
            auto entities = this->registry.view<MeshComponent, Transform3DComponent>().each();
            for (auto [_, mesh, transform] : entities) {
                if (mesh.color.a < 1.0f) {
                    transparentObjects.emplace_back(mesh, transform);
                    continue;
                }
                
                worldShader.setMat4("u_ProjectionViewModelMatrix", camera.getProjectionViewMatrix() * transform.getModelMatrix());
                worldShader.setVec4("u_Color", mesh.color);
                
                mesh.texture.bind(0);
                mesh.vertexArray->drawArrays(Topology::TRIANGLE_LIST);
            }
        }

        glDisable(GL_CULL_FACE);
        // TODO: Use depth sorting for transparent objects
        for (auto &[mesh, transform] : transparentObjects) {
            worldShader.setMat4("u_ProjectionViewModelMatrix", camera.getProjectionViewMatrix() * transform.getModelMatrix());
            worldShader.setVec4("u_Color", mesh.color);
            
            mesh.texture.bind(0);
            mesh.vertexArray->drawArrays(Topology::TRIANGLE_LIST);
        }
    }
    void renderText(const Window &window, const Renderer &renderer, const Camera &camera) const {
        const ShaderProgram &textShader = renderer.getTextShader();
        textShader.bind();

        {
            glEnable(GL_DEPTH_TEST);
            auto entities = this->registry.view<TextComponent, Transform3DComponent>().each();
            for (auto [_, text, transform] : entities) {
                if (text.font == nullptr) continue;

                textShader.setMat4("u_ProjectionViewModelMatrix", camera.getProjectionViewMatrix() * transform.getModelMatrix());
                textShader.setVec4("u_Color", text.color);

                text.font->getTexture().bind(0);
                text.draw();
            }
        }
        {
            float aspect = static_cast<float>(window.getWidth()) / static_cast<float>(window.getHeight());
            glm::mat4 projectionMatrix = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);

            glDisable(GL_DEPTH_TEST);
            auto entities = this->registry.view<TextComponent, Transform2DComponent>().each();
            for (auto [_, text, transform] : entities) {
                if (text.font == nullptr) continue;

                textShader.setMat4("u_ProjectionViewModelMatrix", projectionMatrix * transform.getModelMatrix());
                textShader.setVec4("u_Color", text.color);

                text.font->getTexture().bind(0);
                text.draw();
            }
        }
    }
public:
    Scene() {}
    ~Scene() {}

    void render(const Window &window, const Renderer &renderer, const Camera &camera) const {
        this->renderWorld(renderer, camera);
        this->renderText(window, renderer, camera);
    }

    entt::entity spawn() {
        return this->registry.create();
    }
    void destroy(entt::entity entity) {
        this->registry.destroy(entity);
    }

    template<typename T, typename ...Args>
    void addComponent(entt::entity entity, Args &&...args) {
        this->registry.emplace<T>(entity, std::forward<Args>(args)...);
    }
    template<typename T, typename ...Other>
    void removeComponents(entt::entity entity) {
        this->registry.erase<T, Other...>(entity);
    }

    template<typename T, typename ...Other>
    bool hasComponents(entt::entity entity) {
        return this->registry.any_of<T, Other...>(entity);
    }
    template<typename ...T>
    auto &getComponent(entt::entity entity) {
        return this->registry.get<T...>(entity);
    }
};

int main() {
    if (hasCriticalErrors()) return 1;
    Window window = Window("Example", 1920, 1080, true, false);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);

    Renderer renderer = Renderer();
    Camera camera = Camera();
    Scene scene = Scene();

    FreeType freetype = FreeType();
    Font font = Font(freetype, "./assets/fonts/dos2000-ru-en.otf", 8.0f);

    std::wstring sourceText = LR"(У тебя мать... такая же плоская, и она существует в трехмерном измерении!
Ну вот как она по твоему существует? (неет, нееееет!)
Какого-то хрена у тебя объекты плоские в три дэ блять что-то делают... Хули они там что-то делают ваще?
Они не плоские, они трехмерные!

Ты понимаешь куда я попал? (нет)
Да нет, у меня текст есть, надо прочитать что за текст здесь!
Я вот даже с Эф-Два и вот, даже спрайт могу переместить и вот.
Я сказал - ну это ты, я сказал - переместить!

Нет, ты дебил, потому что... Ты дебил! (да)
Ты дебил; нет ты дебил! Ты дебил; нет ты дебил!
Нет, ты дебил, потому что... Ты дебил! (хз)
Ты дебил; нет ты дебил! Ты дебил; нет ты дебил!

У этого животного такая черная аватарка...
Ты дебил; нет ты дебил! Ты дебил; нет ты дебил!
У этого животного такая черная аватарка...
Я вот даже с Эф-Два и вот, даже спрайт могу переместить и вот.

Ты понимаешь куда я попал? (нет)
Да нет, у меня текст есть, надо прочитать что за текст здесь!
Я вот даже с Эф-Два и вот, даже спрайт могу переместить и вот.
Я сказал - ну это ты, я сказал - переместить!

Я сказал - ну вот давай, я сказал - ну вот я устал...
Я сказал - ну вот серега, я сказал - ну вот... НЕЕЕЕЕТ!!

Вот, взял, и начал вращать на Вэ! Где игры? Где игры?
Видали? Вот, можно! Это называется - можно!
Вот, взял, и начал вращать на Вэ! Где игры? Где игры?
Видали? Вот, можно! Это называется - игры!

Я даже спрайт могу переместить! Я вот даже нажав на эф-пять!
Я даже спрайт могу переместить! Я вот даже c эф-два и вот!

Ты, куда... попал? Куда... попал?
Ты, куда... попал? Это... это называется можно!

Ты понимаешь куда я попал? (нет)
Да нет, у меня текст есть, надо прочитать что за текст здесь!
Я вот даже с Эф-Два и вот, даже спрайт могу переместить и вот.
Я сказал - ну это ты, я сказал - переместить!

Нет, ты дебил, потому что... Ты дебил! (да)
Ты дебил; нет ты дебил! Ты дебил; нет ты дебил!
Нет, дебил! Ты дебил! Ты дебил... потому что...
Нет, дебил! Ты дебил! Ты дебил... Хз...

Ты понимаешь куда я попал? (нет)
Да нет, у меня текст есть, надо прочитать что за текст здесь!
Я вот даже с Эф-Два и вот, даже спрайт могу переместить и вот.
Я сказал - ну это ты, я сказал - переместить!

У тебя мать... такая же плоская, и она существует в трехмерном измерении!
Ну вот как она по твоему существует? (неет, нееееет!)
Какого-то хрена у тебя объекты плоские в три дэ блять что-то делают... Хули они там что-то делают ваще?
Они не плоские, они трехмерные!

Так ты их называешь плоскими! Как твоя мать!
Я... я вот щас могу запустить эту игру, нажав на эф-пять...)";

    entt::entity textExample = scene.spawn();
    scene.addComponent<Transform2DComponent>(textExample, glm::vec2(-1.777f, 0.98f), glm::vec2(), glm::vec2(0.06f));
    scene.addComponent<TextComponent>(textExample, &font, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

    constexpr float encoderTicksPerSecond = 60.0f;
    constexpr size_t encoderTickSubsteps = 14;
    float encoderTimer = 0.0f;
    float textY = 0.0f;
    // Charset containing English (upper/lower), Russian (upper/lower), digits, and common symbols
    std::wstring table = 
        L"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        L"abcdefghijklmnopqrstuvwxyz"
        L"АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ"
        L"абвгдеёжзийклмнопрстуфхцчшщъыьэюя"
        L"0123456789"
        L" .,!?;:-_()[]{}<>@#$%^&*+=/\\|\"'`~";
    std::wstring encodedSource = std::wstring(1, table[0]);
    size_t tablePointer = 0;
    
    float lastTime = 0.0f;
    while (window.isRunning()) {
        float currentTime = static_cast<float>(static_cast<double>(SDL_GetTicks()) / 1000.0);
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        encoderTimer += deltaTime;
        size_t numTicks = static_cast<size_t>(encoderTimer * encoderTicksPerSecond);
        Transform2DComponent &transform = scene.getComponent<Transform2DComponent>(textExample);
        for (size_t i = 0; i < numTicks * encoderTickSubsteps; i++) {
            if (encodedSource.size() >= sourceText.size() && tablePointer >= table.size()) {
                break;
            }

            if (sourceText[encodedSource.size() - 1] == L'\n') {
                encodedSource[encodedSource.size() - 1] = L'\n';
                encodedSource += table[0];
                textY += transform.scale.y;
                continue;
            }

            encodedSource[encodedSource.size() - 1] = table[++tablePointer];
            if (encodedSource[encodedSource.size() - 1] == sourceText[encodedSource.size() - 1]) {
                encodedSource += table[0];
                tablePointer = 0;
            }
        }

        encoderTimer -= static_cast<float>(numTicks) / encoderTicksPerSecond;
        TextComponent &textComponent = scene.getComponent<TextComponent>(textExample);
        textComponent.setText(encodedSource);

        transform.position.x = -static_cast<float>(window.getWidth()) / static_cast<float>(window.getHeight()) + 0.02f;
        transform.position.y += (0.98f + textY - transform.position.y) * deltaTime;

        window.pollEvents();
        camera.update(window);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        scene.render(window, renderer, camera);
        window.swapBuffers();
    }

    return 0;
}