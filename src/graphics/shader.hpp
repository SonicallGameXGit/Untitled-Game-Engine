#pragma once
#include <GL/glew.h>
#include <vector>
#include <string>
#include <glm/glm.hpp>

enum class ShaderType : GLenum {
    VERTEX = GL_VERTEX_SHADER,
    FRAGMENT = GL_FRAGMENT_SHADER,
    COMPUTE_SHADER = GL_COMPUTE_SHADER,
    GEOMETRY_SHADER = GL_GEOMETRY_SHADER,
};
class Shader {
private:
    GLuint id = 0;
public:
    Shader(const std::string &source, bool isPath, ShaderType type);
    ~Shader();

    GLuint getId() const;
};
class ShaderProgram {
private:
    GLuint id = 0;
    std::vector<GLuint> shaders = {};
public:
    ShaderProgram();
    ~ShaderProgram();

    void attach(const Shader &shader);
    void compile();
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