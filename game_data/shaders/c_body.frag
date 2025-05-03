#version 460 core

out vec4 FragColor;
in vec3 VertColor;

void main() {
    FragColor = vec4(VertColor.xyz, 1.0f);
}
