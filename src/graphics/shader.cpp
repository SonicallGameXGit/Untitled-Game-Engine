#include "shader.hpp"

#include <fstream>
#include <glm/gtc/type_ptr.hpp>

static GLuint boundProgram = 0;

GLuint createShader(const char *source, ShaderType type) {
    GLuint shaderId = glCreateShader(static_cast<GLenum>(type));
    glShaderSource(shaderId, 1, &source, nullptr);
    glCompileShader(shaderId);

    GLint success = 0;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE) {
        GLint length = 0;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length);

        std::string log(length, ' ');
        glGetShaderInfoLog(shaderId, length, nullptr, &log[0]);

        throwFatal("glCompileShader", "Failed to compile shader. Error:\n" + log + "\nSource:\n" + source + '\n');
        return 0;
    }

    return shaderId;
}

Shader::Shader(GLuint id) : id(id) {}
Shader::~Shader() {
    glDeleteShader(this->id);
}
Shader Shader::fromFile(const std::string &path, ShaderType type) {
    std::ifstream file = std::ifstream(path);
    if (!file.is_open()) {
        throwFatal("std::ifstream", "Failed to open file: " + path);
        return Shader(0);
    }

    std::string source = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    return Shader(createShader(source.c_str(), type));
}
Shader Shader::fromSourceCode(const char *source, ShaderType type) {
    return Shader(createShader(source, type));
}
GLuint Shader::getId() const {
    return this->id;
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
    glUniform1i(glGetUniformLocation(this->id, name), value);
}
void ShaderProgram::setInt(const char *name, int value) const {
    glUniform1i(glGetUniformLocation(this->id, name), value);
}
void ShaderProgram::setFloat(const char *name, float value) const {
    glUniform1f(glGetUniformLocation(this->id, name), value);
}
void ShaderProgram::setVec2(const char *name, float x, float y) const {
    glUniform2f(glGetUniformLocation(this->id, name), x, y);
}
void ShaderProgram::setVec2(const char *name, const glm::vec2 &vector) const {
    glUniform2f(glGetUniformLocation(this->id, name), vector.x, vector.y);
}
void ShaderProgram::setVec3(const char *name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(this->id, name), x, y, z);
}
void ShaderProgram::setVec3(const char *name, const glm::vec3 &vector) const {
    glUniform3f(glGetUniformLocation(this->id, name), vector.x, vector.y, vector.z);
}
void ShaderProgram::setVec4(const char *name, float x, float y, float z, float w) const {
    glUniform4f(glGetUniformLocation(this->id, name), x, y, z, w);
}
void ShaderProgram::setVec4(const char *name, const glm::vec4 &vector) const {
    glUniform4f(glGetUniformLocation(this->id, name), vector.x, vector.y, vector.z, vector.w);
}

void ShaderProgram::setMat2(const char *name, const glm::mat2 &matrix) const {
    glUniformMatrix2fv(glGetUniformLocation(this->id, name), 1, false, glm::value_ptr(matrix));
}
void ShaderProgram::setMat3(const char *name, const glm::mat3 &matrix) const {
    glUniformMatrix3fv(glGetUniformLocation(this->id, name), 1, false, glm::value_ptr(matrix));
}
void ShaderProgram::setMat4(const char *name, const glm::mat4 &matrix) const {
    glUniformMatrix4fv(glGetUniformLocation(this->id, name), 1, false, glm::value_ptr(matrix));
}