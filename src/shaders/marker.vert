#version 460 core

layout(location = 0) in vec3 pos;

uniform mat4 model;
uniform mat3 scaling;

layout(std140, binding = 0) uniform Matrices {
    mat4 view;
    mat4 projection;
    mat4 text_projection;
};

layout(std140, binding = 1) uniform LightingGlobals {
    float ambient_strength;
    vec3 camera_pos;
};

void main() {
    mat4 model_view = view * model;
    // https://stackoverflow.com/a/66951138
    // https://stackoverflow.com/a/5487981
    // we basically need to nuke the Rotation part of the matrix,
    // but since we want to preserve the scaling, the indentity matrix
    // (that goes in the place of the Rotation section) has to be scaled
    // and put it the place of the Rotation matrix
    // I hope this shit makes sense in 10 years
    model_view[0].xyz = scaling[0];
    model_view[1].xyz = scaling[1];

    vec4 viewspace = model_view * vec4(pos, 1.0);

    gl_Position = projection * viewspace;
    gl_Position.z = 0.0;
}
