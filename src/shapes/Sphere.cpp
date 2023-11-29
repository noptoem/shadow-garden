#include "Sphere.h"

void Sphere::updateParams(int param1, int param2) {
    if (m_param1 != param1 || m_param2 != param2) {
        m_vertexData = std::vector<float>();
        m_param1 = param1;
        m_param2 = param2;
        setVertexData();
    }
}

void Sphere::makeTile(glm::vec3 topLeft,
                      glm::vec3 topRight,
                      glm::vec3 bottomLeft,
                      glm::vec3 bottomRight) {
    // Task 5: Implement the makeTile() function for a Sphere
    // Note: this function is very similar to the makeTile() function for Cube,
    //       but the normals are calculated in a different way!
    // first triangle normal

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, glm::normalize(topLeft));
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, glm::normalize(bottomRight));
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, glm::normalize(topRight));

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, glm::normalize(topLeft));
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, glm::normalize(bottomLeft));
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, glm::normalize(bottomRight));
}

void Sphere::makeWedge(float currentTheta, float nextTheta) {
    // Task 6: create a glm::single wedge of the sphere uglm::sing the
    //         makeTile() function you implemented in Task 5
    // Note: think about how param 1 comes into play here!
    float phi = glm::radians(180.f / float(m_param1));
    glm::vec3 topLeft;
    glm::vec3 topRight;
    glm::vec3 bottomLeft;
    glm::vec3 bottomRight;

    for (int i = 0; i < m_param1; i++) {
        float topPhi = i * phi;
        topLeft = glm::vec3(0.5f * glm::sin(topPhi) * glm::sin(currentTheta), 0.5f * glm::cos(topPhi), 0.5f * glm::sin(topPhi) * glm::cos(currentTheta));
        topRight = glm::vec3(0.5f * glm::sin(topPhi) * glm::sin(nextTheta), 0.5f * glm::cos(topPhi), 0.5f * glm::sin(topPhi) * glm::cos(nextTheta));
        bottomLeft = glm::vec3(0.5f * glm::sin(topPhi + phi) * glm::sin(currentTheta), 0.5f * glm::cos(topPhi + phi), 0.5f * glm::sin(topPhi + phi) * glm::cos(currentTheta));
        bottomRight = glm::vec3(0.5f * glm::sin(topPhi + phi) * glm::sin(nextTheta), 0.5f * glm::cos(topPhi + phi), 0.5f * glm::sin(topPhi + phi) * glm::cos(nextTheta));
        makeTile(topLeft, topRight, bottomLeft, bottomRight);
    }
}

void Sphere::makeSphere() {
    // Task 7: create a full sphere uglm::sing the makeWedge() function you
    //         implemented in Task 6
    // Note: think about how param 2 comes into play here!
    float thetaStep = glm::radians(360.f / m_param2);

    for (int i = 0; i < m_param2; i++) {
        float currentTheta = i * thetaStep;
        float nextTheta = (i + 1) * thetaStep;
        makeWedge(currentTheta, nextTheta);
    }
}

void Sphere::setVertexData() {
    // Uncomment these lines to make a wedge for Task 6, then comment them out for Task 7:



    // Uncomment these lines to make sphere for Task 7:

     makeSphere();
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Sphere::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
