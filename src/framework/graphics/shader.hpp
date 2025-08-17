#pragma once
#include <vector>
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <framework/util/debug.hpp>

enum class ShaderType : uint32_t {
    Vertex = GL_VERTEX_SHADER,
    Fragment = GL_FRAGMENT_SHADER,
    Compute = GL_COMPUTE_SHADER,
    Geometry = GL_GEOMETRY_SHADER,
};
class Shader {
private:
    uint32_t id = 0;
    Shader(uint32_t id);
public:
    ~Shader();

    static Shader fromFile(const std::string &path, ShaderType type);
    static Shader fromSourceCode(const char *source, ShaderType type);

    uint32_t getId() const;
};
class ShaderProgram {
private:
    uint32_t id = 0;
public:
    ShaderProgram(const std::vector<Shader> &shaders);
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