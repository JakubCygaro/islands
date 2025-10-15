#ifndef VERTEX_ARRAY_OBJECT_HPP
#define VERTEX_ARRAY_OBJECT_HPP
#include <cstdint>
class VertexArrrayObject {
protected:
    uint32_t m_vao{}, m_vbo{}, m_ebo{};
    VertexArrrayObject(uint32_t vao, uint32_t vbo, uint32_t ebo);
public:
    VertexArrrayObject();
    VertexArrrayObject(const VertexArrrayObject&) = delete;
    VertexArrrayObject& operator=(const VertexArrrayObject&) = delete;
    VertexArrrayObject(VertexArrrayObject&&);
    VertexArrrayObject& operator=(VertexArrrayObject&&);
    virtual ~VertexArrrayObject();

    virtual void bind() const;
    virtual void unbind() const;
};

#endif
