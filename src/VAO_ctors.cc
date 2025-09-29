#include "Object.hpp"
#include "shader/Shader.hpp"
#include <glm/fwd.hpp>

namespace obj {
    SelectedMarker::VAO ::VAO() {
        ::glGenVertexArrays(1, &id);
        ::glGenBuffers(1, &vbo);
        ::glGenBuffers(1, &ebo);

        ::glBindVertexArray(id);

        glm::vec3 vbo_data[] = {
            // top triangle
            { 0, 1, 0 },
            { -0.2, 1.5, 0 },
            { 0.2, 1.5, 0 },
            // right triangle
            { 1, 0, 0 },
            { 1.5, -0.2, 0 },
            { 1.5, 0.2, 0 },
            // bottom triangle
            { 0, -1, 0 },
            { -0.2, -1.5, 0 },
            { 0.2, -1.5, 0 },
            // left triangle
            { -1, 0, 0 },
            { -1.5, -0.2, 0 },
            { -1.5, 0.2, 0 },
        };
        uint32_t ebo_data[] = {
            0, 1, 2,
            3, 4, 5,
            6, 7, 8,
            9, 10, 11
        };

        ::glBindBuffer(GL_ARRAY_BUFFER, vbo);
        ::glBufferData(GL_ARRAY_BUFFER, sizeof(vbo_data), vbo_data, GL_STATIC_DRAW);

        ::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        ::glBufferData(GL_ARRAY_BUFFER, sizeof(ebo_data), ebo_data, GL_STATIC_DRAW);

        ::glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        ::glEnableVertexAttribArray(0);

        ::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        ::glBindVertexArray(0);
    }
    SelectedMarker::VAO ::~VAO() {
        if(id){
            ::glDeleteVertexArrays(1, &id);
        }
        if(vbo){
            ::glDeleteBuffers(1, &vbo);
        }
        if(ebo){
            ::glDeleteBuffers(1, &ebo);
        }

    }
}
