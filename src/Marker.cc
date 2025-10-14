#include "Object.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include "shader/Shader.hpp"
#include <Singletons.hpp>
using namespace gm::singl;
namespace obj {
    SelectedMarker::SelectedMarker() {}

    void SelectedMarker::forward_render(const glm::vec3& camera_pos, glm::vec3 pos, float radius) const {
        (void)camera_pos;
        auto sh = shader_instances::get_instance(shader_instances::ShaderInstance::Marker);
        auto& vao = VAO::instance();

        auto model = glm::mat4(1.0);

        auto scaleup = radius;

        auto scale = glm::scale(model, glm::vec3(scaleup, scaleup, 1.0));
        auto scaling = glm::mat3(scale[0], scale[1], scale[2]);

        model = glm::translate(model, pos);

        sh->use_shader();
        sh->set_mat4("model", model);
        sh->set_mat3(name_of(scaling), scaling);
        sh->set_vec3("center", pos);

        ::glBindVertexArray(vao.id);
        ::glDisable(GL_CULL_FACE);
        ::glDrawArrays(GL_TRIANGLES, 0, 12);
        ::glEnable(GL_CULL_FACE);
        ::glBindVertexArray(0);
    }
}
