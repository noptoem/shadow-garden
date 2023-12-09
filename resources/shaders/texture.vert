#version 330 core

// Task 15: add a second layout variable representing a UV coordinate
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv_in;

// Task 16: create an "out" variable representing a UV coordinate
out vec2 uv;
out vec4 fragPosLightSpace;

//uniform mat4 model_matrix;
//uniform mat4 lightSpaceMatrix;

void main() {
    // Task 16: assign the UV layout variable to the UV "out" variable
    uv = uv_in;

//    vec3 pos_new = vec3(model_matrix * vec4(position, 1.0));
    fragPosLightSpace = vec4(position, 1.0);
    gl_Position = vec4(position, 1.0);
}
