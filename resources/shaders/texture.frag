#version 330 core

// Task 16: Create a UV coordinate in variable
in vec2 uv;

// Task 8: Add a sampler2D uniform
uniform sampler2D texture_id;

// Task 29: Add a bool on whether or not to filter the texture
uniform bool perPixel;
uniform bool kernelBased;
uniform bool grayScale;
uniform bool sharphen;

out vec4 fragColor;

void main()
{
    // Task 17: Set fragColor using the sampler2D at the UV coordinate
    fragColor = texture(texture_id, uv);
    float kernel[25] = float[25] (
            1.f, 1.f, 1.f, 1.f, 1.f,
            1.f, 1.f, 1.f, 1.f, 1.f,
            1.f, 1.f, 1.f, 1.f, 1.f,
            1.f, 1.f, 1.f, 1.f, 1.f,
            1.f, 1.f, 1.f, 1.f, 1.f
    );

    float kernelSum = 25.f;

    if (kernelBased && !sharphen) {
        fragColor = vec4(0);
        for (int i = -2; i <= 2; ++i) {
            for (int j = -2; j <= 2; ++j) {
                fragColor += texture(texture_id, uv + vec2(i, j) / textureSize(texture_id, 0)) * kernel[(i + 2) * 5 + (j + 2)] / kernelSum;
            }
        }
    }

    float sharphenMat[9] = float[9] (
            -1.f, -1.f, -1.f,
            -1.f, 17.f, -1.f,
            -1.f, -1.f, -1.f
    );

    float sharphenSum = 9.f;
    if (!kernelBased && sharphen) {
        fragColor = vec4(0);
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                fragColor += texture(texture_id, uv + vec2(i, j) / textureSize(texture_id, 0)) * sharphenMat[(i + 1) * 3 + (j + 1)] / sharphenSum;
            }
        }
    }

    if (grayScale) {
        fragColor = vec4(vec3(0.299 * fragColor[0] + 0.587 * fragColor[1] + 0.114 * fragColor[2]), 1);
    }

    // Invert fragColor's r, g, and b color channels if your bool is true
    if (perPixel) {
        fragColor = vec4(1) - fragColor;
    }
}
