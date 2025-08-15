#include "mesh.hpp"

static void updateLayout(VertexArray &vertexArray, const VertexBuffer &vertexBuffer) {
    vertexArray.bindVertexBuffer(vertexBuffer, {
        {
            .type = VertexAttributeType::Float,
            .size = VertexAttributeSize::Vec3,
        },
        {
            .type = VertexAttributeType::Float,
            .size = VertexAttributeSize::Vec2,
        },
        {
            .type = VertexAttributeType::Float,
            .size = VertexAttributeSize::Vec3,
        }
    });
}

Mesh::Mesh() {
    updateLayout(this->vertexArray, this->vertexBuffer);
}
Mesh::Mesh(const std::vector<Vertex> &data) {
    this->setData(data);
}
Mesh::~Mesh() {}

void Mesh::setData(const std::vector<Vertex> &data) {
    this->vertexBuffer.setData(data);
    updateLayout(this->vertexArray, this->vertexBuffer);
}
void Mesh::draw() const {
    this->vertexArray.drawArrays(Topology::TriangleList);
}