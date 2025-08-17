#include "buffers.hpp"

static uint32_t boundVertexBuffer = 0;
static uint32_t boundElementBuffer = 0;
static uint32_t boundVertexArray = 0;

static constexpr GLenum getVertexBufferUsageGL(VertexBufferUsage usage) {
    switch (usage) {
        case VertexBufferUsage::Static:
            return GL_STATIC_DRAW;
        case VertexBufferUsage::Dynamic:
            return GL_DYNAMIC_DRAW;
        default:
            return GL_STATIC_DRAW;
    }
}
static constexpr GLenum getTopologyGL(Topology topology) {
    switch (topology) {
        case Topology::TriangleList:
            return GL_TRIANGLES;
        case Topology::TriangleFan:
            return GL_TRIANGLE_FAN;
        case Topology::TriangleStrip:
            return GL_TRIANGLE_STRIP;
        case Topology::LineList:
            return GL_LINES;
        case Topology::LineStrip:
            return GL_LINE_STRIP;
        case Topology::LineLoop:
            return GL_LINE_LOOP;
        default:
            return GL_TRIANGLES;
    }
}

static constexpr uint32_t getVertexAttributeTypeSizeInBytes(VertexAttributeType type) {
    switch (type) {
        case VertexAttributeType::Double:
            return sizeof(double);
        case VertexAttributeType::Float:
            return sizeof(float);
        case VertexAttributeType::UnsignedInt:
            return sizeof(unsigned int);
        case VertexAttributeType::Int:
            return sizeof(int);
        case VertexAttributeType::UnsignedShort:
            return sizeof(unsigned short);
        case VertexAttributeType::Short:
            return sizeof(short);
        case VertexAttributeType::UnsignedByte:
            return sizeof(unsigned char);
        case VertexAttributeType::Byte:
            return sizeof(char);
        default:
            return sizeof(float);
    }
}
static constexpr GLuint getVertexAttributeDivisorGL(VertexAttributeDivisor divisor) {
    switch (divisor) {
        case VertexAttributeDivisor::PerVertex:
            return 0;
        case VertexAttributeDivisor::PerInstance:
            return 1;
        default:
            return 0;
    }
}

void VertexBuffer::setRawData(const void *data, uint32_t size) {
    this->bind();
    glBufferData(GL_ARRAY_BUFFER, size, data, getVertexBufferUsageGL(this->usage));
    this->sizeInBytes = size;
}

VertexBuffer::VertexBuffer(VertexBufferUsage usage) : usage(usage) {
    glGenBuffers(1, &this->id);
}
VertexBuffer::~VertexBuffer() {
    glDeleteBuffers(1, &this->id);
}

void VertexBuffer::bind() const {
    if (boundVertexBuffer != this->id) {
        glBindBuffer(GL_ARRAY_BUFFER, this->id);
        boundVertexBuffer = this->id;
    }
}

uint32_t VertexBuffer::getSizeInBytes() const {
    return this->sizeInBytes;
}

ElementBuffer::ElementBuffer(VertexBufferUsage usage) : usage(usage) {
    glGenBuffers(1, &this->id);
}
ElementBuffer::~ElementBuffer() {
    glDeleteBuffers(1, &this->id);
}

void ElementBuffer::bind() const {
    if (boundElementBuffer != this->id) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id);
        boundElementBuffer = this->id;
    }
}
void ElementBuffer::setData(const std::vector<uint32_t> &data) {
    this->bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.size() * sizeof(uint32_t), data.data(), getVertexBufferUsageGL(this->usage));
    this->numElements = data.size();
}

uint32_t ElementBuffer::getNumElements() const {
    return this->numElements;
}

VertexArray::VertexArray() {
    glGenVertexArrays(1, &this->id);
}
VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &this->id);
}

void VertexArray::bind() const {
    if (boundVertexArray != this->id) {
        glBindVertexArray(this->id);
        boundVertexArray = this->id;
        boundVertexBuffer = 0;
    }
}

void VertexArray::drawArrays(Topology topology) const {
    if (this->numVertices <= 0) return;

    this->bind();
    glDrawArrays(getTopologyGL(topology), 0, this->numVertices);
}
void VertexArray::drawArraysInstanced(Topology topology, uint32_t numInstances) const {
    if (this->numVertices <= 0 || numInstances <= 0) return;

    this->bind();
    glDrawArraysInstanced(getTopologyGL(topology), 0, this->numVertices, numInstances);
}
void VertexArray::drawElements(Topology topology) const {
    if (this->numVertices <= 0) return;

    this->bind();
    glDrawElements(getTopologyGL(topology), this->numVertices, GL_UNSIGNED_INT, nullptr);
}
void VertexArray::drawElementsInstanced(Topology topology, uint32_t numInstances) const {
    if (this->numVertices <= 0 || numInstances <= 0) return;
    
    this->bind();
    glDrawElementsInstanced(getTopologyGL(topology), this->numVertices, GL_UNSIGNED_INT, nullptr, numInstances);
}

void VertexArray::bindElementBuffer(const ElementBuffer &buffer) {
    this->bind();
    buffer.bind();

    this->numVertices = buffer.getNumElements();
}
void VertexArray::bindVertexBuffer(const VertexBuffer &buffer, const std::vector<VertexAttribute> &attributes) {
    this->bind();
    buffer.bind();

    uint32_t stride = 0;
    for (size_t i = 0; i < attributes.size(); i++) {
        stride += static_cast<uint32_t>(attributes[i].size) * static_cast<uint32_t>(getVertexAttributeTypeSizeInBytes(attributes[i].type));
    }

    if (this->numVertices == 0) {
        this->numVertices = buffer.getSizeInBytes() / stride;
    }

    uintptr_t offset = 0;
    for (size_t i = 0; i < attributes.size(); i++) {
        uint32_t index = static_cast<uint32_t>(this->numAttributes + i);
        glEnableVertexAttribArray(index);

        switch (attributes[i].type) {
            case VertexAttributeType::Double:
                glVertexAttribLPointer(index, static_cast<GLint>(attributes[i].size), GL_DOUBLE, stride, (void*)offset);
                offset += static_cast<uint32_t>(attributes[i].size) * sizeof(double);
                break;
            case VertexAttributeType::Float:
                glVertexAttribPointer(index, static_cast<GLint>(attributes[i].size), GL_FLOAT, GL_FALSE, stride, (void*)offset);
                offset += static_cast<uint32_t>(attributes[i].size) * sizeof(float);
                break;
            case VertexAttributeType::UnsignedInt:
                glVertexAttribIPointer(index, static_cast<GLint>(attributes[i].size), GL_UNSIGNED_INT, stride, (void*)offset);
                offset += static_cast<uint32_t>(attributes[i].size) * sizeof(unsigned int);
                break;
            case VertexAttributeType::Int:
                glVertexAttribIPointer(index, static_cast<GLint>(attributes[i].size), GL_INT, stride, (void*)offset);
                offset += static_cast<uint32_t>(attributes[i].size) * sizeof(int);
                break;
            case VertexAttributeType::UnsignedShort:
                glVertexAttribIPointer(index, static_cast<GLint>(attributes[i].size), GL_UNSIGNED_SHORT, stride, (void*)offset);
                offset += static_cast<uint32_t>(attributes[i].size) * sizeof(unsigned short);
                break;
            case VertexAttributeType::Short:
                glVertexAttribIPointer(index, static_cast<GLint>(attributes[i].size), GL_SHORT, stride, (void*)offset);
                offset += static_cast<uint32_t>(attributes[i].size) * sizeof(short);
                break;
            case VertexAttributeType::UnsignedByte:
                glVertexAttribIPointer(index, static_cast<GLint>(attributes[i].size), GL_UNSIGNED_BYTE, stride, (void*)offset);
                offset += static_cast<uint32_t>(attributes[i].size) * sizeof(unsigned char);
                break;
            case VertexAttributeType::Byte:
                glVertexAttribIPointer(index, static_cast<GLint>(attributes[i].size), GL_BYTE, stride, (void*)offset);
                offset += static_cast<uint32_t>(attributes[i].size) * sizeof(char);
                break;
            default:
                break;
        }

        glVertexAttribDivisor(index, getVertexAttributeDivisorGL(attributes[i].divisor));
    }

    this->numAttributes += static_cast<uint32_t>(attributes.size());
}

void VertexArray::updateVertexCount(const VertexBuffer &buffer) {
    if (this->numVertices == 0) {
        this->numVertices = buffer.getSizeInBytes() / this->numAttributes;
    } else {
        uint32_t newVertexCount = buffer.getSizeInBytes() / this->numAttributes;
        if (newVertexCount != this->numVertices) {
            fprintf(stderr, "Warning: Vertex count mismatch! Expected %u, got %u\n", this->numVertices, newVertexCount);
            this->numVertices = newVertexCount;
        }
    }
}
void VertexArray::updateElementCount(const ElementBuffer &buffer) {
    this->numVertices = buffer.getNumElements();
}

uint32_t VertexArray::getId() const {
    return this->id;
}
uint32_t VertexArray::getVertexCount() const {
    return this->numVertices;
}