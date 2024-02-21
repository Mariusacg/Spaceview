#if defined(__APPLE__)
#define GLFW_INCLUDE_GLCOREARB
#define GL_SILENCE_DEPRECATION
#else
#define GLEW_STATIC
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>                  //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp>   //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp>         //glm extension for accessing the internal data structure of glm types

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"

#include "SkyBox.hpp"

#include <iostream>
#include <cstdlib>
#include <ctime>

using glm::vec3, glm::mat4, glm::mat3, glm::vec4;

// window
int glWindowWidth = 2560;
int glWindowHeight = 1600;
int retina_width, retina_height;
GLFWwindow *glWindow = NULL;

const unsigned int SHADOW_WIDTH = 8192;
const unsigned int SHADOW_HEIGHT = 8192;

// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat4 lightRotation;

glm::mat3 normalMatrix;
glm::mat4 navaRandomRotation;
// light parameters
glm::vec3 lightDir;
glm::vec3 lightColor;

// shader uniform locations
GLint modelLoc;
GLint viewLoc;
GLint projectionLoc;
GLint normalMatrixLoc;
GLint lightDirLoc;
GLint lightColorLoc;
GLint lightPointDir;
GLint lightPointPos;
GLint amplifier;

// camera
gps::Camera myCamera(
    glm::vec3(-712.879f, 9.12177f, -244.182f),
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));

GLfloat cameraSpeed = 0.1f;
GLfloat lightAngle;
GLfloat angle;
float angleY = 0.0f;
vec3 navaCoord = vec3(352.0f, 0.0f, 0.0f);

GLboolean pressedKeys[1024];

// models
gps::Model3D moon;
gps::Model3D nava;
gps::Model3D spaceport;
gps::Model3D proiectil;
gps::Model3D lightCube;
gps::Model3D screenQuad;

// shaders
gps::Shader myBasicShader;
gps::Shader lightShader;
gps::Shader screenQuadShader;
gps::Shader depthMapShader;

GLuint shadowMapFBO;
GLuint depthMapTexture;

bool showDepthMap;

// for skybox
gps::SkyBox mySkyBox;
gps::Shader skyboxShader;

// mouse movement
bool mouse = true;
float lastX = 400, lastY = 300;
float yaw, pitch;

bool flashlight = false;
bool intro = true;
std::vector<vec3> introPoints;
float tempbetweenPoints = 0.0f;
int counter100 = 0;
int const NR_FPS_VSYNC = 165;
int counter165 = 0;
float randomm, randommAnt;
float randomnava, randomAntnava;
vec3 punctCurent = vec3(-712.879f, 9.12177f, -244.182f), punctViitor = vec3(-59.2134f, 18.0912f, -72.1994f);

struct bullet
{
    vec3 position;
    vec3 direction;
    float rotation;
    float distance;
};
std::vector<bullet> bullets;
std::ostream &operator<<(std::ostream &stream, const vec3 &other)
{
    stream << other.x << ", " << other.y << ", " << other.z;
    return stream;
}
std::ostream &operator<<(std::ostream &stream, const vec4 &other)
{
    stream << other.x << ", " << other.y << ", " << other.z << ", " << other.w;
    return stream;
}
bool operator==(const vec3 &other, const vec3 &other2)
{
    return other.x == other2.x && other.y == other2.y && other.z == other2.z;
}
vec3 operator+(const vec3 &other, const vec4 &other2)
{
    return vec3(other.x + other2.x, other.y + other2.y, other.z + other2.z);
}

GLenum glCheckError_(const char *file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
        case GL_INVALID_ENUM:
            error = "INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            error = "INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            error = "INVALID_OPERATION";
            break;
        case GL_OUT_OF_MEMORY:
            error = "OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error = "INVALID_FRAMEBUFFER_OPERATION";
            break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow *window, int width, int height)
{
    fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
    // TODO
}

void keyboardCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key == GLFW_KEY_M && action == GLFW_PRESS)
    {
        showDepthMap = !showDepthMap;
    }
    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
    {
        bullet newBullet = bullet();
        newBullet.position = navaCoord + (glm::rotate(mat4(1.0f), glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f)) * vec4(172.74f, 13.85f, -24.71f, 1.0f));
        newBullet.rotation = angleY + 269;
        newBullet.direction = glm::rotate(mat4(1.0f), glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f)) * vec4(1.0f, 0.0f, 0.0f, 1.0f);
        bullets.push_back(newBullet);
        newBullet = bullet();
        newBullet.position = navaCoord + (glm::rotate(mat4(1.0f), glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f)) * vec4(172.74f, 13.85f, 17.1491f, 1.0f));
        newBullet.rotation = angleY + 269;
        newBullet.direction = glm::rotate(mat4(1.0f), glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f)) * vec4(1.0f, 0.0f, 0.0f, 1.0f);
        bullets.push_back(newBullet);
    }
    if (key == GLFW_KEY_F && action == GLFW_PRESS)
    {
        if (flashlight == false)
        {
            myBasicShader.useShaderProgram();
            glUniform1f(amplifier, 500.0f);
            flashlight = true;
        }
        else
        {
            myBasicShader.useShaderProgram();
            glUniform1f(amplifier, 0.0f);
            flashlight = false;
        }
    }

    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
        {
            pressedKeys[key] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            pressedKeys[key] = false;
        }
    }
}

void mouseCallback(GLFWwindow *window, double xpos, double ypos)
{
    if (mouse)
    {
        lastX = xpos;
        lastY = ypos;
        mouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = -lastY + ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.5f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw -= xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
    {
        pitch = 89.0f;
    }
    if (pitch < -89.0f)
    {
        pitch = -89.0f;
    }
}

void processMovement()
{

    // move camera forward
    if (pressedKeys[GLFW_KEY_LEFT_SHIFT])
    {
        cameraSpeed = 2.5;
    }
    else
    {
        cameraSpeed = 0.1;
    }
    if (pressedKeys[GLFW_KEY_W])
    {
        myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
    }

    // move camera backward
    if (pressedKeys[GLFW_KEY_S])
    {
        myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
    }

    // move camera left
    if (pressedKeys[GLFW_KEY_A])
    {
        myCamera.move(gps::MOVE_LEFT, cameraSpeed);
    }

    // move camera right
    if (pressedKeys[GLFW_KEY_D])
    {
        myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
    }

    if (pressedKeys[GLFW_KEY_Q])
    {
        angleY -= 1.0f;
    }

    if (pressedKeys[GLFW_KEY_E])
    {
        angleY += 1.0f;
    }

    // move light cube
    if (pressedKeys[GLFW_KEY_Z])
    {
        lightAngle -= 1.0f;
    }

    // move light cube
    if (pressedKeys[GLFW_KEY_X])
    {
        lightAngle += 1.0f;
    }

    if (pressedKeys[GLFW_KEY_UP])
    {
        navaCoord = navaCoord + (glm::rotate(mat4(1.0f), glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f)) * vec4(1.0f, 0.0f, 0.0f, 1.0f));
    }
    if (pressedKeys[GLFW_KEY_DOWN])
    {
        navaCoord = navaCoord + (glm::rotate(mat4(1.0f), glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f)) * vec4(-1.0f, 0.0f, 0.0f, 1.0f));
    }
    if (pressedKeys[GLFW_KEY_RIGHT])
    {
        navaCoord = navaCoord + (glm::rotate(mat4(1.0f), glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f)) * vec4(0.0f, 0.0f, 1.0f, 1.0f));
    }
    if (pressedKeys[GLFW_KEY_LEFT])
    {
        navaCoord = navaCoord + (glm::rotate(mat4(1.0f), glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f)) * vec4(0.0f, 0.0f, -1.0f, 1.0f));
    }

    if (pressedKeys[GLFW_KEY_SPACE])
    {
        navaCoord += vec3(0.0f, 1.0f, 0.0f);
    }
    if (pressedKeys[GLFW_KEY_LEFT_CONTROL])
    {
        navaCoord += vec3(0.0f, -1.0f, 0.0f);
    }
    if (pressedKeys[GLFW_KEY_C])
    {
        std::cout << "Camera Position: " << myCamera.getPosition() << std::endl;
        std::cout << bullets.empty() << std::endl;
    }

    // point view
    if (pressedKeys[GLFW_KEY_1])
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    }

    // line view
    if (pressedKeys[GLFW_KEY_2])
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    // normal view
    if (pressedKeys[GLFW_KEY_3])
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

bool initOpenGLWindow()
{
    if (!glfwInit())
    {
        fprintf(stderr, "ERROR: could not start GLFW3\n");
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // window scaling for HiDPI displays
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

    // for sRBG framebuffer
    glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

    // for antialising
    glfwWindowHint(GLFW_SAMPLES, 4);

    glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
    if (!glWindow)
    {
        fprintf(stderr, "ERROR: could not open window with GLFW3\n");
        glfwTerminate();
        return false;
    }

    glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
    glfwSetKeyCallback(glWindow, keyboardCallback);
    glfwSetCursorPosCallback(glWindow, mouseCallback);
    // glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwMakeContextCurrent(glWindow);
    glfwSwapInterval(1);

#if not defined(__APPLE__)
    // start GLEW extension handler
    glewExperimental = GL_TRUE;
    glewInit();
#endif

    // get version info
    const GLubyte *renderer = glGetString(GL_RENDERER); // get renderer string
    const GLubyte *version = glGetString(GL_VERSION);   // version as a string
    printf("Renderer: %s\n", renderer);
    printf("OpenGL version supported %s\n", version);

    // for RETINA display
    glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

    return true;
}

void initFBO()
{
    // generate FBO ID
    glGenFramebuffers(1, &shadowMapFBO);

    // create depth texture for FBO
    glGenTextures(1, &depthMapTexture);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                 SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // attach texture to FBO
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void initOpenGLState()
{
    glClearColor(0.6f, 0.6f, 0.6f, 1.0f);
    glViewport(0, 0, retina_width, retina_height);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc(GL_LESS);    // depth-testing interprets a smaller value as "closer"
    glEnable(GL_CULL_FACE);  // cull face
    glCullFace(GL_BACK);     // cull back face
    glFrontFace(GL_CCW);     // GL_CCW for counter clock-wise
}

void initSkyBox()
{
    std::vector<const GLchar *> faces;
    faces.push_back("skybox/front.tga");
    faces.push_back("skybox/back.tga");
    faces.push_back("skybox/top.tga");
    faces.push_back("skybox/down.tga");
    faces.push_back("skybox/left.tga");
    faces.push_back("skybox/right.tga");
    mySkyBox.Load(faces);
}

void initModels()
{
    nava.LoadModel("models/Test/Teste.obj");
    spaceport.LoadModel("models/Spaceport/untitled.obj");
    proiectil.LoadModel("models/Proiectil/bullet.obj");
    lightCube.LoadModel("models/cube/cube.obj");
    screenQuad.LoadModel("models/quad/quad.obj");
    moon.LoadModel("models/Moon/moon.obj");
}

void initShaders()
{

    lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
    lightShader.useShaderProgram();

    myBasicShader.loadShader("shaders/mainShader.vert", "shaders/mainShader.frag");
    myBasicShader.useShaderProgram();
    depthMapShader.loadShader("shaders/lightSpaceTrMatrix.vert", "shaders/lightSpaceTrMatrix.frag");
    depthMapShader.useShaderProgram();
    screenQuadShader.loadShader("shaders/screenQuad.vert", "shaders/screenQuad.frag");
    screenQuadShader.useShaderProgram();
    skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
    skyboxShader.useShaderProgram();
}

void initUniforms()
{
    myBasicShader.useShaderProgram();

    // create model matrix
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    // get view matrix for current camera
    view = myCamera.getViewMatrixWrotate(pitch, yaw);
    viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
    // send view matrix to shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    // create projection matrix
    projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 5000.0f);
    projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
    // send projection matrix to shader
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // set the light direction (direction towards the light)
    lightDir = glm::vec3(1.0f, 0.5f, 1.0f);
    lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
    // send light dir to shader
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

    // set light color
    lightColor = glm::vec3(1.0f, 1.0f, 1.0f); // white light
    lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
    // send light color to shader
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

    // set light point direction
    lightPointDir = glGetUniformLocation(myBasicShader.shaderProgram, "pointLightdirection");
    // send light dir to shader
    glUniform3fv(lightPointDir, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

    // set light point position
    lightPointPos = glGetUniformLocation(myBasicShader.shaderProgram, "pointLightPos");
    // send light dir to shader
    glUniform3fv(lightPointPos, 1, glm::value_ptr(navaCoord));

    amplifier = glGetUniformLocation(myBasicShader.shaderProgram, "amplifier");
    glUniform1f(amplifier, 0.0f);

    lightShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}

glm::mat4 computeLightSpaceTrMatrix()
{
    glm::mat4 lightView = glm::lookAt(glm::normalize((glm::mat3(lightRotation)) * lightDir), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    const GLfloat near_plane = -2000.0f, far_plane = 5000.0f;
    glm::mat4 lightProjection = glm::ortho(-1000.0f, 1000.0f, -1000.0f, 2000.0f, near_plane, far_plane);
    glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;
    return lightSpaceTrMatrix;
}

void drawObjects(gps::Shader shader, bool depthPass)
{

    // Draw nava
    shader.useShaderProgram();
    model = mat4(1.0f);
    model = glm::translate(model, navaCoord);
    model = glm::rotate(model, glm::radians(angleY - 269.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    float ajutatornava = (float)counter165 / 165;
    navaRandomRotation = glm::rotate(mat4(1.0f), glm::radians((1 - ajutatornava) * randomAntnava + ajutatornava * randomnava), vec3(0.0f, 0.0f, 1.0f));
    navaRandomRotation = glm::rotate(navaRandomRotation, glm::radians((1 - ajutatornava) * randomAntnava + ajutatornava * randomnava), vec3(0.0f, 1.0f, 0.0f));
    navaRandomRotation = glm::rotate(navaRandomRotation, glm::radians((1 - ajutatornava) * randomAntnava + ajutatornava * randomnava), vec3(1.0f, 0.0f, 0.0f));
    model = model * navaRandomRotation;
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass)
    {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    nava.Draw(shader);

    // Draw moon
    model = glm::translate(glm::mat4(1.0f), glm::vec3(500.0f, -1250.0f, 200.0f));
    model = glm::scale(model, glm::vec3(1000.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass)
    {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    moon.Draw(shader);

    // Draw spacePort
    model = glm::rotate(mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    float ajutator = (float)counter165 / 165;
    model = glm::rotate(model, glm::radians((1 - ajutator) * randommAnt + ajutator * randomm), vec3(0.0f, 0.0f, 1.0f));
    model = glm::rotate(model, glm::radians((1 - ajutator) * randommAnt + ajutator * randomm), vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians((1 - ajutator) * randommAnt + ajutator * randomm), vec3(1.0f, 0.0f, 0.0f));
    model = glm::translate(model, glm::vec3(-32.0f, -46.0f, -264.0f));
    model = glm::scale(model, glm::vec3(55.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    // do not send the normal matrix if we are rendering in the depth map
    if (!depthPass)
    {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }
    spaceport.Draw(shader);
    // Draw bullets
    for (bullet bls : bullets)
    {
        model = glm::translate(glm::mat4(1.0f), bls.position + bls.distance * bls.direction);
        model = glm::rotate(model, glm::radians(bls.rotation), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(15.0f));
        glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        // do not send the normal matrix if we are rendering in the depth map
        if (!depthPass)
        {
            normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
            glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
        }
        proiectil.Draw(shader);
    }
}

void renderScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    depthMapShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
                       1,
                       GL_FALSE,
                       glm::value_ptr(computeLightSpaceTrMatrix()));
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    drawObjects(depthMapShader, false);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (showDepthMap)
    {
        glViewport(0, 0, retina_width, retina_height);

        glClear(GL_COLOR_BUFFER_BIT);

        screenQuadShader.useShaderProgram();

        // bind the depth map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);

        glDisable(GL_DEPTH_TEST);
        screenQuad.Draw(screenQuadShader);
        glEnable(GL_DEPTH_TEST);
    }
    else
    {

        // final scene rendering pass (with shadows)
        if (intro)
        {
            tempbetweenPoints += 0.01f;
            counter100 += 1;
            if (counter100 == 100)
            {
                if (introPoints.empty() == true)
                {
                    intro = false;
                }
                else
                {
                    counter100 = 0;
                    tempbetweenPoints = 0.00;
                    punctCurent = punctViitor;
                    punctViitor = introPoints.front();
                    introPoints.erase(introPoints.begin());
                }
            }
            myCamera.setPosition((1 - tempbetweenPoints) * punctCurent + tempbetweenPoints * punctViitor);
        }
        glViewport(0, 0, retina_width, retina_height);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        myBasicShader.useShaderProgram();

        view = myCamera.getViewMatrixWrotate(pitch, yaw);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

        vec4 test = view * vec4(navaCoord + (glm::rotate(mat4(1.0f), glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f)) * vec4(275.0f, 0.0f, -3.780f, 1.0f)), 1.0f);
        glUniform3fv(lightPointPos, 1, glm::value_ptr(vec3(test.x, test.y, test.z)));
        mat4 lightPointRotation = glm::rotate(mat4(1.0f), glm::radians(angleY + 239), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniform3fv(lightPointDir, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightPointRotation)) * lightDir));

        // bind the shadow map
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "shadowMap"), 3);

        glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "lightSpaceTrMatrix"),
                           1,
                           GL_FALSE,
                           glm::value_ptr(computeLightSpaceTrMatrix()));

        drawObjects(myBasicShader, false);

        // draw a white cube around the light

        lightShader.useShaderProgram();

        glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

        model = lightRotation;
        model = glm::translate(model, 5.0f * lightDir);
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        lightCube.Draw(lightShader);
        mySkyBox.Draw(skyboxShader, view, projection);
    }
}
void initIntro()
{
    introPoints.push_back(glm::vec3(362.203f, 20.5277f, -40.6377f));
    introPoints.push_back(glm::vec3(543.716f, 53.8854f, -29.7531f));
    introPoints.push_back(glm::vec3(481.362f, 37.443f, 39.364f));
    introPoints.push_back(glm::vec3(352.0f, 45.483f, -6.2229f));
}
void actualizareMiscari()
{
    int i = 0;
    for (bullet &bls : bullets)
    {
        bls.distance = bls.distance + 1.0f;
        if (bls.distance > 500.0f)
            bullets.erase(bullets.begin() + i);
        i++;
    }
    if (counter165 == NR_FPS_VSYNC)
    {
        randommAnt = randomm;
        randomm = (-1.0f) + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (2.0f)));
        randomAntnava = randomnava;
        randomnava = (-1.0f) + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (2.0f)));
        counter165 = 0;
    }
    else
        counter165++;
}
void cleanup()
{
    glDeleteTextures(1, &depthMapTexture);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &shadowMapFBO);
    glfwDestroyWindow(glWindow);
    // close GL context and any other GLFW resources
    glfwTerminate();
}

int main(int argc, const char *argv[])
{
    srand((unsigned)time(0));

    if (!initOpenGLWindow())
    {
        glfwTerminate();
        return 1;
    }

    initOpenGLState();
    initFBO();
    initSkyBox();
    initModels();
    initShaders();
    initUniforms();
    initIntro();

    glCheckError();

    glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    while (!glfwWindowShouldClose(glWindow))
    {
        processMovement();
        renderScene();
        actualizareMiscari();

        glfwPollEvents();
        glfwSwapBuffers(glWindow);
    }

    cleanup();

    return EXIT_SUCCESS;
}
