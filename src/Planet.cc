#include <Object.hpp>

namespace obj {

    std::shared_ptr<Shader> Planet::shader_instance() {
        if(s_planet_shader){
            return s_planet_shader;
        } else {
            s_planet_shader = std::make_shared<Shader>(Shader::from_shader_dir(Planet::PLANET_SHADER_FILE));
            return s_planet_shader;
        }
    }
}
