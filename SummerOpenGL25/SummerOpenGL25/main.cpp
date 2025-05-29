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
#include <sstream>
#include <vector>
#include <iostream>

#include "cShaderManager/cShaderManager.h"
#include "cVAOManager/cVAOManager.h"
#include "cMeshObject.h"

struct sVertex
{
    float x;
    float y;
    float z;
    float r;
    float g;
    float b;
};

sVertex* pVerticies = NULL;
cShaderManager* g_pTheShaderManager = NULL;
cVAOManager* g_pMeshManager = NULL;

std::vector<cMeshObject*> g_pMeshesToDraw;

unsigned int g_NumVerticiesToDraw = 0;
unsigned int g_SizeOfVertexArrayInBytes = 0;
glm::vec3 g_cameraEye = glm::vec3(0.0, 0.0, -30.0f);

//static const char* vertex_shader_text =
//"#version 110\n"
//"uniform mat4 MVP;\n"
//"attribute vec3 vCol;\n"
//"attribute vec3 vPos;\n"
//"varying vec3 color;\n"
//"void main()\n"
//"{\n"
//"    gl_Position = MVP * vec4(vPos, 1.0);\n"
//"    color = vCol;\n"
//"}\n";
//
//static const char* fragment_shader_text =
//"#version 110\n"
//"varying vec3 color;\n"
//"void main()\n"
//"{\n"
//"    gl_FragColor = vec4(color, 1.0);\n"
//"}\n";

bool LoadPLY_XYZ_Format_With_Indicies(std::string filename)
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

    int numVerticiesToDraw = numTriangles * 3;
    pVerticies = new sVertex[numVerticiesToDraw];

    unsigned int vertIndex = 0;

    for (unsigned int index = 0; index != numTriangles; index++)
    {
        sPlyTriangle curTri = vecTriangles[index];

        sPlyVertexXYZ v0 = vecVerticies[curTri.v0];
        sPlyVertexXYZ v1 = vecVerticies[curTri.v1];
        sPlyVertexXYZ v2 = vecVerticies[curTri.v2];


        pVerticies[vertIndex].x = v0.x;
        pVerticies[vertIndex].y = v0.y;
        pVerticies[vertIndex].z = v0.z;
        // colours 
        pVerticies[vertIndex].r = 0.0f;
        pVerticies[vertIndex].g = 1.0f;
        pVerticies[vertIndex].b = 0.0f;

        pVerticies[vertIndex + 1].x = v1.x;
        pVerticies[vertIndex + 1].y = v1.y;
        pVerticies[vertIndex + 1].z = v1.z;
        // colours 
        pVerticies[vertIndex + 1].r = 0.0f;
        pVerticies[vertIndex + 1].g = 1.0f;
        pVerticies[vertIndex + 1].b = 0.0f;

        pVerticies[vertIndex + 2].x = v2.x;
        pVerticies[vertIndex + 2].y = v2.y;
        pVerticies[vertIndex + 2].z = v2.z;
        // colours 
        pVerticies[vertIndex + 2].r = 0.0f;
        pVerticies[vertIndex + 2].g = 1.0f;
        pVerticies[vertIndex + 2].b = 0.0f;

        vertIndex += 3;
    }

    ::g_NumVerticiesToDraw = numVerticiesToDraw;
    ::g_SizeOfVertexArrayInBytes = numVerticiesToDraw * sizeof(sVertex);

    theFile.close();

    return true;
}

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

    float scale = 0.1f;


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

    const float camera_speed = 0.5f;

    if (key == GLFW_KEY_A)
    {
        ::g_cameraEye.x += camera_speed;
    }

    if (key == GLFW_KEY_D)
    {
        ::g_cameraEye.x -= camera_speed;
    }

    if (key == GLFW_KEY_W)
    {
        ::g_cameraEye.z += camera_speed;
    }

    if (key == GLFW_KEY_S)
    {
        ::g_cameraEye.z -= camera_speed;
    }

    if (key == GLFW_KEY_Q)
    {
        ::g_cameraEye.y += camera_speed;
    }

    if (key == GLFW_KEY_E)
    {
        ::g_cameraEye.y -= camera_speed;
    }
}

int main(void)
{
    GLFWwindow* window;
    GLuint vertex_buffer;
    //GLuint vertex_shader;
    //GLuint fragment_shader;
    GLuint program;
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

    /*bool loaded = LoadPLY_XYZ_Format_With_Indicies("assets/models/cow.ply");

    if (loaded)
    {
        std::cout << "cow is loaded" << std::endl;
    }
    else
    {
        std::cout << "ALL HOPE IS LOST!!!" << std::endl;
        return -1;
    }*/

  

    // NOTE: OpenGL error checks have been omitted for brevity
    /*glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, g_SizeOfVertexArrayInBytes, pVerticies, GL_STATIC_DRAW);*/

    ::g_pTheShaderManager = new cShaderManager();

    ::g_pTheShaderManager->setBasePath("assets/shaders/");

    cShaderManager::cShader vert_shader;
    vert_shader.fileName = "vertex_shader.glsl";

    cShaderManager::cShader frag_shader;
    frag_shader.fileName = "fragment_shader.glsl";


    if (::g_pTheShaderManager->createProgramFromFile("shader1", vert_shader, frag_shader))
    {
        std::cout << "Shaders succesfully created!" << std::endl;
    }
    else
    {
        std::cout << ::g_pTheShaderManager->getLastError() << std::endl;
    }

    program = ::g_pTheShaderManager->getIDFromFriendlyName("shader1");


   /* vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);
    program = glCreateProgram();

    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);*/

    mvp_location = glGetUniformLocation(program, "MVP");
    //vpos_location = glGetAttribLocation(program, "vPos");
    //vcol_location = glGetAttribLocation(program, "vCol");

    //glEnableVertexAttribArray(vpos_location);
    //glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE,
    //    sizeof(sVertex), (void*)0);
    //glEnableVertexAttribArray(vcol_location);
    //glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
    //    sizeof(sVertex), (void*)(sizeof(float) * 3));
   
    ::g_pMeshManager = new cVAOManager();
    sModelDrawInfo meshInfoCow;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/cow.ply",
        meshInfoCow, program))
    {
        std::cout << "Cow not loaded into VAO!" << std::endl;
    }

    sModelDrawInfo meshTeapot;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/Utah_Teapot.ply",
        meshInfoCow, program))
    {
        std::cout << "Teapot NOT loaded into VAO!" << std::endl;
    }

    sModelDrawInfo dolphinMeshInfo;

    if (!::g_pMeshManager->LoadModelIntoVAO("assets/models/dolphin.ply",
        dolphinMeshInfo, program))
    {
        std::cout << "Teapot NOT loaded into VAO!" << std::endl;
    }

    cMeshObject* pCow = new cMeshObject();
    pCow->bIsWireframe = true;
    pCow->bOverrideVertexModelColour = true;
    pCow->colourRGB = glm::vec3(0.0f, 1.0f, 0.0f);
    pCow->position.x = -10.f;
    pCow->orientation.z = 90.0f;
    pCow->meshFileName = "assets/models/cow.ply";

    cMeshObject* pCow2 = new cMeshObject();
    pCow2->bIsWireframe = false;
    pCow2->bOverrideVertexModelColour = true;
    pCow2->colourRGB = glm::vec3(1.0f, 0.0f, 0.0f);
    pCow2->position.x = 10.f;
    pCow2->scale = 0.5f;
    pCow2->meshFileName = "assets/models/cow.ply";

    ::g_pMeshesToDraw.push_back(pCow);
    ::g_pMeshesToDraw.push_back(pCow2);

    cMeshObject* pTeapot = new cMeshObject();
    //pTeapot->bIsVisible = false;
    pTeapot->bIsWireframe = true;
    pTeapot->meshFileName = "assets/models/Utah_Teapot.ply";

    ::g_pMeshesToDraw.push_back(pTeapot);

    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);



    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;
        //       mat4x4 m, p, mvp;
        glm::mat4 matModel, matProj, matView, mvp;
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float)height;
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);

        GLint proj_location = glGetUniformLocation(program, "mProj");
        GLint view_location = glGetUniformLocation(program, "mView");
        GLint Model_location = glGetUniformLocation(program, "mModel");

        //mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        matProj = glm::perspective(0.6f,
            ratio,
            0.1f,
            1000.0f);

        matView = glm::mat4(1.0f);

        glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);

        matView = glm::lookAt(g_cameraEye,
            cameraTarget,
            upVector);

        glUniformMatrix4fv(proj_location, 1, GL_FALSE, glm::value_ptr(matProj));
        glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(matView));

        for (unsigned int index = 0; index != ::g_pMeshesToDraw.size(); index++)
        {
            cMeshObject* pCurrentMesh = ::g_pMeshesToDraw[index];
            if (!pCurrentMesh->bIsVisible)
            {
                continue;
            }

            GLint useOverrideColor_location = glGetUniformLocation(program, "bUseOverrideColor");
            GLint overrideColor_location = glGetUniformLocation(program, "colorOverride");

            if (pCurrentMesh->bOverrideVertexModelColour)
            {
                glUniform3f(overrideColor_location, pCurrentMesh->colourRGB.r,
                    pCurrentMesh->colourRGB.g, pCurrentMesh->colourRGB.b);

                glUniform1f(useOverrideColor_location, GL_TRUE); // 1.0f

            }
            else
            {
                glUniform1f(useOverrideColor_location, GL_FALSE);
            }

            //         mat4x4_identity(m);
            matModel = glm::mat4(1.0f);

            glm::mat4 translation = glm::translate(glm::mat4(1.0f), pCurrentMesh->position);

            //mat4x4_rotate_Z(m, m, (float) glfwGetTime());
            glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f),
                pCurrentMesh->orientation.x,
                glm::vec3(1.0f, 0.0f, 0.0f));

            glm::mat4 rotateY = glm::rotate(glm::mat4(1.0f),
                pCurrentMesh->orientation.y,
                glm::vec3(0.0f, 1.0f, 0.0f));

            glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f),
                pCurrentMesh->orientation.z,
                glm::vec3(0.0f, 0.0f, 1.0f));

            float uniformScale = pCurrentMesh->scale;
            glm::mat4 scaleXYZ = glm::scale(glm::mat4(1.0f), 
                glm::vec3(uniformScale, uniformScale, uniformScale));

            matModel = matModel * translation * rotateX * rotateY * rotateZ * scaleXYZ;

            //m = m * rotateZ;

            //mat4x4_mul(mvp, p, m);
            //mvp = matProj * matView * matModel;

            if (pCurrentMesh->bIsWireframe)
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }
            else
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
            

            //glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
            glUniformMatrix4fv(Model_location, 1, GL_FALSE, glm::value_ptr(matModel));

            //glDrawArrays(GL_TRIANGLES, 0, g_NumVerticiesToDraw);
            sModelDrawInfo modelToDraw;

            if (::g_pMeshManager->FindDrawInfoByModelName(pCurrentMesh->meshFileName,
                modelToDraw))
            {
                glBindVertexArray(modelToDraw.VAO_ID);
                glDrawElements(GL_TRIANGLES, modelToDraw.numberOfIndices,
                    GL_UNSIGNED_INT, (void*)0);
                glBindVertexArray(0);
            }
        }

     
        std::stringstream ssWindowTitle;

        ssWindowTitle << "Camera (XYZ)" << ::g_cameraEye.x << ","
            << ::g_cameraEye.y << ", " << ::g_cameraEye.z << std::endl;

        glfwSetWindowTitle(window, ssWindowTitle.str().c_str());


   

       /* if (::g_pMeshManager->FindDrawInfoByModelName("assets/models/cow.ply",
            modelToDraw))
        {
            glBindVertexArray(modelToDraw.VAO_ID);
            glDrawElements(GL_TRIANGLES, modelToDraw.numberOfIndices, 
                GL_UNSIGNED_INT, (void*)0);
            glBindVertexArray(0);
        }

        if (::g_pMeshManager->FindDrawInfoByModelName("assets/models/Utah_Teapot.ply",
            modelToDraw))
        {
            glBindVertexArray(modelToDraw.VAO_ID);
            glDrawElements(GL_TRIANGLES, modelToDraw.numberOfIndices,
                GL_UNSIGNED_INT, (void*)0);
            glBindVertexArray(0);
        }

        if (::g_pMeshManager->FindDrawInfoByModelName("assets/models/dolphin.ply",
            modelToDraw))
        {
            glBindVertexArray(modelToDraw.VAO_ID);
            glDrawElements(GL_TRIANGLES, modelToDraw.numberOfIndices,
                GL_UNSIGNED_INT, (void*)0);
            glBindVertexArray(0);
        }*/



        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
