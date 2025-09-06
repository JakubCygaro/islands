#version 460 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;


layout(std140, binding = 1) uniform LightingGlobals {
    float ambient_strength;
    vec3 camera_pos;
};
uniform mat4 shadow_trans[6];

out vec4 FragPos;

void main(){
    for(int face = 0; face < 6; face++){
        gl_Layer = face;
        for(int i = 0; i < 3; i++){
            FragPos = gl_in[i].gl_Position;
            gl_Position = shadow_trans[face] * FragPos;
            EmitVertex();
        }
        EndPrimitive();
    }
}
