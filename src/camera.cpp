#include "realtime.h"

#include "utils/sceneparser.h"
#include "settings.h"
#include "math.h"

void Realtime::setupViewMatrix() {
    // generate view matrix
    glm::vec3 pos = renderData.cameraData.pos;
    glm::vec3 up = renderData.cameraData.up;
    glm::vec3 look = renderData.cameraData.look;

    glm::mat4 m_translate = glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, -pos[0], -pos[1], -pos[2], 1);
    glm::mat4 m_rotate = glm::mat4(1);

    glm::vec3 w = glm::normalize(-look);
    glm::vec3 v = glm::normalize(up - (glm::dot(up, w) * w));
    glm::vec3 u = glm::cross(v, w);

    m_rotate[0][0] = u[0];
    m_rotate[0][1] = v[0];
    m_rotate[0][2] = w[0];
    m_rotate[1][0] = u[1];
    m_rotate[1][1] = v[1];
    m_rotate[1][2] = w[1];
    m_rotate[2][0] = u[2];
    m_rotate[2][1] = v[2];
    m_rotate[2][2] = w[2];

    m_view = m_rotate * m_translate;
    m_view_inverse = glm::inverse(m_view);
}

glm::mat4 Realtime::setupLightViewMatrix(glm::vec3 pos, glm::vec3 look, glm::vec3 up) {
    // generate view matrix
    glm::mat4 m_translate = glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, -pos[0], -pos[1], -pos[2], 1);
    glm::mat4 m_rotate = glm::mat4(1);

    glm::vec3 w = glm::normalize(-look);
    glm::vec3 v = glm::normalize(up - (glm::dot(up, w) * w));
    glm::vec3 u = glm::cross(v, w);

    m_rotate[0][0] = u[0];
    m_rotate[0][1] = v[0];
    m_rotate[0][2] = w[0];
    m_rotate[1][0] = u[1];
    m_rotate[1][1] = v[1];
    m_rotate[1][2] = w[1];
    m_rotate[2][0] = u[2];
    m_rotate[2][1] = v[2];
    m_rotate[2][2] = w[2];

    return m_rotate * m_translate;
}

void Realtime::setupProjMatrix() {
    glm::mat4 remap = glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, -2, 0, 0, 0, -1, 1);
    glm::mat4 scale = glm::mat4(1);
    scale[0][0] = 1/ (aspectRatio * settings.farPlane * tan(renderData.cameraData.heightAngle/ 2));
    scale[1][1] = 1/ (settings.farPlane * tan(renderData.cameraData.heightAngle/ 2));
    scale[2][2] = 1/ settings.farPlane;

    float c = -settings.nearPlane / settings.farPlane;
    // switch side (matrix)
    glm::mat4 unhinge = glm::mat4(1, 0, 0, 0,
                                  0, 1, 0, 0,
                                  0, 0, 1/(1+c), -1,
                                  0, 0, -c/(1+c), 0);

    m_proj = remap * unhinge * scale;
}
