#version 460 core
layout (location = 0) in vec3 pos;

out vec3 texture_coords;

layout(std140, binding = 0) uniform Matrices {
    mat4 view;
    mat4 projection;
    mat4 text_projection;
};

void main()
{
    texture_coords = pos;
    mat4 view_no_translate = mat4(mat3(view));
    vec4 pos = projection * view_no_translate * vec4(pos, 1.0);
    gl_Position = pos.xyww;
}
