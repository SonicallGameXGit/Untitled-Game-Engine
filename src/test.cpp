#include "includes.hpp"
#include <SDL3_image/SDL_image.h>

int main(int argc, char *argv[]) {
    Window window = Window("Example", 1920, 1080, true);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.3f, 0.6f, 1.0f, 1.0f);
 
    ShaderProgram shaderProgram = ShaderProgram();
    {
        Shader vertexShader = Shader::fromFile("./assets/shaders/hue.vert", ShaderType::VERTEX);
        Shader fragmentShader = Shader::fromFile("./assets/shaders/hue.frag", ShaderType::FRAGMENT);

        shaderProgram.attach(vertexShader);
        shaderProgram.attach(fragmentShader);
        shaderProgram.compile();
    }
    shaderProgram.bind();
    shaderProgram.setInt("u_ColorSampler", 0);

    Mesh quad = Mesh(std::vector<float>{
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 1.0f,
    }, { MeshAttribute::Vec2, MeshAttribute::Vec2 }, MeshTopology::TRIANGLE_FAN);

    Texture texture = Texture::fromFile("./assets/img/seryoha.png", TextureFilter::LINEAR, TextureWrap::REPEAT);

    while (window.isRunning()) {
        window.pollEvents();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderProgram.bind();
        shaderProgram.setFloat("u_Time", static_cast<float>(static_cast<double>(SDL_GetTicks()) / 1000.0) * 5.0f);

        texture.bind(0);
        quad.render();

        window.swapBuffers();
    }

    return 0;
}
