#version 460 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 tex_coords;

uniform mat4 projection;

out vec2 texture_coords;

void main(){
   vec4 pos = projection * vec4(position.xy, -1.0, 1.0);
   texture_coords = tex_coords;
   gl_Position = vec4(pos.x, pos.y, -1.0, 1.0);
}
