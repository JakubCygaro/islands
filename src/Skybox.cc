#include "Skybox.hpp"
#include <Singletons.hpp>
using namespace gm::singl;
void Skybox::forward_render() const{
    auto* shader = shader_instances::get_instance(shader_instances::ShaderInstance::Skybox);

    ::glDepthFunc(GL_LEQUAL);
    ::glBindVertexArray(m_cube_vao);
    ::glActiveTexture(GL_TEXTURE0);
    ::glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap);
    shader->use_shader();
    ::glDrawArrays(GL_TRIANGLES, 0, Skybox::vert_count);
    ::glDepthFunc(GL_LESS);
    ::glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
