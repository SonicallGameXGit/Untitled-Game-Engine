#pragma once
#include <GL/glew.h>
#include <vector>
#include <cstdio>

// TODO: Add more advanced attribute types like Int, IVec2, IVec3, IVec4; Double, DVec2, DVec3, DVec4 and Long, LVec2, LVec3, LVec4 (if exists)
enum class MeshAttribute : size_t {
    Float = 1,
    Vec2 = 2,
    Vec3 = 3,
    Vec4 = 4
};
enum class MeshTopology : GLenum {
    TRIANGLE_LIST = GL_TRIANGLES,
    TRIANGLE_FAN = GL_TRIANGLE_FAN,
    TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
    LINE_LIST = GL_LINES,
    LINE_STRIP = GL_LINE_STRIP,
    LINE_LOOP = GL_LINE_LOOP
};

class Mesh {
private:
    GLuint vao = 0, ebo = 0, vbo = 0;
    GLenum topology = static_cast<GLenum>(MeshTopology::TRIANGLE_LIST);
    GLsizei numVertices = 0;

    template<size_t N>
    size_t buildAttributesAndGetOffset(const std::array<MeshAttribute, N> &attributes) {
        GLsizei stride = 0;
        for (size_t i = 0; i < attributes.size(); i++) {
            stride += static_cast<GLint>(attributes[i]) * sizeof(float); // TODO: After adding new attribute types, add switch to consider size in bytes of each attribute type.
        }
        
        size_t offset = 0;
        for (size_t i = 0; i < attributes.size(); i++) {
            size_t numAttributeFloats = static_cast<size_t>(attributes[i]);
            glEnableVertexAttribArray(i);
            glVertexAttribPointer(i, static_cast<GLint>(numAttributeFloats), GL_FLOAT, GL_FALSE, stride, (void*)offset);
            
            offset += numAttributeFloats * sizeof(float); // TODO: After adding new attribute types, add switch to consider size in bytes of each attribute type.
        }

        return offset;
    }
public:
    template<typename VT, size_t NAttributes>
    Mesh(const std::vector<GLuint> &elements, const std::vector<VT> &vertices, const std::array<MeshAttribute, NAttributes> &attributes, MeshTopology topology) :
        topology(static_cast<GLenum>(topology)),
        numVertices(static_cast<GLsizei>(elements.size()))
    {
        glGenVertexArrays(1, &this->vao);
        glGenBuffers(1, &this->vbo);
        glGenBuffers(1, &this->ebo);

        glBindVertexArray(this->vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
        
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(GLuint), elements.data(), GL_STATIC_DRAW);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VT), vertices.data(), GL_STATIC_DRAW);

        this->buildAttributesAndGetOffset(attributes);
    }

    template<typename VT, size_t NAttributes>
    Mesh(const std::vector<VT> &vertices, const std::array<MeshAttribute, NAttributes> &attributes, MeshTopology topology) : topology(static_cast<GLenum>(topology)) {
        glGenVertexArrays(1, &this->vao);
        glGenBuffers(1, &this->vbo);

        glBindVertexArray(this->vao);
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
        
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VT), vertices.data(), GL_STATIC_DRAW);
        
        size_t offset = this->buildAttributesAndGetOffset(attributes);
        this->numVertices = static_cast<GLsizei>(vertices.size() / (offset / sizeof(VT)));
    }
    ~Mesh();
    void render() const;
};