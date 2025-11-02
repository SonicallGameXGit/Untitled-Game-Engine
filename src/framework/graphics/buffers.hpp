#pragma once
#include <glad/glad.h>
#include <vector>
#include <cstdio>

enum class VertexBufferUsage : uint8_t {
    Static, Dynamic
};

class VertexBuffer {
private:
    uint32_t id = 0;
    uint32_t sizeInBytes = 0;

    VertexBufferUsage usage = VertexBufferUsage::Static;
    void setRawData(const void *data, uint32_t size);
public:
    explicit VertexBuffer(VertexBufferUsage usage);

    template<typename T>
    VertexBuffer(const std::vector<T> &data, VertexBufferUsage usage) : VertexBuffer(usage) {
        this->setData(data);
    }
    ~VertexBuffer();

    void bind() const;

    template<typename T>
    void setData(const std::vector<T> &data) {
        this->setRawData(data.data(), static_cast<uint32_t>(data.size() * sizeof(T)));
    }

    uint32_t getSizeInBytes() const;
};
class ElementBuffer {
private:
    uint32_t id = 0;
    uint32_t numElements = 0;

    VertexBufferUsage usage = VertexBufferUsage::Static;
public:
    ElementBuffer(VertexBufferUsage usage);
    ~ElementBuffer();

    void bind() const;
    void setData(const std::vector<uint32_t> &data);

    uint32_t getNumElements() const;
};

enum class VertexAttributeType : uint8_t {
    Double, Float,
    UnsignedInt, Int,
    UnsignedShort, Short,
    UnsignedByte, Byte
};
enum class VertexAttributeSize : uint8_t {
    Single = 1,
    Vec2 = 2,
    Vec3 = 3,
    Vec4 = 4
};
enum class VertexAttributeDivisor : uint8_t {
    PerVertex,
    PerInstance,
};
struct VertexAttribute {
    VertexAttributeType type = VertexAttributeType::Float;
    VertexAttributeSize size = VertexAttributeSize::Vec3;
    VertexAttributeDivisor divisor = VertexAttributeDivisor::PerVertex;
};
enum class Topology {
    TriangleList, TriangleFan, TriangleStrip,
    LineList, LineStrip, LineLoop
};

class VertexArray {
private:
    uint32_t id = 0;
    uint32_t numVertices = 0;
    uint32_t numAttributes = 0;
public:
    VertexArray();
    ~VertexArray();

    void bind() const;

    void drawArrays(Topology topology) const;
    void drawArraysInstanced(Topology topology, uint32_t numInstances) const;
    void drawElements(Topology topology) const;
    void drawElementsInstanced(Topology topology, uint32_t numInstances) const;

    void bindElementBuffer(const ElementBuffer &buffer);
    void bindVertexBuffer(const VertexBuffer &buffer, const std::vector<VertexAttribute> &attributes);

    void updateVertexCount(const VertexBuffer &buffer);
    void updateElementCount(const ElementBuffer &buffer);

    uint32_t getId() const;
    uint32_t getVertexCount() const;
};