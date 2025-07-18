#include "mesh.hpp"
#include <stdio.h>

static GLuint meshBound = 0;

Mesh::~Mesh() {
    glDeleteBuffers(1, &this->vao);
    glDeleteBuffers(1, &this->ebo);
    glDeleteBuffers(1, &this->vbo);
}
void Mesh::render() const {
    if (meshBound != this->vao) {
        glBindVertexArray(this->vao);
        meshBound = this->vao;
    }

    if (this->ebo != 0) {
        glDrawElements(this->topology, this->numVertices, GL_UNSIGNED_INT, nullptr);
    } else {
        glDrawArrays(this->topology, 0, this->numVertices);
    }
}