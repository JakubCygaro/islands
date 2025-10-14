#ifndef SINGLETONS_HPP
#define SINGLETONS_HPP
#include "shader/Shader.hpp"
#include <Game.hpp>
namespace gm{
    namespace singl {
        namespace shader_instances {
            enum class ShaderInstance : int {
                Text3D = 0,
                Star,
                ShadowMap,
                Selected,
                Normals,
                Planet,
                PlanetForward,
                Trail,
                Marker,
                Text,
                Grid,
                Skybox,
                LightPass,
                LightPassSphere,
                __end
            };
            void load_all();
            void unload_all();
            Shader* get_instance(ShaderInstance ins);

        }
    }
}

#endif
