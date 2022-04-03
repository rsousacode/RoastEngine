#include "OglRender.h"
#include <iostream>
#ifdef __APPLE__
/* Defined before OpenGL and GLUT includes to avoid deprecation messages */
#define GL_SILENCE_DEPRECATION
#include "glad/glad.h"
#endif
#include <GLFW/glfw3.h>
#include<stdexcept>
#include "LoadShaders.h"


const GLuint Triangles_ID = 0;
const GLuint NumVAOs = 3;
const GLuint Buffer_ID = 0;
const GLuint NumBuffer = 3;
const GLuint vPosition = 0;
const GLuint NumVertices = 6;


GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffer];


static const GLfloat vertices[NumVertices][3] = {
        {-0.9, -0.9, 0.5}, {0.85, -0.9, 0.5}, {-0.9, 0.85, 0.5},
        {0.9, -0.85, 0.5}, {0.9, 0.9, 0.5}, {-0.85, 0.9, 0.5}
};


/*!
	Called once to prepare the OPENGL Context.
	@param pInfo  Window Create Information.
	@param createInfo  Render Create Information.
    @param keyCb  GLFW Input callback.
*/

void
OglRender::prepare(const RDWindowCreateInfo &pInfo, const RDCreateInfo &createInfo, GLFWkeyfun keyCb) {
    std::cout << "Starting GLFW context, OpenGL 4.1" << std::endl;
    // Init GLFW
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a GLFWwindow object that we can use for GLFW's functions
    pGlfwWindow = glfwCreateWindow( createInfo.windowWidth, createInfo.windowHeight, createInfo.windowTitle, NULL, NULL);
    glfwMakeContextCurrent(pGlfwWindow);
    if (pGlfwWindow == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }

    // Set the required callback functions
    glfwSetKeyCallback(pGlfwWindow, keyCb);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        throw std::runtime_error("Failed to ini opengl");
    }

    // Define the viewport dimensions
    glViewport(0, 0, createInfo.windowWidth, createInfo.windowHeight);
}


/*!
	Feed pipeline with display data.
*/

void OglRender::display(Vector4 &color) {
    static const float c[] = {color.x, color.y, color.z, color.w};
    // Clears the specified buffer within the current display framebuffer to the specified clear values
    // The buffer argument indicates the buffer to clear
    // Can be GL_COLOR, GL_DEPTH, GL_STENCIL
    // When Buffer is GL_DEPTH or GL_STENCIL, drawbuffer must be zero, otherwise
    // it is the index of the color attachment to be cleared
    glClearBufferfv(GL_COLOR, 0, c);

    // Select the vertex we want to use as vertex data
    glBindVertexArray(VAOs[Triangles_ID]);

    // Virtually sends vertex data to the GL pipeline
    // Constructs a sequence of geometric primitives
    // using data from the currently bound vertex array
    // starting at first ending at the count - 1
    // the first argument (mode)
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);
}

void OglRender::bindShaderStep() {
    // glGen*, glBind*
    // create vaos and bind selected vaos to context
    // return NumVAOS unused names for use as vaos in VAOs array
    glGenVertexArrays(NumVAOs, VAOs);
    // attach this vao to opengl context before use
    glBindVertexArray(VAOs[Triangles_ID]);
    GLboolean isVertexArray = glIsVertexArray(VAOs[Triangles_ID]);

    if(isVertexArray != GL_TRUE) {
        throw std::runtime_error("invalid vertex array");
    }

    glGenBuffers(NumBuffer, Buffers);

    // The glBindBuffer target specficies the current active buffer object
    // Target must be set to one of GL_ARRAY_BUFFER, GL_ATOMIC_COUNTER_BUFFER,
    // GL_ELEMENT_ARRAY_BUFFER, GL_PIXEL_PACK_BUFFER, GL_PIXEL_UNPACKBUFFER, GL_COPY_READ_BUFFER,
    // GL_COPY_WRITE_BUFFER, GL_SHADER_STORAGE_BUFFER, GL_QUERY_RESULT_BUFFER, GL_DRAW_INDIRECT_BUFFER,
    // GL_TRANSFORM_FEEDBACK_BUFFER, GL_UNIFORM_BUFFER
    // Buffer specifies object to be bound to
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[Buffer_ID]);
    // loading data(vertices) into buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // TODO Define proper location
    ShaderInfo  shaders[] =
            {
                    {GL_VERTEX_SHADER, "../src/ogl/shaders/triangles.vert"},
                    {GL_FRAGMENT_SHADER, "../src/ogl/shaders/triangles.frag"},
                    {GL_NONE, NULL}
            };

    GLuint program = LoadShaders(shaders);
    glUseProgram(program);

    // put buffer into VAO attributes position
    glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(vPosition);
}
