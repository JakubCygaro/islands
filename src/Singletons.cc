#include "Font.hpp"
#include <Singletons.hpp>
#include <cstddef>
#include <shader/Shader.hpp>
#include <string>
#include <files.hpp>
#include <VertexArrayObject.hpp>
#ifdef DEBUG
#include <shader_files.hpp>
#else
#include <shaders.hpp>
#endif

#ifdef DEBUG
#define VERT(NAME)\
    files::src::shaders::NAME ## _VERT
#else
    shaders::NAME ## _VERT
#endif
#ifdef DEBUG
#define FRAG(NAME)\
    files::src::shaders::NAME ## _FRAG
#else
    shaders::NAME ## _FRAG
#endif
#ifdef DEBUG
#define GEOM(NAME)\
    files::src::shaders::NAME ## _GEOM
#else
    shaders::NAME ## _GEOM
#endif


namespace gm::singl::shader_instances {
    namespace {
        static std::vector<Shader> INSTANCES(static_cast<int>(ShaderInstance::__end));
        Shader load_shader(const char* vert, const char* frag, const char* geom = nullptr){
#ifdef DEBUG
            //load directly from source tree -> works without whole project rebuild
            std::string geom_as_str;
            std::string* geom_ptr = nullptr;
            if(geom){
                geom_as_str = std::string(geom);
                geom_ptr = &geom_as_str;
            }
            auto shader = Shader(
                        std::string(vert),
                        std::string(frag),
                        geom_ptr
                        );
#else
            auto shader = Shader(
                        vert,
                        frag,
                        geom
                        );
#endif
            return shader;
        }
        void load_shader_instance(ShaderInstance instance_name, const char* v, const char* f, const char* g = nullptr){
            INSTANCES[static_cast<int>(instance_name)] = load_shader(v, f, g);
        }
    }
    void load_all(){
        load_shader_instance(ShaderInstance::Text3D, VERT(TEXT3D), FRAG(TEXT3D));
        load_shader_instance(ShaderInstance::Star, VERT(STAR), FRAG(STAR));
        load_shader_instance(ShaderInstance::ShadowMap, VERT(SHADOW_MAP), FRAG(SHADOW_MAP), GEOM(SHADOW_MAP));
        load_shader_instance(ShaderInstance::Selected, VERT(SELECTED), FRAG(SELECTED), GEOM(SELECTED));
        load_shader_instance(ShaderInstance::Normals, VERT(NORMALS), FRAG(NORMALS), GEOM(NORMALS));
        load_shader_instance(ShaderInstance::Planet, VERT(PLANET), FRAG(PLANET));
        load_shader_instance(ShaderInstance::PlanetForward, VERT(PLANET_FORWARD), FRAG(PLANET_FORWARD));
        load_shader_instance(ShaderInstance::Trail, VERT(TRAIL), FRAG(TRAIL));
        load_shader_instance(ShaderInstance::Marker, VERT(MARKER), FRAG(MARKER));
        load_shader_instance(ShaderInstance::Text, VERT(TEXT), FRAG(TEXT));
        load_shader_instance(ShaderInstance::Grid, VERT(GRID), FRAG(GRID));
        load_shader_instance(ShaderInstance::Skybox, VERT(SKYBOX), FRAG(SKYBOX));
        load_shader_instance(ShaderInstance::LightPass, VERT(LIGHT_PASS), FRAG(LIGHT_PASS));
        load_shader_instance(ShaderInstance::LightPassSphere, VERT(LIGHT_PASS_SPHERE), FRAG(LIGHT_PASS_SPHERE));
    };
    void unload_all(){
        INSTANCES.clear();
    };
    Shader* get_instance(ShaderInstance ins){
        return &INSTANCES[static_cast<int>(ins)];
    }
}
namespace gm::singl::font_instances {
    namespace {
        static font::FontBitmap* INSTANCE = nullptr;
    }
    void load_default_font(){
        INSTANCE = reinterpret_cast<font::FontBitmap*>(new unsigned char [sizeof(font::FontBitmap)]);
        *INSTANCE = font::load_font(files::game_data::fonts::ARCADE_TTF, 48);
    }
    void unload_default_font(){
        delete INSTANCE;
    }
    font::FontBitmap* get_default_font_instance(){
        return INSTANCE;
    }
}
namespace gm::singl::buffer_instances {
    namespace {
        void load_buffer_instance(BufferInstance ins, VertexArrrayObject* vao){
            BUFFERS[static_cast<int>(ins)] = vao;
        }
    }
    void load_all(){
        load_buffer_instance(BufferInstance::SelectedMarker, static_cast<VertexArrrayObject*>(new obj::SelectedMarkerVAO()));
    }
    void unload_all(){
        for(size_t i = 0; i < sizeof(BUFFERS) / sizeof(VertexArrrayObject*); i++){
            delete BUFFERS[i];
        }
    }
}
