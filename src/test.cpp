#include "includes.hpp"

int main(int argc, char *argv[]) {
    Window window = Window("Example", 1920, 1080, true);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.3f, 0.6f, 1.0f, 1.0f);

    ShaderProgram shaderProgram = ShaderProgram();
    {
        Shader vertexShader = Shader::fromSourceCode(R"(
            #version 410

            layout(location=0) in vec2 a_Position;
            layout(location=1) in vec3 a_Color;

            layout(location=0) out vec3 v_Color;

            void main() {
                gl_Position = vec4(a_Position, 0.0, 1.0);
                v_Color = a_Color;
            }
        )", ShaderType::VERTEX);
        Shader fragmentShader = Shader::fromSourceCode(R"(
            #version 410

            layout(location=0) in vec3 v_Color;
            layout(location=0) out vec4 f_Color;

            uniform float u_Time;

            // Source: https://gist.github.com/fschr/a8476f8993e5b9a60aa1c7ec4af3704b
            vec3 hueShift(vec3 color, float hueAdjust) {
                const vec3 kRGBToYPrime = vec3(0.299, 0.587, 0.114);
                const vec3 kRGBToI = vec3(0.596, -0.275, -0.321);
                const vec3 kRGBToQ = vec3(0.212, -0.523, 0.311);

                const vec3 kYIQToR = vec3(1.0, 0.956, 0.621);
                const vec3 kYIQToG = vec3(1.0, -0.272, -0.647);
                const vec3 kYIQToB = vec3(1.0, -1.107, 1.704);

                float YPrime = dot(color, kRGBToYPrime);
                float I = dot(color, kRGBToI);
                float Q = dot(color, kRGBToQ);
                float hue = atan(Q, I);
                float chroma = sqrt(I * I + Q * Q);

                hue += hueAdjust;

                Q = chroma * sin (hue);
                I = chroma * cos (hue);

                vec3 yIQ = vec3 (YPrime, I, Q);

                return vec3(dot(yIQ, kYIQToR), dot(yIQ, kYIQToG), dot(yIQ, kYIQToB));
            }

            void main() {
                f_Color = vec4(hueShift(v_Color, u_Time), 1.0);
            }
        )", ShaderType::FRAGMENT);

        shaderProgram.attach(vertexShader);
        shaderProgram.attach(fragmentShader);
        shaderProgram.compile();
    }

    Mesh quad = Mesh(std::vector<float>{
        -1.0f, -1.0f, 0xFE / 255.0f, 0xDA / 255.0f, 0x77 / 255.0f,
         1.0f, -1.0f, 0xF5 / 255.0f, 0x85 / 255.0f, 0x29 / 255.0f,
         1.0f,  1.0f, 0x81 / 255.0f, 0x34 / 255.0f, 0xAF / 255.0f,
        -1.0f,  1.0f, 0x51 / 255.0f, 0x5B / 255.0f, 0xD4 / 255.0f,
    }, { MeshAttribute::Vec2, MeshAttribute::Vec3 }, MeshTopology::TRIANGLE_FAN);

    while (window.isRunning()) {
        window.pollEvents();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderProgram.bind();
        shaderProgram.setFloat("u_Time", static_cast<float>(static_cast<double>(SDL_GetTicks()) / 1000.0) * 5.0f);
        quad.render();

        window.swapBuffers();
    }

    return 0;
}
