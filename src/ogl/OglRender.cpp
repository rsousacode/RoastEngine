#include "OglRender.h"
#import <GLFW/glfw3.h>
#import "../common/RDDebug.h"
#import<stdexcept>
#include <iostream>
#include "LoadShaders.h"
#include "vgl.h"



const GLuint Triangles_ID = 0;
const GLuint NumVAOs = 3;

const GLuint Buffer_ID = 0;
const GLuint NumBuffer = 3;
const GLuint vPosition = 0;
const GLuint NumVertices = 6;

GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffer];

void OglRender::preSetup() {

}
void OglRender::setupAdapter(const char *wName, int wWidth, int wHeight) {
    static const GLfloat vertices[NumVertices][3] {
            {-.90, -.90, .5},
            {.85, -.90, 0.5},
            {-.90, -.85, .5},
            {.90, .90, .5},
            {-.85, .90, .5},

    };

// glGen*, glBind*
    // create vaos and bind selected vaos to context
    // return NumVAOS unused names for use as vaos in VAOs array
    glGenVertexArrays(NumVAOs, VAOs);
    // attach this vao to opengl context before use
    glBindVertexArray(VAOs[Triangles_ID]);
    GLboolean isVertexArray = glIsVertexArray(VAOs[Triangles_ID]);
    std::string output = isVertexArray == GL_TRUE? "yes" : "no";
    std::cout << "is vertex array? " << output << std::endl;

    // create names of vertex-buffer objects
    glGenBuffers(NumBuffer, Buffers);
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[Buffer_ID]);
    // loading data(vertices) into buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // init shaders
    ShaderInfo  shaders[] =
            {
                    {GL_VERTEX_SHADER, "triangles.vert"},
                    {GL_FRAGMENT_SHADER, "triangles.frag"},
                    {GL_NONE, NULL}
            };

    GLuint program = LoadShaders(shaders);
    glUseProgram(program);

    // put buffer into VAO attributes position
    glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(vPosition);

}

void draw() {
    static const float black[] = {0.0f, 0.0f, 0.0f, 0.0f};
    glClearBufferfv(GL_COLOR, 0, black);
    glBindVertexArray(VAOs[Triangles_ID]);
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);
}

void OglRender::draw() {
    static const float black[] = {0.0f, 0.0f, 0.0f, 0.0f};
    glClearBufferfv(GL_COLOR, 0, black);
    glBindVertexArray(VAOs[Triangles_ID]);
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);
}
