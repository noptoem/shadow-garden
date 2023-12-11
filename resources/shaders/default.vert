#version 330 core
// input
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;

// output
out vec4 world_space_pos;
out vec3 world_space_normal;
out vec4 fragLight;

// mat4 to store model matrix
uniform mat4 model_matrix;
uniform mat3 model_matrix_transpose_inverse;

// view and projection matrix
uniform mat4 view_matrix;
uniform mat4 proj_matrix;

void main() {
    world_space_pos =  model_matrix * vec4(pos, 1);
    world_space_normal = model_matrix_transpose_inverse * normal;
    fragLight = vec4(pos, 1);
    gl_Position = proj_matrix * view_matrix * world_space_pos;
}
