#include "Cylinder.h"

void Cylinder::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = param2;
    setVertexData();
}

void Cylinder::makeTileBase(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {

    glm::vec3 normal1 = glm::cross(-glm::normalize(topRight - topLeft), -glm::normalize(topRight - bottomRight));

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normal1);
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normal1);
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, normal1);


    glm::vec3 normal2 = glm::cross(-glm::normalize(bottomLeft - bottomRight), -glm::normalize(bottomLeft - topLeft));

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normal2);
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normal2);
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normal2);
}

void Cylinder::makeTileSide(glm::vec3 topLeft,
                      glm::vec3 topRight,
                      glm::vec3 bottomLeft,
                      glm::vec3 bottomRight) {
    // Task 5: Implement the makeTile() function for a Sphere
    // Note: this function is very similar to the makeTile() function for Cube,
    //       but the normals are calculated in a different way!
    // first triangle normal

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, glm::normalize(glm::vec3(topLeft[0], 0.f, topLeft[2])));
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, glm::normalize(glm::vec3(bottomRight[0], 0.f, bottomRight[2])));
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, glm::normalize(glm::vec3(topRight[0], 0.f, topRight[2])));

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, glm::normalize(glm::vec3(topLeft[0], 0.f, topLeft[2])));
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, glm::normalize(glm::vec3(bottomLeft[0], 0.f, bottomLeft[2])));
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, glm::normalize(glm::vec3(bottomRight[0], 0.f, bottomRight[2])));
}

void Cylinder::makeWedge(float currentTheta, float nextTheta) {
    // Task 6: create a glm::single wedge of the sphere uglm::sing the
    //         makeTile() function you implemented in Task 5
    // Note: think about how param 1 comes into play here!
    glm::vec3 topLeft;
    glm::vec3 topRight;
    glm::vec3 bottomLeft;
    glm::vec3 bottomRight;

    float distance = 1.f / m_param1;
    glm::vec3 current_offset = glm::vec3(0.5f * glm::sin(currentTheta), 0, 0.5f * glm::cos(currentTheta)) / float(m_param1);
    glm::vec3 next_offset = glm::vec3(0.5f * glm::sin(nextTheta), 0, 0.5f * glm::cos(nextTheta)) / float(m_param1);

    for (int i = 0; i < m_param1; i++) {
        // make side
        topLeft = glm::vec3(0.5f * glm::sin(currentTheta), -0.5f + distance * (i + 1), 0.5f * glm::cos(currentTheta));
        bottomLeft = glm::vec3(0.5f * glm::sin(currentTheta), -0.5f + distance * i, 0.5f * glm::cos(currentTheta));
        topRight = glm::vec3(0.5f * glm::sin(nextTheta), -0.5f + distance * (i + 1), 0.5f * glm::cos(nextTheta));
        bottomRight = glm::vec3(0.5f * glm::sin(nextTheta), -0.5f + distance * i, 0.5f * glm::cos(nextTheta));
        makeTileSide(topLeft, topRight, bottomLeft, bottomRight);
    }

    // make center triangle
    glm::vec3 normal = glm::vec3(0.f, 1.0f, 0.f);

    // for the top
    insertVec3(m_vertexData, glm::vec3(0.f, 0.5f, 0.f));
    insertVec3(m_vertexData, normal);
    insertVec3(m_vertexData, current_offset + glm::vec3(0.f, 0.5f, 0.f));
    insertVec3(m_vertexData, normal);
    insertVec3(m_vertexData, next_offset + glm::vec3(0.f, 0.5f, 0.f));
    insertVec3(m_vertexData, normal);

    // for the bottom
    insertVec3(m_vertexData, glm::vec3(0.f, -0.5f, 0.f));
    insertVec3(m_vertexData, -normal);
    insertVec3(m_vertexData, next_offset + glm::vec3(0.f, -0.5f, 0.f));
    insertVec3(m_vertexData, -normal);
    insertVec3(m_vertexData, current_offset + glm::vec3(0.f, -0.5f, 0.f));
    insertVec3(m_vertexData, -normal);

    for (int i = 1; i < m_param1; i++) {
        // make top
        topLeft = glm::vec3(0.f, 0.5f, 0.f) + float(i) * current_offset;
        bottomLeft = glm::vec3(0.f, 0.5f, 0.f) + float(i + 1) * current_offset;
        topRight = glm::vec3(0.f, 0.5f, 0.f) + float(i) * next_offset;
        bottomRight = glm::vec3(0.f, 0.5f, 0.f) + float(i + 1) * next_offset;
        makeTileBase(topLeft, topRight, bottomLeft, bottomRight);

        // make bottom
        topRight = glm::vec3(0.f, -0.5f, 0.f) + float(i) * current_offset;
        bottomRight = glm::vec3(0.f, -0.5f, 0.f) + float(i + 1) * current_offset;
        topLeft = glm::vec3(0.f, -0.5f, 0.f) + float(i) * next_offset;
        bottomLeft = glm::vec3(0.f, -0.5f, 0.f) + float(i + 1) * next_offset;
        makeTileBase(topLeft, topRight, bottomLeft, bottomRight);
    }

}

void Cylinder::setVertexData() {
    // TODO for Project 5: Lights, Camera
    float thetaStep = glm::radians(360.f / m_param2);

    for (int i = 0; i < m_param2; i++) {
        float currentTheta = i * thetaStep;
        float nextTheta = (i + 1) * thetaStep;
        makeWedge(currentTheta, nextTheta);
    }
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cylinder::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
