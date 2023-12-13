#include "realtime.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void Realtime::paintTexture(GLuint texture, bool perPixel, bool kernelBased, bool grayScale, bool sharphen) {
    glUseProgram(m_texture_shader);

    GLuint perPixel_location = glGetUniformLocation(m_texture_shader, "perPixel");
    glUniform1i(perPixel_location, perPixel);
    GLuint kernelBased_location = glGetUniformLocation(m_texture_shader, "kernelBased");
    glUniform1i(kernelBased_location, kernelBased);
    GLuint grayScale_location = glGetUniformLocation(m_texture_shader, "grayScale");
    glUniform1i(grayScale_location, grayScale);
    GLuint sharphen_location = glGetUniformLocation(m_texture_shader, "sharphen");
    glUniform1i(sharphen_location, sharphen);

    glBindVertexArray(m_fullscreen_vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void Realtime::makeFBO(){
    // Generate and bind an empty texture, set its min/mag filter interpolation, then unbind
    glGenTextures(1, &m_fbo_texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_fbo_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_fbo_width , m_fbo_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Generate and bind a renderbuffer of the right size, set its format, then unbind
    glGenRenderbuffers(1, &m_fbo_renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_fbo_renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_fbo_width, m_fbo_height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Generate and bind an FBO
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // Add our texture as a color attachment, and our renderbuffer as a depth+stencil attachment, to our FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fbo_texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_fbo_renderbuffer);

    // Unbind the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
}

void Realtime::makeDepthMap(){
    // Making the binding on the depthMap
     glGenFramebuffers(1, &depthMapFBO);
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_shadow_width, m_shadow_height, 0,
                 GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("There is an issue in FBO DepthMap");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
}
