#ifndef SINGLETONS_HPP
#define SINGLETONS_HPP
#include "Font.hpp"
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
        namespace font_instances {
            void load_default_font();
            void unload_default_font();
            font::FontBitmap* get_default_font_instance();
        }
        namespace buffer_instances {
            enum class BufferInstance : int {
                SelectedMarker,
                UnitSphere,
                MoveVector,
                __end
            };
            inline VertexArrrayObject* BUFFERS[static_cast<int>(BufferInstance::__end)] = { };
            void load_all();
            void unload_all();
            template<typename T>
            inline static T* get_instance(BufferInstance ins){
                static_assert(std::is_base_of_v<VertexArrrayObject, T> || std::is_same_v<T, VertexArrrayObject>,
                        "T has to be derived from the VertexArrrayObject interface");
                auto* b = BUFFERS[static_cast<int>(ins)];
                auto* ret = dynamic_cast<T*>(b);
                return ret;
            }
        }
    }
}

#endif
