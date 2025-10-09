#include "Skybox.hpp"


void Skybox::forward_render() const{
    auto& shader = ShaderInstance::get_instance().shader;

    ::glDepthMask(GL_FALSE);
    ::glBindVertexArray(m_cube_vao);
    shader.use_shader();
    ::glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, NULL);
    ::glDepthMask(GL_TRUE);
}
