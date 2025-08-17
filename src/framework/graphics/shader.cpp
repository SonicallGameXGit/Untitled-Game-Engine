#include "shader.hpp"
#include <fstream>
#include <glm/gtc/type_ptr.hpp>

static uint32_t boundProgram = 0;

static uint32_t createShader(const char *source, ShaderType type) {
    uint32_t shaderId = glCreateShader(static_cast<uint32_t>(type));
    glShaderSource(shaderId, 1, &source, nullptr);
    glCompileShader(shaderId);

    int32_t success = 0;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE) {
        int32_t length = 0;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length);

        std::string log(length, ' ');
        glGetShaderInfoLog(shaderId, length, nullptr, &log[0]);

        Debug::throwFatal("glCompileShader", "Failed to compile shader. Error:\n" + log + "\nSource:\n" + source + '\n');
        return 0;
    }

    return shaderId;
}

Shader::Shader(uint32_t id) : id(id) {}
Shader::~Shader() {
    glDeleteShader(this->id);
}
Shader Shader::fromFile(const std::string &path, ShaderType type) {
    std::ifstream file = std::ifstream(path);
    if (!file.is_open()) {
        Debug::throwFatal("std::ifstream", "Failed to open file: " + path);
        return Shader(0);
    }

    std::string source = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    return Shader(createShader(source.c_str(), type));
}
Shader Shader::fromSourceCode(const char *source, ShaderType type) {
    return Shader(createShader(source, type));
}
uint32_t Shader::getId() const {
    return this->id;
}

ShaderProgram::ShaderProgram(const std::vector<Shader> &shaders) {
    this->id = glCreateProgram();

    for (const Shader &shader : shaders) {
        glAttachShader(this->id, shader.getId());
    }
    glLinkProgram(this->id);

    int32_t success = 0;
    glGetProgramiv(this->id, GL_LINK_STATUS, &success);

    if (success != GL_TRUE) {
        int32_t length = 0;
        glGetProgramiv(this->id, GL_INFO_LOG_LENGTH, &length);

        std::string log(length, ' ');
        glGetProgramInfoLog(this->id, length, nullptr, &log[0]);

        Debug::throwFatal("glLinkProgram", "Failed to link shader program. Log:\n" + log);
        return;
    }
}
ShaderProgram::~ShaderProgram() {
    glDeleteProgram(this->id);
}
void ShaderProgram::bind() const {
    if (boundProgram == this->id) {
        return;
    }

    glUseProgram(this->id);
    boundProgram = this->id;
}

void ShaderProgram::setBool(const char *name, bool value) const {
    this->bind();
    glUniform1i(glGetUniformLocation(this->id, name), value);
}
void ShaderProgram::setInt(const char *name, int value) const {
    this->bind();
    glUniform1i(glGetUniformLocation(this->id, name), value);
}
void ShaderProgram::setFloat(const char *name, float value) const {
    this->bind();
    glUniform1f(glGetUniformLocation(this->id, name), value);
}
void ShaderProgram::setVec2(const char *name, float x, float y) const {
    this->bind();
    glUniform2f(glGetUniformLocation(this->id, name), x, y);
}
void ShaderProgram::setVec2(const char *name, const glm::vec2 &vector) const {
    this->bind();
    glUniform2f(glGetUniformLocation(this->id, name), vector.x, vector.y);
}
void ShaderProgram::setVec3(const char *name, float x, float y, float z) const {
    this->bind();
    glUniform3f(glGetUniformLocation(this->id, name), x, y, z);
}
void ShaderProgram::setVec3(const char *name, const glm::vec3 &vector) const {
    this->bind();
    glUniform3f(glGetUniformLocation(this->id, name), vector.x, vector.y, vector.z);
}
void ShaderProgram::setVec4(const char *name, float x, float y, float z, float w) const {
    this->bind();
    glUniform4f(glGetUniformLocation(this->id, name), x, y, z, w);
}
void ShaderProgram::setVec4(const char *name, const glm::vec4 &vector) const {
    this->bind();
    glUniform4f(glGetUniformLocation(this->id, name), vector.x, vector.y, vector.z, vector.w);
}

void ShaderProgram::setMat2(const char *name, const glm::mat2 &matrix) const {
    this->bind();
    glUniformMatrix2fv(glGetUniformLocation(this->id, name), 1, false, glm::value_ptr(matrix));
}
void ShaderProgram::setMat3(const char *name, const glm::mat3 &matrix) const {
    this->bind();
    glUniformMatrix3fv(glGetUniformLocation(this->id, name), 1, false, glm::value_ptr(matrix));
}
void ShaderProgram::setMat4(const char *name, const glm::mat4 &matrix) const {
    this->bind();
    glUniformMatrix4fv(glGetUniformLocation(this->id, name), 1, false, glm::value_ptr(matrix));
}