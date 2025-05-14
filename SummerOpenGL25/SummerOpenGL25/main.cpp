#include <glad/glad.h>
#include <GLFW/glfw3.h>
//#include "linmath.h"
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> 
// glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <vector>
#include <iostream>

struct sVertex
{
    float x;
    float y;
    float z;
    float r;
    float g;
    float b;
};

sVertex vertices[3] =
{
    { 3.f, -0.4f, 0.0f, 1.f, 0.f, 0.f },
    {  0.6f, -0.4f, 0.0f, 0.f, 1.f, 0.f },
    {   0.f,  0.6f, 0.0f, 0.f, 0.f, 1.f }
};

sVertex* pVerticies = NULL;

unsigned int g_NumVerticiesToDraw = 0;
unsigned int g_SizeOfVertexArrayInBytes = 0;

//static const struct
//{
//    float x, y;
//    float r, g, b;
//} vertices[3] =
//{
//    { -0.6f, -0.4f, 1.f, 0.f, 0.f },
//    {  0.6f, -0.4f, 0.f, 1.f, 0.f },
//    {   0.f,  0.6f, 0.f, 0.f, 1.f }
//};

static const char* vertex_shader_text =
"#version 110\n"
"uniform mat4 MVP;\n"
"attribute vec3 vCol;\n"
"attribute vec3 vPos;\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 1.0);\n"
"    color = vCol;\n"
"}\n";

static const char* fragment_shader_text =
"#version 110\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_FragColor = vec4(color, 1.0);\n"
"}\n";

bool LoadPLY_XYZ_Format(std::string filename)
{
    struct sPlyVertexXYZ
    {
        float x;
        float y;
        float z;
    };

    struct sPlyTriangle
    {
        int numVerticies;
        int v0;
        int v1;
        int v2;
    };

    std::vector<sPlyVertexXYZ> vecVerticies;
    std::vector<sPlyTriangle> vecTriangles;

    std::ifstream theFile(filename.c_str());

    if (!theFile.is_open())
    {
        return false;
    }

    std::string curWord;

    while (theFile >> curWord)
    {
        if (curWord == "vertex")
        {
            break;
        }
    }

    unsigned int numVerticies = 0;

    theFile >> numVerticies;

    while (theFile >> curWord)
    {
        if (curWord == "face")
        {
            break;
        }
    }

    unsigned int numTriangles = 0;

    theFile >> numTriangles;

    while (theFile >> curWord)
    {
        if (curWord == "end_header")
        {
            break;
        }
    }

    // start reading the verticies...

    for (unsigned int count = 0; count < numVerticies; count++)
    {
        sPlyVertexXYZ curVert;
        theFile >> curVert.x >> curVert.y >> curVert.z;
        vecVerticies.push_back(curVert);
    }

    for (unsigned int count = 0; count < numTriangles; count++)
    {
        sPlyTriangle curTri;
        theFile >> curTri.numVerticies >> curTri.v0 >> curTri.v1 >> curTri.v2;
        vecTriangles.push_back(curTri);
    }

    pVerticies = new sVertex[numVerticies];
    for (unsigned int index = 0; index != numVerticies; index++)
    {
        pVerticies[index].x = vecVerticies[index].x;
        pVerticies[index].y = vecVerticies[index].y;
        pVerticies[index].z = vecVerticies[index].z;

        // colours 
        pVerticies[index].r = 0.0f;
        pVerticies[index].g = 1.0f;
        pVerticies[index].b = 0.0f;
    }

    ::g_NumVerticiesToDraw = numVerticies;
    ::g_SizeOfVertexArrayInBytes = numVerticies * sizeof(sVertex);

    theFile.close();

    return true;
}

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main(void)
{
    GLFWwindow* window;
    GLuint vertex_buffer, vertex_shader, fragment_shader, program;
    GLint mvp_location, vpos_location, vcol_location;

    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
    {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    bool loaded = LoadPLY_XYZ_Format("assets/models/cow.ply");

    if (loaded)
    {
        std::cout << "cow is loaded" << std::endl;
    }
    else
    {
        std::cout << "ALL HOPE IS LOST!!!" << std::endl;
        return -1;
    }

    // NOTE: OpenGL error checks have been omitted for brevity
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, g_SizeOfVertexArrayInBytes, pVerticies, GL_STATIC_DRAW);

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);
    program = glCreateProgram();

    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    mvp_location = glGetUniformLocation(program, "MVP");
    vpos_location = glGetAttribLocation(program, "vPos");
    vcol_location = glGetAttribLocation(program, "vCol");

    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE,
        sizeof(vertices[0]), (void*)0);
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
        sizeof(vertices[0]), (void*)(sizeof(float) * 3));
    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;
        //       mat4x4 m, p, mvp;
        glm::mat4 m, p, v, mvp;
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float)height;
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        //         mat4x4_identity(m);
        m = glm::mat4(1.0f);

        //mat4x4_rotate_Z(m, m, (float) glfwGetTime());
        glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f),
            (float)glfwGetTime(),
            glm::vec3(0.0f, 0.0, 1.0f));

        //m = m * rotateZ;

        //mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        p = glm::perspective(0.6f,
            ratio,
            0.1f,
            1000.0f);

        v = glm::mat4(1.0f);

        glm::vec3 cameraEye = glm::vec3(0.0, 0.0, -30.0f);
        glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);

        v = glm::lookAt(cameraEye,
            cameraTarget,
            upVector);

        //mat4x4_mul(mvp, p, m);
        mvp = p * v * m;


        glUseProgram(program);


        //glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp));

        glDrawArrays(GL_TRIANGLES, 0, g_NumVerticiesToDraw);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
