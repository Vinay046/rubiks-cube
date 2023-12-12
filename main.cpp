#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

GLfloat WINDOW_WIDTH = 1920;
GLfloat WINDOW_HEIGHT = 1080;

GLFWwindow *window;

float angleX = 0.0f;
float angleY = 0.0f;
float angleZ = 0.0f;
double prevXPos = 0.0;
double prevYPos = 0.0;
bool mousePressedLeft = false;
bool mousePressedRight = false;

const char *vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;
    out vec3 vertexColor;
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    void main() {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
        vertexColor = aColor;
    }
)";

const char *fragmentShaderSource = R"(
    #version 330 core
    in vec3 vertexColor;
    out vec4 FragColor;
    void main() {
        FragColor = vec4(vertexColor, 1.0);
    }
)";

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
void cursorPositionCallback(GLFWwindow *window, double xpos, double ypos);

void renderCube(unsigned int shaderProgram, glm::vec3 position, glm::vec3 rotationAngles, glm::vec3 orientation)
{
    // Cube vertices and indices
    GLfloat vertices[] = {
        // front face - red
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, // 0
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,  // 1
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,   // 2
        -0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,  // 3

        // top face - yellow (red + green)
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, // 4
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f,  // 5
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f,   // 6
        -0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f,  // 7

        // left face - blue
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // 8
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f,  // 9
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,   // 10
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,  // 11

        // right face - green
        0.5f, -0.5f, -0.5f, 0.4f, 0.4f, 0.0f, // 12
        0.5f, 0.5f, -0.5f, 0.4f, 0.4f, 0.0f,  // 13
        0.5f, 0.5f, 0.5f, 0.4f, 0.4f, 0.0f,   // 14
        0.5f, -0.5f, 0.5f, 0.4f, 0.4f, 0.0f,  // 15

        // back face - orange (red + green but less intense than yellow)
        -0.5f, -0.5f, 0.5f, 1.0f, 0.5f, 0.0f, // 16
        0.5f, -0.5f, 0.5f, 1.0f, 0.5f, 0.0f,  // 17
        0.5f, 0.5f, 0.5f, 1.0f, 0.5f, 0.0f,   // 18
        -0.5f, 0.5f, 0.5f, 1.0f, 0.5f, 0.0f,  // 19

        // bottom face - white
        -0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, // 20
        0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f,  // 21
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, // 22
        -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f // 23
    };

    unsigned int indices[] = {
        0, 1, 2, 2, 3, 0,       // Front face
        4, 5, 6, 6, 7, 4,       // Back face
        8, 9, 10, 10, 11, 8,    // Left face
        12, 13, 14, 14, 15, 12, // Right face
        16, 17, 18, 18, 19, 16, // Bottom face
        20, 21, 22, 22, 23, 20  // Top face
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glBindVertexArray(VAO);

    // Set the model matrix
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(rotationAngles.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotationAngles.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotationAngles.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, position);
    model = glm::scale(model, orientation);

    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    // Draw the cube
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}
int main()
{
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 16); // Request 4x multisampling

    // After creating the OpenGL context with glfwMakeContextCurrent
    glEnable(GL_MULTISAMPLE); // Enable

    // Create GLFW window
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Colored Cube", NULL, NULL);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Set key callback function
    glfwSetKeyCallback(window, key_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursorPosCallback(window, cursorPositionCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Set viewport
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);

    // Compile shaders
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // Check for shader compile errors (not shown for brevity)

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // Check for shader compile errors (not shown for brevity)

    // Link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // Check for linking errors (not shown for brevity)

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Process input
        glfwPollEvents();

        // Render
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use shader program
        glUseProgram(shaderProgram);

        // Set the projection matrix
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // Set the view matrix
        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

        // center peices
        {
            glm::vec3 position0(0.0f, 1.0f, 0.0f);
            glm::vec3 rotationAngles0(angleX, (GLfloat)glfwGetTime() * 30, 0.0f);
            glm::vec3 orientation0(1.0f, 1.0f, 1.0f);
            renderCube(shaderProgram, position0, rotationAngles0, orientation0);

            glm::vec3 position1(0.0f, -1.0f, 0.0f);
            glm::vec3 rotationAngles1(angleX, angleY, 0.0f);
            glm::vec3 orientation1(1.0f, 1.0f, 1.0f);
            renderCube(shaderProgram, position1, rotationAngles1, orientation1);

            glm::vec3 position2(1.0f, 0.0f, 0.0f);
            glm::vec3 rotationAngles2(angleX, angleY, 0.0f);
            glm::vec3 orientation2(1.0f, 1.0f, 1.0f);
            renderCube(shaderProgram, position2, rotationAngles2, orientation2);

            glm::vec3 position3(-1.0f, 0.0f, 0.0f);
            glm::vec3 rotationAngles3(angleX, angleY, 0.0f);
            glm::vec3 orientation3(1.0f, 1.0f, 1.0f);
            renderCube(shaderProgram, position3, rotationAngles3, orientation3);

            glm::vec3 position4(0.0f, 0.0f, -1.0f);
            glm::vec3 rotationAngles4(angleX, angleY, 0.0f);
            glm::vec3 orientation4(1.0f, 1.0f, 1.0f);
            renderCube(shaderProgram, position4, rotationAngles4, orientation4);

            glm::vec3 position5(0.0f, 0.0f, 1.0f);
            glm::vec3 rotationAngles5(angleX, angleY, 0.0f);
            glm::vec3 orientation5(1.0f, 1.0f, 1.0f);
            renderCube(shaderProgram, position5, rotationAngles5, orientation5);
        }

        // edge peices
        {
            glm::vec3 position6(0.0f, 1.0f, 1.0f);
            glm::vec3 rotationAngles6(angleX, (GLfloat)glfwGetTime() * 30, 0.0f);
            glm::vec3 orientation6(1.0f, 1.0f, 1.0f);
            renderCube(shaderProgram, position6, rotationAngles6, orientation6);

            glm::vec3 position7(0.0f, 1.0f, -1.0f);
            glm::vec3 rotationAngles7(angleX, (GLfloat)glfwGetTime() * 30, 0.0f);
            glm::vec3 orientation7(1.0f, 1.0f, 1.0f);
            renderCube(shaderProgram, position7, rotationAngles7, orientation7);

            glm::vec3 position8(0.0f, -1.0f, 1.0f);
            glm::vec3 rotationAngles8(angleX, angleY, 0.0f);
            glm::vec3 orientation8(1.0f, 1.0f, 1.0f);
            renderCube(shaderProgram, position8, rotationAngles8, orientation8);

            glm::vec3 position9(0.0f, -1.0f, -1.0f);
            glm::vec3 rotationAngles9(angleX, angleY, 0.0f);
            glm::vec3 orientation9(1.0f, 1.0f, 1.0f);
            renderCube(shaderProgram, position9, rotationAngles9, orientation9);

            glm::vec3 position10(1.0f, 1.0f, 0.0f);
            glm::vec3 rotationAngles10(angleX, (GLfloat)glfwGetTime() * 30, 0.0f);
            glm::vec3 orientation10(1.0f, 1.0f, 1.0f);
            renderCube(shaderProgram, position10, rotationAngles10, orientation10);

            glm::vec3 position11(1.0f, -1.0f, 0.0f);
            glm::vec3 rotationAngles11(angleX, angleY, 0.0f);
            glm::vec3 orientation11(1.0f, 1.0f, 1.0f);
            renderCube(shaderProgram, position11, rotationAngles11, orientation11);

            glm::vec3 position12(-1.0f, 1.0f, 0.0f);
            glm::vec3 rotationAngles12(angleX, (GLfloat)glfwGetTime() * 30, 0.0f);
            glm::vec3 orientation12(1.0f, 1.0f, 1.0f);
            renderCube(shaderProgram, position12, rotationAngles12, orientation12);

            glm::vec3 position13(-1.0f, -1.0f, 0.0f);
            glm::vec3 rotationAngles13(angleX, angleY, 0.0f);
            glm::vec3 orientation13(1.0f, 1.0f, 1.0f);
            renderCube(shaderProgram, position13, rotationAngles13, orientation13);

            glm::vec3 position14(1.0f, 0.0f, 1.0f);
            glm::vec3 rotationAngles14(angleX, angleY, 0.0f);
            glm::vec3 orientation14(1.0f, 1.0f, 1.0f);
            renderCube(shaderProgram, position14, rotationAngles14, orientation14);

            glm::vec3 position15(-1.0f, 0.0f, 1.0f);
            glm::vec3 rotationAngles15(angleX, angleY, 0.0f);
            glm::vec3 orientation15(1.0f, 1.0f, 1.0f);
            renderCube(shaderProgram, position15, rotationAngles15, orientation15);

            glm::vec3 position16(1.0f, 0.0f, -1.0f);
            glm::vec3 rotationAngles16(angleX, angleY, 0.0f);
            glm::vec3 orientation16(1.0f, 1.0f, 1.0f);
            renderCube(shaderProgram, position16, rotationAngles16, orientation16);

            glm::vec3 position17(-1.0f, 0.0f, -1.0f);
            glm::vec3 rotationAngles17(angleX, angleY, 0.0f);
            glm::vec3 orientation17(1.0f, 1.0f, 1.0f);
            renderCube(shaderProgram, position17, rotationAngles17, orientation17);
        }

        // corner peices
        {
            glm::vec3 position18(1.0f, 1.0f, -1.0f);
            glm::vec3 rotationAngles18(angleX, (GLfloat)glfwGetTime() * 30, 0.0f);
            glm::vec3 orientation18(1.0f, 1.0f, 1.0f);
            renderCube(shaderProgram, position18, rotationAngles18, orientation18);

            glm::vec3 position19(1.0f, 1.0f, 1.0f);
            glm::vec3 rotationAngles19(angleX, (GLfloat)glfwGetTime() * 30, 0.0f);
            glm::vec3 orientation19(1.0f, 1.0f, 1.0f);
            renderCube(shaderProgram, position19, rotationAngles19, orientation19);

            glm::vec3 position20(-1.0f, 1.0f, -1.0f);
            glm::vec3 rotationAngles20(angleX, (GLfloat)glfwGetTime() * 30, 0.0f);
            glm::vec3 orientation20(1.0f, 1.0f, 1.0f);
            renderCube(shaderProgram, position20, rotationAngles20, orientation20);

            glm::vec3 position21(-1.0f, 1.0f, 1.0f);
            glm::vec3 rotationAngles21(angleX, (GLfloat)glfwGetTime() * 30, 0.0f);
            glm::vec3 orientation21(1.0f, 1.0f, 1.0f);
            renderCube(shaderProgram, position21, rotationAngles21, orientation21);

            glm::vec3 position22(1.0f, -1.0f, -1.0f);
            glm::vec3 rotationAngles22(angleX, angleY, 0.0f);
            glm::vec3 orientation22(1.0f, 1.0f, 1.0f);
            renderCube(shaderProgram, position22, rotationAngles22, orientation22);

            glm::vec3 position23(1.0f, -1.0f, 1.0f);
            glm::vec3 rotationAngles23(angleX, angleY, 0.0f);
            glm::vec3 orientation23(1.0f, 1.0f, 1.0f);
            renderCube(shaderProgram, position23, rotationAngles23, orientation23);

            glm::vec3 position24(-1.0f, -1.0f, -1.0f);
            glm::vec3 rotationAngles24(angleX, angleY, 0.0f);
            glm::vec3 orientation24(1.0f, 1.0f, 1.0f);
            renderCube(shaderProgram, position24, rotationAngles24, orientation24);

            glm::vec3 position25(-1.0f, -1.0f, 1.0f);
            glm::vec3 rotationAngles25(angleX, angleY, 0.0f);
            glm::vec3 orientation25(1.0f, 1.0f, 1.0f);
            renderCube(shaderProgram, position25, rotationAngles25, orientation25);
        }
        // Swap buffers and poll IO events
        glfwSwapBuffers(window);
    }

    // Clean up and exit
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    // Close the window when the user presses the ESC key
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
    WINDOW_WIDTH = static_cast<GLfloat>(width);
    WINDOW_HEIGHT = static_cast<GLfloat>(height);
}

void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            mousePressedLeft = true;
        }
        else if (action == GLFW_RELEASE)
        {
            mousePressedLeft = false;
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (action == GLFW_PRESS)
        {
            mousePressedRight = true;
        }
        else if (action == GLFW_RELEASE)
        {
            mousePressedRight = false;
        }
    }
}

void cursorPositionCallback(GLFWwindow *window, double xpos, double ypos)
{
    if (mousePressedLeft)
    {
        double deltaX = xpos - prevXPos;
        double deltaY = ypos - prevYPos;

        // Increase the sensitivity by scaling the delta values
        float sensitivity = 0.08f;
        deltaX *= sensitivity;
        deltaY *= sensitivity;

        angleY += static_cast<float>(deltaX); // Rotate around Y-axis
        angleX += static_cast<float>(deltaY); // Rotate around X-axis
    }
    prevXPos = xpos;
    prevYPos = ypos;
}

// (GLfloat)glfwGetTime() * 30