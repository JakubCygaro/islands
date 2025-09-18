#version 460 core

uniform vec4 color;

out vec4 FragColor;

in VS_OUT {
    float grid_alpha;
};

void main(){
    FragColor = vec4(color.rgb, clamp(color.a * grid_alpha, 0.0, 1.0));
}
