#include "Object.hpp"
#include "shader/Shader.hpp"

namespace obj {
    SelectedMarker::SelectedMarker() {

    }

    void SelectedMarker::forward_render(glm::vec3 pos, float radius) const {
        auto sh = shader_instance();
        auto& vao = VAO::instance();

        auto model = glm::mat4(1.0);
        model = glm::scale(model, glm::vec3(radius, radius, 1.0));
        model = glm::translate(model, pos);
        sh->use_shader();
        sh->set_mat4("model", model);

        ::glBindVertexArray(vao.id);
        // ::glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, NULL);
        ::glDrawArrays(GL_POINTS, 0, 12);
        ::glBindVertexArray(0);
    }
}
