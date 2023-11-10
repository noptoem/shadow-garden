#version 330 core
// output
in vec4 world_space_pos;
in vec3 world_space_normal;

// output
out vec4 fragColor;

// color value
uniform float ka;
uniform vec4 cAmbient;

// light
uniform int light_num;
uniform vec3 light_dir[8];
uniform vec3 light_color[8];

// diffuse
uniform float kd;
uniform vec4 cDiffuse;

// specular
uniform float ks;
uniform vec4 cSpecular;
uniform float shininess;

uniform vec4 camera_pos;

void main() {
    // ambient
    fragColor = ka * vec4(cAmbient);

    for (int i = 0; i < light_num; i++) {
        float diffuse = min(max(dot(normalize(-light_dir[i]), world_space_normal), 0), 1);
        float specular = pow(min(max(dot(vec4(reflect(normalize(light_dir[i]), world_space_normal), 0), normalize(camera_pos - world_space_pos)), 0), 1), shininess);
        for (int j = 0; j < 3; j++) {
            // diffuse
            fragColor[j] += kd * diffuse * light_color[i][j] * cDiffuse[j];

            // specular
            fragColor[j] += ks * specular * light_color[i][j] * cSpecular[j];
        }

    }
//    fragColor = vec4(1);
}
