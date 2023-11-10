#include "sceneparser.h"
#include "scenefilereader.h"
#include <glm/gtx/transform.hpp>
#include "imagereader.h"

#include <chrono>
#include <iostream>

void dfsAddRenderData(SceneNode* curNode, std::vector<RenderShapeData>* shapeList, std::vector<SceneLightData>* lightList, glm::mat4 curData) {

    glm::mat4 curMat = glm::mat4(1);

    for (int i = 0; i < size(curNode->transformations); i++) {
        SceneTransformation* t = curNode->transformations[i];
        switch(t->type) {
        case TransformationType::TRANSFORMATION_TRANSLATE:
            curMat = glm::translate(curMat, t->translate);
            break;

        case TransformationType::TRANSFORMATION_SCALE:
            curMat = glm::scale(curMat, t->scale);
            break;

        case TransformationType::TRANSFORMATION_ROTATE:
            curMat = glm::rotate(curMat, t->angle, t->rotate);
            break;

        case TransformationType::TRANSFORMATION_MATRIX:
            curMat = t->matrix * curMat;
            break;
        }
    }

    curData = curData * curMat;

    for (int i = 0; i < size(curNode->primitives); i++) {
        RenderShapeData new_shape;
        new_shape.ctm = curData;
        new_shape.primitive = *curNode->primitives[i];
        shapeList->push_back(new_shape);
    }

    for (int i = 0; i < size(curNode->lights); i++) {
        SceneLight* l = curNode->lights[i];
        SceneLightData new_light;
        new_light.id = l->id;
        new_light.type = l->type;
        new_light.color = l->color;
        new_light.function = l->function;
        new_light.dir = curData * l->dir;
        new_light.pos = curData * glm::vec4(0, 0, 0, 1);
        new_light.penumbra = l->penumbra;
        new_light.angle = l->angle;
        lightList->push_back(new_light);
    }

    for (int i = 0; i < size(curNode->children); i++) {
        dfsAddRenderData(curNode->children[i], shapeList, lightList, curData);
    }
}


bool SceneParser::parse(std::string filepath, RenderData &renderData) {
    ScenefileReader fileReader = ScenefileReader(filepath);
    bool success = fileReader.readJSON();
    if (!success) {
        return false;
    }

    // populate renderData with global data, and camera data;
    renderData.globalData = fileReader.getGlobalData();
    renderData.cameraData = fileReader.getCameraData();

    // populate renderData's list of primitives and their transforms.
    renderData.shapes.clear();
    SceneNode* rootNode = fileReader.getRootNode();
    glm::mat4 curData = glm::mat4(1);
    dfsAddRenderData(rootNode, &renderData.shapes, &renderData.lights, curData);

    for (RenderShapeData &node : renderData.shapes) {
        node.ctm_inverse = glm::inverse(node.ctm);
        if (node.primitive.material.textureMap.isUsed) {
            ImageReader texture {QString::fromStdString(node.primitive.material.textureMap.filename)};
            node.m_height = texture.m_height;
            node.m_width = texture.m_width;
            node.m_data = texture.m_data;
        }
    }

    return true;
}
