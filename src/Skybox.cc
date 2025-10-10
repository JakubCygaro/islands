#include "Skybox.hpp"


void Skybox::forward_render() const{
    auto& shader = ShaderInstance::get_instance().shader;

    // ::glDisable(GL_CULL_FACE);
    ::glDepthFunc(GL_LEQUAL);
    ::glBindVertexArray(m_cube_vao);
    ::glActiveTexture(GL_TEXTURE0);
    ::glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap);
    shader.use_shader();
    ::glDrawArrays(GL_TRIANGLES, 0, Skybox::vert_count);
    ::glDepthFunc(GL_LESS);
    // ::glEnable(GL_CULL_FACE);
}
