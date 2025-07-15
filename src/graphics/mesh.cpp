#include "mesh.hpp"

#include <stdio.h>

Mesh::~Mesh() {
    if (this->vao != 0) {
        glDeleteBuffers(1, &this->vao);
    }
    if (this->vbo != 0) {
        glDeleteBuffers(1, &this->vbo);
    }
}
void Mesh::render() const {
    glBindVertexArray(this->vao);
    glDrawArrays(this->topology, 0, this->numVertices);
}

IndexedMesh::~IndexedMesh() {
    if (this->vao != 0) {
        glDeleteVertexArrays(1, &this->vao);
    }
    if (this->ebo != 0) {
        glDeleteBuffers(1, &this->ebo);
    }
    if (this->vbo != 0) {
        glDeleteBuffers(1, &this->vbo);
    }
}
void IndexedMesh::render() const {
    glBindVertexArray(this->vao);
    glDrawElements(this->topology, this->numElements, GL_UNSIGNED_INT, nullptr);
}