#pragma once
#include <GL/glew.h>
#include <array>
#include <string>
#include <glm/glm.hpp>

#include "../util/debug.hpp"

enum class ShaderType : GLenum {
    VERTEX = GL_VERTEX_SHADER,
    FRAGMENT = GL_FRAGMENT_SHADER,
    COMPUTE_SHADER = GL_COMPUTE_SHADER,
    GEOMETRY_SHADER = GL_GEOMETRY_SHADER,
};
class Shader {
private:
    GLuint id = 0;
    Shader(GLuint id);
public:
    ~Shader();

    static Shader fromFile(const std::string &path, ShaderType type);
    static Shader fromSourceCode(const char *source, ShaderType type);

    GLuint getId() const;
};
class ShaderProgram {
private:
    GLuint id = 0;
public:
    template<std::size_t N>
    ShaderProgram(const std::array<Shader, N> &shaders) {
        this->id = glCreateProgram();

        for (const Shader &shader : shaders) {
            glAttachShader(this->id, shader.getId());
        }
        glLinkProgram(this->id);

        GLint success = 0;
        glGetProgramiv(this->id, GL_LINK_STATUS, &success);

        if (success != GL_TRUE) {
            GLint length = 0;
            glGetProgramiv(this->id, GL_INFO_LOG_LENGTH, &length);

            std::string log(length, ' ');
            glGetProgramInfoLog(this->id, length, nullptr, &log[0]);

            throwFatal("glLinkProgram", "Failed to link shader program. Log:\n" + log);
            return;
        }
    }
    ~ShaderProgram();

    void bind() const;
    
    void setBool(const char *name, bool value) const;
    void setInt(const char *name, int value) const;
    void setFloat(const char *name, float value) const;

    void setVec2(const char *name, float x, float y) const;
    void setVec2(const char *name, const glm::vec2 &vector) const;
    void setVec3(const char *name, float x, float y, float z) const;
    void setVec3(const char *name, const glm::vec3 &vector) const;
    void setVec4(const char *name, float x, float y, float z, float w) const;
    void setVec4(const char *name, const glm::vec4 &vector) const;

    void setMat2(const char *name, const glm::mat2 &matrix) const;
    void setMat3(const char *name, const glm::mat3 &matrix) const;
    void setMat4(const char *name, const glm::mat4 &matrix) const;
};