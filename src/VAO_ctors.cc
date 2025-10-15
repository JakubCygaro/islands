#include "Object.hpp"
#include <cstdio>
#include <glm/fwd.hpp>

namespace obj {
    SelectedMarkerVAO::SelectedMarkerVAO() {
        ::glGenVertexArrays(1, &m_vao);
        ::glGenBuffers(1, &m_vbo);

        ::glBindVertexArray(m_vao);

        glm::vec3 vbo_data[] = {
            // top triangle
            { 0, 1.05, 0 },
            { 0.2, 1.2, 0 },
            { -0.2, 1.2, 0 },
            // right triangle
            { 1.05, 0, 0 },
            { 1.2, -0.2, 0 },
            { 1.2, 0.2, 0 },
            // bottom triangle
            { 0, -1.05, 0 },
            { -0.2, -1.2, 0 },
            { 0.2, -1.2, 0 },
            // left triangle
            { -1.05, 0, 0 },
            { -1.2, 0.2, 0 },
            { -1.2, -0.2, 0 },
        };

        ::glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        ::glBufferData(GL_ARRAY_BUFFER, sizeof(vbo_data), vbo_data, GL_STATIC_DRAW);

        ::glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        ::glEnableVertexAttribArray(0);

        ::glBindBuffer(GL_ARRAY_BUFFER, 0);
        ::glBindVertexArray(0);
    }
    SelectedMarkerVAO::SelectedMarkerVAO(SelectedMarkerVAO&& other): VertexArrrayObject(std::move(other)) {

    };
    SelectedMarkerVAO& SelectedMarkerVAO::operator=(SelectedMarkerVAO&& other){
        VertexArrrayObject::operator=(std::move(other));
        return *this;
    }
    SelectedMarkerVAO::~SelectedMarkerVAO() {
    }
}
