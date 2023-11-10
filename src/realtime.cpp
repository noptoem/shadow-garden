#include "realtime.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include "settings.h"
#include "utils/sceneparser.h"
#include "utils/shaderloader.h"

// ================== Project 5: Lights, Camera

Realtime::Realtime(QWidget *parent)
    : QOpenGLWidget(parent)
{
    m_prev_mouse_pos = glm::vec2(size().width()/2, size().height()/2);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    m_keyMap[Qt::Key_W]       = false;
    m_keyMap[Qt::Key_A]       = false;
    m_keyMap[Qt::Key_S]       = false;
    m_keyMap[Qt::Key_D]       = false;
    m_keyMap[Qt::Key_Control] = false;
    m_keyMap[Qt::Key_Space]   = false;

    // If you must use this function, do not edit anything above this

    // initilize shapes
    m_cube = new Cube();
    m_cube->updateParams(settings.shapeParameter1);
    m_sphere = new Sphere();
    m_sphere->updateParams(settings.shapeParameter1, settings.shapeParameter2);
    m_cylinder = new Cylinder();
    m_cylinder->updateParams(settings.shapeParameter1, settings.shapeParameter2);
    m_cone = new Cone();
    m_cone->updateParams(settings.shapeParameter1, settings.shapeParameter2);
}

void Realtime::finish() {
    killTimer(m_timer);
    this->makeCurrent();

    // Students: anything requiring OpenGL calls when the program exits should be done here

    this->doneCurrent();
}

void Realtime::initializeGL() {
    m_devicePixelRatio = this->devicePixelRatio();

    m_timer = startTimer(1000/60);
    m_elapsedTimer.start();

    // Initializing GL.
    // GLEW (GL Extension Wrangler) provides access to OpenGL functions.
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error while initializing GL: " << glewGetErrorString(err) << std::endl;
    }
    std::cout << "Initialized GL: Version " << glewGetString(GLEW_VERSION) << std::endl;

    // Allows OpenGL to draw objects appropriately on top of one another
    glEnable(GL_DEPTH_TEST);
    // Tells OpenGL to only draw the front face
    glEnable(GL_CULL_FACE);
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here

    m_shader = ShaderLoader::createShaderProgram(":/resources/shaders/default.vert", ":/resources/shaders/default.frag");

    initializeV(m_sphere_vao, m_sphere_vbo, PrimitiveType::PRIMITIVE_SPHERE);
    initializeV(m_cone_vao, m_cone_vbo, PrimitiveType::PRIMITIVE_CONE);
    initializeV(m_cube_vao, m_cube_vbo, PrimitiveType::PRIMITIVE_CUBE);
    initializeV(m_cylinder_vao, m_cylinder_vbo, PrimitiveType::PRIMITIVE_CYLINDER);
}

void Realtime::paintGL() {
    // Students: anything requiring OpenGL calls every frame should be done here
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    bind(m_sphere_vao, m_sphere_vbo, PrimitiveType::PRIMITIVE_SPHERE);
    bind(m_cone_vao, m_cone_vbo, PrimitiveType::PRIMITIVE_CONE);
    bind(m_cube_vao, m_cube_vbo, PrimitiveType::PRIMITIVE_CUBE);
    bind(m_cylinder_vao, m_cylinder_vbo, PrimitiveType::PRIMITIVE_CYLINDER);

    // use the shader
    glUseProgram(m_shader);

    // loop through the shapes
    for (RenderShapeData& shape: renderData.shapes) {
        // bind correct vao
        switch(shape.primitive.type) {
        case PrimitiveType::PRIMITIVE_CONE:
            glBindVertexArray(m_cone_vao);
            break;
        case PrimitiveType::PRIMITIVE_CYLINDER:
            glBindVertexArray(m_cylinder_vao);
            break;
        case PrimitiveType::PRIMITIVE_SPHERE:
            glBindVertexArray(m_sphere_vao);
            break;
        case PrimitiveType::PRIMITIVE_CUBE:
            glBindVertexArray(m_cube_vao);
            break;
        case PrimitiveType::PRIMITIVE_MESH:
            //NYI
            break;
        }

        // pass in m_model as a uniform into the shader program
        GLuint model_location = glGetUniformLocation(m_shader, "model_matrix");
        GLuint model_ti_location = glGetUniformLocation(m_shader, "model_matrix_transpose_inverse");

        glm::mat3 model_ti = glm::transpose(shape.ctm_inverse);

        glUniformMatrix4fv(model_location, 1, GL_FALSE, &shape.ctm[0][0]);
        glUniformMatrix3fv(model_ti_location, 1, GL_FALSE, &model_ti[0][0]);

        // pass in m_view and m_proj
        GLuint view_location = glGetUniformLocation(m_shader, "view_matrix");
        GLuint proj_location = glGetUniformLocation(m_shader, "proj_matrix");

        glUniformMatrix4fv(view_location, 1, GL_FALSE, &m_view[0][0]);
        glUniformMatrix4fv(proj_location, 1, GL_FALSE, &m_proj[0][0]);

        // ambient
        GLuint ka_location = glGetUniformLocation(m_shader, "ka");
        glUniform1f(ka_location, renderData.globalData.ka);
        GLuint cAmbient_location = glGetUniformLocation(m_shader, "cAmbient");
        glUniform4fv(cAmbient_location, 1, &shape.primitive.material.cAmbient[0]);

        // light
        GLuint light_num_location = glGetUniformLocation(m_shader, "light_num");
        glUniform1i(light_num_location, renderData.lights.size());
        for (int i = 0; i < renderData.lights.size(); i++) {
            GLuint light_dir_location = glGetUniformLocation(m_shader, ("light_dir[" + std::to_string(i) + "]").c_str());
            glUniform3fv(light_dir_location, 1, &renderData.lights[i].dir[0]);
            GLuint light_color_location = glGetUniformLocation(m_shader, ("light_color[" + std::to_string(i) + "]").c_str());
            glUniform3fv(light_color_location, 1, &renderData.lights[i].color[0]);
        }

        // diffuse
        GLuint kd_location = glGetUniformLocation(m_shader, "kd");
        glUniform1f(kd_location, renderData.globalData.kd);
        GLuint cDiffuse_location = glGetUniformLocation(m_shader, "cDiffuse");
        glUniform4fv(cDiffuse_location, 1, &shape.primitive.material.cDiffuse[0]);

        // specular
        GLuint ks_location = glGetUniformLocation(m_shader, "ks");
        glUniform1f(ks_location, renderData.globalData.ks);
        GLuint shininess_location = glGetUniformLocation(m_shader, "shininess");
        glUniform1f(shininess_location, shape.primitive.material.shininess);
        GLuint cSpecular_location = glGetUniformLocation(m_shader, "cSpecular");
        glUniform4fv(cSpecular_location, 1, &shape.primitive.material.cSpecular[0]);

        // camera pos
        glm::vec4 camera_pos = m_view_inverse * glm::vec4(glm::vec3(0.f), 1.f);
        GLuint camera_pos_location = glGetUniformLocation(m_shader, "camera_pos");
        glUniform4fv(camera_pos_location, 1, &camera_pos[0]);

        // Draw Command
        switch(shape.primitive.type) {
        case PrimitiveType::PRIMITIVE_CONE:
            glDrawArrays(GL_TRIANGLES, 0, m_coneDataSize / 6);
            break;
        case PrimitiveType::PRIMITIVE_CYLINDER:
            glDrawArrays(GL_TRIANGLES, 0, m_cylinderDataSize / 6);
            break;
        case PrimitiveType::PRIMITIVE_SPHERE:
            glDrawArrays(GL_TRIANGLES, 0, m_sphereDataSize / 6);
            break;
        case PrimitiveType::PRIMITIVE_CUBE:
            glDrawArrays(GL_TRIANGLES, 0, m_cubeDataSize / 6);
            break;
        case PrimitiveType::PRIMITIVE_MESH:
            //NYI
            break;
        }

        // Unbind Vertex Array
        glBindVertexArray(0);
    }

    glUseProgram(0);
}

void Realtime::resizeGL(int w, int h) {
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here
    aspectRatio = float(w) / float(h);
}

void Realtime::sceneChanged() {
    renderData.lights.clear();
    bool success = SceneParser::parse(settings.sceneFilePath, renderData);

    if (!success) {
        std::cerr << "Error loading scene: \"" << settings.sceneFilePath << "\"" << std::endl;
    }

    setupViewMatrix();
    setupProjMatrix();

    update(); // asks for a PaintGL() call to occur
}

void Realtime::settingsChanged() {
    m_sphere->updateParams(settings.shapeParameter1, settings.shapeParameter2);
    m_cube->updateParams(settings.shapeParameter1);
    m_cone->updateParams(settings.shapeParameter1, settings.shapeParameter2);
    m_cylinder->updateParams(settings.shapeParameter1, settings.shapeParameter2);

    setupViewMatrix();
    setupProjMatrix();

    update(); // asks for a PaintGL() call to occur
}

void Realtime::initializeV(GLuint& m_vao, GLuint& m_vbo, PrimitiveType type) {
    // Generate and bind VBO
    glGenBuffers(1, &m_vbo);
    glGenVertexArrays(1, &m_vao);

    bind(m_vao, m_vbo, type);
}

void Realtime::bind(GLuint& m_vao, GLuint& m_vbo, PrimitiveType type) {
    // Generate data
    switch(type) {
    case PrimitiveType::PRIMITIVE_CONE:
        m_data = m_cone->generateShape();
        m_coneDataSize = m_data.size();
        break;
    case PrimitiveType::PRIMITIVE_CYLINDER:
        m_data = m_cylinder->generateShape();
        m_cylinderDataSize = m_data.size();
        break;
    case PrimitiveType::PRIMITIVE_SPHERE:
        m_data = m_sphere->generateShape();
        m_sphereDataSize = m_data.size();
        break;
    case PrimitiveType::PRIMITIVE_CUBE:
        m_data = m_cube->generateShape();
        m_cubeDataSize = m_data.size();
        break;
    case PrimitiveType::PRIMITIVE_MESH:
        //NYI
        break;
    }

    // Send data to VBO
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER,m_data.size() * sizeof(GLfloat),m_data.data(), GL_STATIC_DRAW);

    glBindVertexArray(m_vao);

    // Enable and define attribute 0 to store vertex positions
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));

    // Clean-up bindings
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
}
// ================== Project 6: Action!

void Realtime::keyPressEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = true;
}

void Realtime::keyReleaseEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = false;
}

void Realtime::mousePressEvent(QMouseEvent *event) {
    if (event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = true;
        m_prev_mouse_pos = glm::vec2(event->position().x(), event->position().y());
    }
}

void Realtime::mouseReleaseEvent(QMouseEvent *event) {
    if (!event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = false;
    }
}

void Realtime::mouseMoveEvent(QMouseEvent *event) {
    if (m_mouseDown) {
        int posX = event->position().x();
        int posY = event->position().y();
        int deltaX = posX - m_prev_mouse_pos.x;
        int deltaY = posY - m_prev_mouse_pos.y;
        m_prev_mouse_pos = glm::vec2(posX, posY);

        // Use deltaX and deltaY here to rotate

        update(); // asks for a PaintGL() call to occur
    }
}

void Realtime::timerEvent(QTimerEvent *event) {
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();

    // Use deltaTime and m_keyMap here to move around

    update(); // asks for a PaintGL() call to occur
}

// DO NOT EDIT
void Realtime::saveViewportImage(std::string filePath) {
    // Make sure we have the right context and everything has been drawn
    makeCurrent();

    int fixedWidth = 1024;
    int fixedHeight = 768;

    // Create Frame Buffer
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Create a color attachment texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fixedWidth, fixedHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    // Optional: Create a depth buffer if your rendering uses depth testing
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, fixedWidth, fixedHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return;
    }

    // Render to the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, fixedWidth, fixedHeight);

    // Clear and render your scene here
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    paintGL();

    // Read pixels from framebuffer
    std::vector<unsigned char> pixels(fixedWidth * fixedHeight * 3);
    glReadPixels(0, 0, fixedWidth, fixedHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    // Unbind the framebuffer to return to default rendering to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Convert to QImage
    QImage image(pixels.data(), fixedWidth, fixedHeight, QImage::Format_RGB888);
    QImage flippedImage = image.mirrored(); // Flip the image vertically

    // Save to file using Qt
    QString qFilePath = QString::fromStdString(filePath);
    if (!flippedImage.save(qFilePath)) {
        std::cerr << "Failed to save image to " << filePath << std::endl;
    }

    // Clean up
    glDeleteTextures(1, &texture);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteFramebuffers(1, &fbo);
}
