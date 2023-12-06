#include "realtime.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobjloader/tiny_obj_loader.h"

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
    glDeleteProgram(m_shader);
    glDeleteBuffers(1, &m_sphere_vbo);
    glDeleteVertexArrays(1, &m_sphere_vao);
    glDeleteBuffers(1, &m_cube_vbo);
    glDeleteVertexArrays(1, &m_cube_vao);
    glDeleteBuffers(1, &m_cone_vbo);
    glDeleteVertexArrays(1, &m_cone_vao);
    glDeleteBuffers(1, &m_cylinder_vbo);
    glDeleteVertexArrays(1, &m_cylinder_vao);
    glDeleteBuffers(1, &m_mesh_vbo);
    glDeleteVertexArrays(1, &m_mesh_vao);

    this->doneCurrent();
}

void Realtime::initializeGL() {
    m_devicePixelRatio = this->devicePixelRatio();

    m_timer = startTimer(1000/60);
    m_elapsedTimer.start();
    m_defaultFBO = 2;
    m_screen_width = size().width() * m_devicePixelRatio;
    m_screen_height = size().height() * m_devicePixelRatio;
    m_fbo_width = m_screen_width;
    m_fbo_height = m_screen_height;

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
    m_texture_shader = ShaderLoader::createShaderProgram(":/resources/shaders/texture.vert", ":/resources/shaders/texture.frag");

    initializeV(m_sphere_vao, m_sphere_vbo, PrimitiveType::PRIMITIVE_SPHERE);
    initializeV(m_cone_vao, m_cone_vbo, PrimitiveType::PRIMITIVE_CONE);
    initializeV(m_cube_vao, m_cube_vbo, PrimitiveType::PRIMITIVE_CUBE);
    initializeV(m_cylinder_vao, m_cylinder_vbo, PrimitiveType::PRIMITIVE_CYLINDER);
    glGenBuffers(1, &m_mesh_vbo);
    glGenVertexArrays(1, &m_mesh_vao);
    \
    glUseProgram(m_texture_shader);
    GLuint texture_location = glGetUniformLocation(m_texture_shader, "texture_id");
    glUniform1i(texture_location, 0);
    glUseProgram(0);

    std::vector<GLfloat> fullscreen_quad_data =
        { //     POSITIONS    //
         // lower triangle
         -1.f ,  1.f, 0.0f,
         0.f ,  1.f,

         -1.f , -1.f, 0.0f,
         0.f ,  0.f,

         1.f , -1.f, 0.0f,
         1.f ,  0.f,

         // upper triangle
         1.f ,  1.f, 0.0f,
         1.f ,  1.f,

         -1.f ,  1.f, 0.0f,
         0.f ,  1.f,

         1.f , -1.f, 0.0f,
         1.f ,  0.f,
         };

    // Generate and bind a VBO and a VAO for a fullscreen quad
    glGenBuffers(1, &m_fullscreen_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_fullscreen_vbo);
    glBufferData(GL_ARRAY_BUFFER, fullscreen_quad_data.size()*sizeof(GLfloat), fullscreen_quad_data.data(), GL_STATIC_DRAW);
    glGenVertexArrays(1, &m_fullscreen_vao);
    glBindVertexArray(m_fullscreen_vao);

    // modify the code below to add a second attribute to the vertex attribute array
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));

    // Unbind the fullscreen quad's VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    makeFBO();
}

void Realtime::paintGL() {
    // Students: anything requiring OpenGL calls every frame should be done here
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_fbo_width, m_fbo_height);

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
            loadOBJ(shape.primitive.meshfile.c_str(), m_data);
            m_meshDataSize = m_data.size();
            bind(m_mesh_vao, m_mesh_vbo, PrimitiveType::PRIMITIVE_MESH);
            glBindVertexArray(m_mesh_vao);
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
            GLuint light_type_location = glGetUniformLocation(m_shader, ("light_type[" + std::to_string(i) + "]").c_str());

            switch(renderData.lights[i].type) {
                case LightType::LIGHT_DIRECTIONAL:
                    glUniform1i(light_type_location, 0);
                    break;
                case LightType::LIGHT_POINT:
                    glUniform1i(light_type_location, 1);
                    break;
                case LightType::LIGHT_SPOT:
                    glUniform1i(light_type_location, 2);
                    break;
            }

            GLuint light_function_location = glGetUniformLocation(m_shader, ("light_function[" + std::to_string(i) + "]").c_str());
            glUniform3fv(light_function_location, 1, &renderData.lights[i].function[0]);
            GLuint light_pos_location = glGetUniformLocation(m_shader, ("light_pos[" + std::to_string(i) + "]").c_str());
            glUniform3fv(light_pos_location, 1, &renderData.lights[i].pos[0]);
            GLuint light_penumbra_location = glGetUniformLocation(m_shader, ("light_penumbra[" + std::to_string(i) + "]").c_str());
            glUniform1f(light_penumbra_location, renderData.lights[i].penumbra);
            GLuint light_angle_location = glGetUniformLocation(m_shader, ("light_angle[" + std::to_string(i) + "]").c_str());
            glUniform1f(light_angle_location, renderData.lights[i].angle);
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
            glDrawArrays(GL_TRIANGLES, 0, m_meshDataSize / 6);
            break;
        }

        // Unbind Vertex Array
        glBindVertexArray(0);
    }

    glUseProgram(0);

    // FBO

    // Bind the default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
    glViewport(0, 0, m_screen_width, m_screen_height);

    // Clear the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Call paintTexture to draw our FBO color attachment texture | Task 31: Set bool parameter to true
    paintTexture(m_fbo_texture, settings.perPixelFilter, settings.kernelBasedFilter, settings.extraCredit1, settings.extraCredit2);
}

void Realtime::resizeGL(int w, int h) {
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here
    m_width = float(w);
    aspectRatio = float(w) / float(h);

    glDeleteTextures(1, &m_fbo_texture);
    glDeleteRenderbuffers(1, &m_fbo_renderbuffer);
    glDeleteFramebuffers(1, &m_fbo);

    m_screen_width = size().width() * m_devicePixelRatio;
    m_screen_height = size().height() * m_devicePixelRatio;
    m_fbo_width = m_screen_width;
    m_fbo_height = m_screen_height;
    // Task 34: Regenerate your FBOs
    makeFBO();
}

void Realtime::setTime() {
    float theta = glm::radians((float(settings.time) - 6.f) * 15.f);
    float alpha = glm::radians(30.f);
    for (int i = 0; i < std::size(renderData.lights); i++){
        if (renderData.lights[i].type == LightType::LIGHT_DIRECTIONAL){
            renderData.lights[i].dir = glm::vec4(-sin(theta) * sin(alpha), -sin(theta) * cos(alpha), cos(theta), 0.f);
        }
    }
}

void Realtime::sceneChanged() {
    renderData.lights.clear();
    bool success = SceneParser::parse(settings.sceneFilePath, renderData);

    if (!success) {
        std::cerr << "Error loading scene: \"" << settings.sceneFilePath << "\"" << std::endl;
    }
    setTime();
    setupViewMatrix();
    setupProjMatrix();

    update(); // asks for a PaintGL() call to occur
}

void Realtime::settingsChanged() {
    m_sphere->updateParams(settings.shapeParameter1, settings.shapeParameter2);
    m_cube->updateParams(settings.shapeParameter1);
    m_cone->updateParams(settings.shapeParameter1, settings.shapeParameter2);
    m_cylinder->updateParams(settings.shapeParameter1, settings.shapeParameter2);

    setTime();
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
// A function to load and parse an OBJ file using rapidobj
bool Realtime::loadOBJ(
    const char * path,
    std::vector < float > & out_array
    ) {
    // clear out_array
    out_array.clear();

    tinyobj::ObjReaderConfig reader_config;
    reader_config.mtl_search_path = ""; // Path to material files

    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(path, reader_config)) {
        if (!reader.Error().empty()) {
            std::cerr << "TinyObjReader: " << reader.Error();
        }
        exit(1);
    }

    if (!reader.Warning().empty()) {
        std::cout << "TinyObjReader: " << reader.Warning();
    }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    auto& materials = reader.GetMaterials();

    // Loop over shapes
    for (size_t s = 0; s < shapes.size(); s++) {
        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++) {
                    // access to vertex
                    tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                    tinyobj::real_t vx = attrib.vertices[3*size_t(idx.vertex_index)+0];
                    tinyobj::real_t vy = attrib.vertices[3*size_t(idx.vertex_index)+1];
                    tinyobj::real_t vz = attrib.vertices[3*size_t(idx.vertex_index)+2];
                    out_array.push_back(vx);
                    out_array.push_back(vy);
                    out_array.push_back(vz);

                    // Check if `normal_index` is zero or positive. negative = no normal data
                    if (idx.normal_index >= 0) {
                        tinyobj::real_t nx = attrib.normals[3*size_t(idx.normal_index)+0];
                        tinyobj::real_t ny = attrib.normals[3*size_t(idx.normal_index)+1];
                        tinyobj::real_t nz = attrib.normals[3*size_t(idx.normal_index)+2];
                        out_array.push_back(nx);
                        out_array.push_back(ny);
                        out_array.push_back(nz);
                    }

//                    // Check if `texcoord_index` is zero or positive. negative = no texcoord data
//                    if (idx.texcoord_index >= 0) {
//                        tinyobj::real_t tx = attrib.texcoords[2*size_t(idx.texcoord_index)+0];
//                        tinyobj::real_t ty = attrib.texcoords[2*size_t(idx.texcoord_index)+1];
//                    }

                    // Optional: vertex colors
                    // tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
                    // tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
                    // tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];
            }
            index_offset += fv;

            // per-face material
            shapes[s].mesh.material_ids[f];
        }
    }
}
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
        float c = cos(float(deltaY) / m_width);
        float s = sin(float(deltaY) / m_width / aspectRatio);
        glm::vec3 u = glm::normalize(glm::cross(glm::vec3(renderData.cameraData.look), glm::vec3(renderData.cameraData.up)));
        glm::mat3 rotateY = glm::mat3(c + pow(u.x, 2) * (1-c), u.x * u.y * (1-c) + u.z * s, u.x * u.z * (1-c) - u.y * s,
                                      u.x * u.y * (1-c) - u.z * s, c + pow(u.y, 2) * (1-c), u.y * u.z * (1-c) + u.x * s,
                                      u.x * u.z * (1-c) + u.y * s, u.y * u.z * (1-c) - u.x * s, c + pow(u.z, 2) * (1-c));
        renderData.cameraData.look =  glm::vec4(rotateY * glm::vec3(renderData.cameraData.look), 0.f);
        c = cos(float(deltaX) / m_width);
        s = sin(float(deltaX) / m_width / aspectRatio);
        glm::mat3 rotateX = glm::mat3(c, 0, -s,
                                      0, 1, 0,
                                      s, 0, c);
        renderData.cameraData.look =  glm::vec4(rotateX * glm::vec3(renderData.cameraData.look), 0.f);

        setupViewMatrix();
        update(); // asks for a PaintGL() call to occur
    }
}

void Realtime::timerEvent(QTimerEvent *event) {
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();

    // Use deltaTime and m_keyMap here to move around
    float move = deltaTime * 3;
    glm::vec4 u = glm::vec4(glm::normalize(glm::cross(glm::vec3(renderData.cameraData.look), glm::vec3(renderData.cameraData.up))), 0);
    if (m_keyMap[Qt::Key_W]) {
        renderData.cameraData.pos += move * renderData.cameraData.look;
    }
    if (m_keyMap[Qt::Key_S]) {
        renderData.cameraData.pos -= move * renderData.cameraData.look;
    }
    if (m_keyMap[Qt::Key_A]) {
        renderData.cameraData.pos -= move * u * 3.f;
    }
    if (m_keyMap[Qt::Key_D]) {
        renderData.cameraData.pos += move * u * 3.f;
    }
    if (m_keyMap[Qt::Key_Space]) {
        renderData.cameraData.pos += move * glm::vec4(0.f, 1.f, 0.f, 0.f);
    }
    if (m_keyMap[Qt::Key_Control]) {
        renderData.cameraData.pos -= move * glm::vec4(0.f, 1.f, 0.f, 0.f);
    }

    setupViewMatrix();
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
