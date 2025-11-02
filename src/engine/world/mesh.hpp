#pragma once
#include <framework/graphics/buffers.hpp>

struct Vertex {
    float x, y, z;
    float u, v;
    float nx, ny, nz;
};

class Mesh {
private:
    VertexBuffer vertexBuffer = VertexBuffer(VertexBufferUsage::Static);
    VertexArray vertexArray = VertexArray();
public:
    Mesh();
    explicit Mesh(const std::vector<Vertex> &data);
    ~Mesh();

    void setData(const std::vector<Vertex> &data);
    void draw() const;
};