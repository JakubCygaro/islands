#include "Object.hpp"
#include "shader/Shader.hpp"
#include <glm/fwd.hpp>

namespace obj {
    SelectedMarker::VAO ::VAO() {
        ::glGenVertexArrays(1, &id);
        ::glGenBuffers(1, &vbo);

        ::glBindVertexArray(id);

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

        ::glBindBuffer(GL_ARRAY_BUFFER, vbo);
        ::glBufferData(GL_ARRAY_BUFFER, sizeof(vbo_data), vbo_data, GL_STATIC_DRAW);

        ::glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        ::glEnableVertexAttribArray(0);

        ::glBindBuffer(GL_ARRAY_BUFFER, 0);
        ::glBindVertexArray(0);
    }
    SelectedMarker::VAO ::~VAO() {
        if(id){
            ::glDeleteVertexArrays(1, &id);
            id = 0;
        }
        if(vbo){
            ::glDeleteBuffers(1, &vbo);
            vbo = 0;
        }

    }
}
