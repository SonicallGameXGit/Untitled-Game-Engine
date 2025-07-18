#include "includes.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <entt/entt.hpp>
#include <optional>

struct TransformComponent {
    glm::vec3 position = glm::vec3();
    glm::vec3 rotation = glm::vec3();
    glm::vec3 scale = glm::vec3(1.0f);

    TransformComponent() {}
    TransformComponent(const glm::vec3 &position) : position(position) {}
    TransformComponent(const glm::vec3 &position, const glm::vec3 &rotation, const glm::vec3 &scale) : position(position), rotation(rotation), scale(scale) {}
    ~TransformComponent() {}

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

struct MeshComponent {
    Mesh *mesh = nullptr;
    TextureView texture = TextureView();
    glm::vec4 color = glm::vec4(1.0f);

    MeshComponent() {}
    MeshComponent(Mesh *mesh) : mesh(mesh) {}
    MeshComponent(Mesh *mesh, TextureView texture) : mesh(mesh), texture(texture) {}
    MeshComponent(Mesh *mesh, TextureView texture, const glm::vec4 &color) : mesh(mesh), texture(texture), color(color) {}
    MeshComponent(Mesh *mesh, const glm::vec4 &color) : mesh(mesh), color(color) {}
    ~MeshComponent() {}
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
public:
    Renderer() {
        this->worldShader.bind();
        this->worldShader.setInt("u_ColorSampler", 0);
    }

    const ShaderProgram &getWorldShader() const {
        return this->worldShader;
    }
};
class Scene {
private:
    entt::registry registry = entt::registry();
public:
    Scene() {
        this->registry.group<TransformComponent, MeshComponent>();
    }
    ~Scene() {}

    void render(const Renderer &renderer, const Camera &camera) const {
        const ShaderProgram &worldShader = renderer.getWorldShader();
        worldShader.bind();
        
        for (auto [entity, mesh, transform] : this->registry.view<MeshComponent, TransformComponent>().each()) {
            worldShader.setMat4("u_ProjectionViewModelMatrix", camera.getProjectionViewMatrix() * transform.getModelMatrix());
            worldShader.setVec4("u_Color", mesh.color);
            
            mesh.texture.bind(0);
            mesh.mesh->render();
        }
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
    Window window = Window("Example", 1920, 1080, true);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.3f, 0.6f, 1.0f, 1.0f);

    Renderer renderer = Renderer();
    Camera camera = Camera();
    camera.position.z = 2.0f;
    Scene scene = Scene();

    Mesh cube = Mesh(
        {
            // front and back
             0,  3,  2,
             2,  1,  0,
             4,  5,  6,
             6,  7,  4,
            // left and right
            11,  8,  9,
             9, 10, 11,
            12, 13, 14,
            14, 15, 12,
            // bottom and top
            16, 17, 18,
            18, 19, 16,
            20, 21, 22,
            22, 23, 20
        },
        std::vector<float>{
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  // A 0
             0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  // B 1
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  // C 2
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  // D 3
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  // E 4
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  // F 5
             0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  // G 6
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  // H 7
    
            -0.5f,  0.5f, -0.5f,  0.0f, 0.0f,  // D 8
            -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  // A 9
            -0.5f, -0.5f,  0.5f,  1.0f, 1.0f,  // E 10
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  // H 11
             0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  // B 12
             0.5f,  0.5f, -0.5f,  1.0f, 0.0f,  // C 13
             0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  // G 14
             0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  // F 15
    
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  // A 16
             0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  // B 17
             0.5f, -0.5f,  0.5f,  1.0f, 1.0f,  // F 18
            -0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  // E 19
             0.5f,  0.5f, -0.5f,  0.0f, 0.0f,  // C 20
            -0.5f,  0.5f, -0.5f,  1.0f, 0.0f,  // D 21
            -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  // H 22
             0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  // G 23
        },
        std::array<MeshAttribute, 2>{ MeshAttribute::Vec3, MeshAttribute::Vec2 },
        MeshTopology::TRIANGLE_LIST
    );
    Mesh triangle = Mesh(std::vector<float>{
        -0.5f, -0.5f, 0.0f, 0.0f,
         0.5f, -0.5f, 1.0f, 0.0f,
         0.5f,  0.5f, 0.5f, 1.0f,
    }, std::array<MeshAttribute, 2>{ MeshAttribute::Vec2, MeshAttribute::Vec2 }, MeshTopology::TRIANGLE_LIST);

    Texture seryohaTexture = Texture::fromFile("./assets/img/seryoha.png", TextureFilter::LINEAR, TextureWrap::REPEAT);

    entt::entity seryoha = scene.spawn();
    scene.addComponent<TransformComponent>(seryoha);
    scene.addComponent<MeshComponent>(seryoha, &cube, TextureView(seryohaTexture), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    entt::entity seryoha2 = scene.spawn();
    scene.addComponent<TransformComponent>(seryoha2);
    scene.addComponent<MeshComponent>(seryoha2, &triangle, TextureView(seryohaTexture), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    while (window.isRunning()) {
        if (scene.hasComponents<TransformComponent>(seryoha)) {
            TransformComponent &transform = scene.getComponent<TransformComponent>(seryoha);
            transform.position.x = glm::sin(static_cast<float>(static_cast<double>(SDL_GetTicks()) / 1000.0));
        }

        window.pollEvents();
        camera.update(window);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        scene.render(renderer, camera);

        window.swapBuffers();
    }

    return 0;
}