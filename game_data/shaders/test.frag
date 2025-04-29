#version 460 core

out vec4 FragColor;
in vec3 VertColor;

void main() {
    FragColor = vec4(1.0f, VertColor.y,  VertColor.y, 1.0f);
}
