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
uniform int light_type[8];
uniform vec3 light_dir[8];
uniform vec3 light_color[8];
uniform vec3 light_function[8];
uniform vec3 light_pos[8];
uniform float light_penumbra[8];
uniform float light_angle[8];

// diffuse
uniform float kd;
uniform vec4 cDiffuse;

// specular
uniform float ks;
uniform vec4 cSpecular;
uniform float shininess;

// plane
uniform vec4 camera_pos;

void main() {
    // ambient
    fragColor = ka * vec4(cAmbient);

    for (int i = 0; i < light_num; i++) {
        vec3 normal = normalize(world_space_normal);
        float distance = distance(light_pos[i], vec3(world_space_pos));
        float attentuate = min(1.0f, float(1.0f/(light_function[i][0] + (light_function[i][1] * distance) + light_function[i][2] * pow(distance, 2.f))));
        vec3 L = normalize(light_pos[i] - vec3(world_space_pos));
        vec3 subnormal = dot(normal, L) * normal;
        vec3 R = normalize(2.f*subnormal - L) ;
        float specular, diffuse;

        switch(light_type[i]) {
        case 0:
            diffuse = max(dot(normalize(-light_dir[i]), normal), 0.f);
            if (shininess != 0) {
                specular = pow(max(dot(vec4(reflect(normalize(light_dir[i]), normal), 0.f), normalize(camera_pos - world_space_pos)), 0.f), shininess);
            } else {
                specular = 1.f;
            }

            for (int j = 0; j < 3; j++) {
                // diffuse
                fragColor[j] += kd * diffuse * light_color[i][j] * cDiffuse[j];

                // specular
                fragColor[j] += ks * specular * light_color[i][j] * cSpecular[j];
            }
            break;
        case 1:
            diffuse = max(0.f, dot(normal, L));
            if (shininess != 0) {
                specular = pow(max(0.f, dot(R, normalize(vec3(camera_pos) - vec3(world_space_pos)))), shininess);
            } else {
                specular = 1.f;
            }

            for (int j = 0; j < 3; j++) {
                // diffuse
                fragColor[j] += attentuate * kd * diffuse * light_color[i][j] * cDiffuse[j];

                // specular
                fragColor[j] += attentuate * ks * specular * light_color[i][j] * cSpecular[j];
            }
            break;
        case 2:
            vec3 light_to_pos = light_pos[i] - vec3(world_space_pos);
            float angle = acos(length(dot(light_to_pos, vec3(light_dir[i])) * vec3(light_dir[i])) / length(light_to_pos) / pow(length(light_dir[i]), 2));
            diffuse = max(0.f, dot(normal, L));
            if (shininess != 0) {
                specular = pow(max(0.f, dot(R, normalize(vec3(camera_pos) - vec3(world_space_pos)))), shininess);
            } else {
                specular = 1.f;
            }
            float falloff = 1.f;
            if (angle <= light_angle[i] - light_penumbra[i]) {
                falloff = 0;
            } else if (angle <= light_angle[i]) {
                float variable = ((angle - light_angle[i] + light_penumbra[i]) / (light_penumbra[i]));
                falloff = 3 * pow(variable, 2) - 2 * pow(variable, 3);
            }

            for (int j = 0; j < 3; j++) {
                // diffuse
                fragColor[j] += (1.f - falloff) * attentuate * kd * diffuse * light_color[i][j] * cDiffuse[j];

                // specular
                fragColor[j] += (1.f - falloff) * attentuate * ks * specular * light_color[i][j] * cSpecular[j];
            }
            break;
        }
    }


//    fragColor = vec4(world_space_normal, 1);
}
