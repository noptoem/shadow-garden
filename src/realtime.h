#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
#include "utils/sceneparser.h"
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <unordered_map>
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QTime>
#include <QTimer>
#include "shapes/Cube.h"
#include "shapes/Sphere.h"
#include "shapes/Cylinder.h"
#include "shapes/Cone.h"


class Realtime : public QOpenGLWidget
{
public:
    Realtime(QWidget *parent = nullptr);
    void finish();                                      // Called on program exit
    void sceneChanged();
    void settingsChanged();
    void saveViewportImage(std::string filePath);

public slots:
    void tick(QTimerEvent* event);                      // Called once per tick of m_timer

protected:
    void initializeGL() override;                       // Called once at the start of the program
    void paintGL() override;                            // Called whenever the OpenGL context changes or by an update() request
    void resizeGL(int width, int height) override;      // Called when window size changes

private:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void timerEvent(QTimerEvent *event) override;
    void initializeV(GLuint& m_vao, GLuint& m_vbo, PrimitiveType type);
    void bind(GLuint& m_vao, GLuint& m_vbo, PrimitiveType type);
    void setupViewMatrix();
    void setupProjMatrix();
    void setTime();
    void paintTexture(GLuint texture, bool perPixel, bool kernelBased, bool grayScale, bool sharphen);
    void makeFBO();
    void makeDepthMap();
    void renderScene();
    glm::mat4 setupLightViewMatrix();
    glm::mat4 setupLightProjMatrix();
    void paintShadow(float near_plane, float far_plane);
    bool loadOBJ(
        const char * path,
        std::vector < float > & out_array);

    // Tick Related Variables
    int m_timer;                                        // Stores timer which attempts to run ~60 times per second
    QElapsedTimer m_elapsedTimer;                       // Stores timer which keeps track of actual time between frames

    // Input Related Variables
    bool m_mouseDown = false;                           // Stores state of left mouse button
    glm::vec2 m_prev_mouse_pos;                         // Stores mouse position
    std::unordered_map<Qt::Key, bool> m_keyMap;         // Stores whether keys are pressed or not

    // Device Correction Variables
    int m_devicePixelRatio;

    // current shape data
    std::vector<float> m_data;

    // view and project
    glm::mat4 m_view;
    glm::mat4 m_view_inverse;
    glm::mat4 m_proj;
    float m_width;
    float aspectRatio;

    // shader ID
    GLuint m_shader;

    // RenderData
    RenderData renderData;

    // sphere
    GLuint m_sphere_vbo; // Stores id of vbo
    GLuint m_sphere_vao; // Stores id of vao
    int m_sphereDataSize;
    Sphere* m_sphere;

    // cube
    GLuint m_cube_vbo; // Stores id of vbo
    GLuint m_cube_vao; // Stores id of vao
    int m_cubeDataSize;
    Cube* m_cube;

    // cone
    GLuint m_cone_vbo; // Stores id of vbo
    GLuint m_cone_vao; // Stores id of vao
    int m_coneDataSize;
    Cone* m_cone;

    // cylinder
    GLuint m_cylinder_vbo; // Stores id of vbo
    GLuint m_cylinder_vao; // Stores id of vao
    int m_cylinderDataSize;
    Cylinder* m_cylinder;

    // cone
    GLuint m_mesh_vbo; // Stores id of vbo
    GLuint m_mesh_vao; // Stores id of vao
    int m_meshDataSize;

    // FBO
    GLuint m_fbo;
    GLuint m_fbo_texture;
    GLuint m_fbo_renderbuffer;
    GLuint m_texture_shader;
    GLuint m_debug_shader;
    GLuint m_shadow_shader;
    GLuint m_fullscreen_vbo;
    GLuint m_fullscreen_vao;
    GLuint m_defaultFBO;
    int m_fbo_width;
    int m_fbo_height;
    int m_screen_width;
    int m_screen_height;

    // Shadow
    const unsigned int m_shadow_width = 1024;
    const unsigned int m_shadow_height = 1024;
    GLuint depthMapFBO;
    GLuint depthMap;
};

