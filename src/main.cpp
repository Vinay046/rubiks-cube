#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <shader.hpp>
#include <texture.hpp>
#include <VAO.hpp>
#include <VBO.hpp>
#include <EBO.hpp>

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

float rotationSpeed = 90.0f; // Degrees per second

float targetAngleX = 0.0f; // Target rotation angle
bool ifRotatingX = false;  // Flag to check if rotation is ongoing

double lastFrameTime = 0.0;
double deltaTime = 0.0;

/* Vertices with texture coords */
float vertices[] = {
    // positions          // colors           // texture coords
    -0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
     0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f, // bottom right
     0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f, // bottom left
    -0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f, // top left

    -0.5f, 0.5f, -0.5f,    1.0f, 1.0f, 0.0f,   1.0f, 1.0f, // top right
     0.5f, 0.5f, -0.5f,    1.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
     0.5f, 0.5f,  0.5f,    1.0f, 1.0f, 0.0f,   0.0f, 0.0f, // bottom left
    -0.5f, 0.5f,  0.5f,    1.0f, 1.0f, 0.0f,   0.0f, 1.0f, // top left

    -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f, // top right
    -0.5f,  0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f, // bottom right
    -0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
    -0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f, // top left

     0.5f, -0.5f, -0.5f,   0.4f, 0.4f, 0.0f,   1.0f, 1.0f, // top right
     0.5f,  0.5f, -0.5f,   0.4f, 0.4f, 0.0f,   1.0f, 0.0f, // bottom right
     0.5f,  0.5f,  0.5f,   0.4f, 0.4f, 0.0f,   0.0f, 0.0f, // bottom left
     0.5f, -0.5f,  0.5f,   0.4f, 0.4f, 0.0f,   0.0f, 1.0f, // top left

    -0.5f, -0.5f,  0.5f,   1.0f, 0.5f, 0.0f,   1.0f, 1.0f, // top right
     0.5f, -0.5f,  0.5f,   1.0f, 0.5f, 0.0f,   1.0f, 0.0f, // bottom right
     0.5f,  0.5f,  0.5f,   1.0f, 0.5f, 0.0f,   0.0f, 0.0f, // bottom left
    -0.5f,  0.5f,  0.5f,   1.0f, 0.5f, 0.0f,   0.0f, 1.0f, // top left

    -0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, // top right
     0.5f, -0.5f,  0.5f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f, // bottom right
     0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f, // bottom left
    -0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f  // top left
};

unsigned int indices[] = {
    0,  1,  2,  2,  3,  0,       // Front face
    4,  5,  6,  6,  7,  4,       // Back face
    8,  9,  10, 10, 11, 8,       // Left face
    12, 13, 14, 14, 15, 12,      // Right face
    16, 17, 18, 18, 19, 16,      // Bottom face
    20, 21, 22, 22, 23, 20       // Top face
};


void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
void cursorPositionCallback(GLFWwindow *window, double xpos, double ypos);

void renderCube(Shader shader, glm::vec3 position, glm::vec3 rotationAngles, glm::vec3 orientation)
{
    // Set the model matrix
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(rotationAngles.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotationAngles.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotationAngles.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, position);
    model = glm::scale(model, orientation);

    shader.setMatrix4("model", model);

    // Draw the cube
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

int main()
{
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 16); // Request 16x multisampling

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

    Shader shader("shaders/cube.vert", "shaders/cube.frag");
    unsigned int shaderProgram = shader.ID;

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    /* Generates Vertex Array Object and binds it */
    VAO VAO1;
    VAO1.Bind();

    /* Generates Vertex Buffer Object and links it to vertices */
    VBO VBO1(vertices, sizeof(vertices));
    /* Generates Element Buffer Object and links it to indices */
    EBO EBO1(indices, sizeof(indices));

    /* Links VBO attributes such as coordinates and colors to VAO */
    VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 8* sizeof(float), (void*)0);
    VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 8* sizeof(float), (void*)(3 * sizeof(float)));
    VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 8* sizeof(float), (void*)(6 * sizeof(float)));

    /* Unbind all to prevent accidentally modifying them */
    VAO1.Unbind();
    VBO1.Unbind();
    EBO1.Unbind();

    /* Load and create a texture */
    Texture cubeFace("resources/textures/edgeFace.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);

    lastFrameTime = glfwGetTime(); // Call this just before your main loop starts

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        double currentFrameTime = glfwGetTime();
        deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;
        // Process input
        glfwPollEvents();

        // Render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        cubeFace.Bind();

        if (ifRotatingX)
        {
            float rotationStep = rotationSpeed * deltaTime;
            if (abs(targetAngleX - angleX) < rotationStep)
            {
                angleX = targetAngleX;
                ifRotatingX = false;
            }
            else if (targetAngleX > angleX)
            {
                angleX += rotationStep;
            }
            else
            {
                angleX -= rotationStep;
            }
        }
        // Use shader program
        shader.use();

        // Set the projection matrix
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
        shader.setMatrix4("projection", projection);

        // Set the view matrix
        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        shader.setMatrix4("view", view);

        glm::vec3 position0(0.0f, 0.0f, 0.0f);
        glm::vec3 rotationAngles0(angleX, angleY, 0.0f);
        glm::vec3 orientation0(1.0f, 1.0f, 1.0f);

        VAO1.Bind(); // bind VAO just before rendering the cube.
        renderCube(shader, position0, rotationAngles0, orientation0);

        // Swap buffers and poll IO events
        glfwSwapBuffers(window);
    }

    // Clean up and exit
    shader.Delete();
    VAO1.Delete();
    VBO1.Delete();
    EBO1.Delete();
    cubeFace.Delete();
    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        switch (key)
        {
        case GLFW_KEY_U:
            if (mods & GLFW_MOD_SHIFT)
            {
                // Shift+U pressed: Rotate anti-clockwise
                if (!ifRotatingX || (ifRotatingX && targetAngleX >= angleX))
                {
                    targetAngleX = angleX - 90.0f;
                    ifRotatingX = true;
                }
            }
            else
            {
                // U pressed: Rotate clockwise
                if (!ifRotatingX || (ifRotatingX && targetAngleX <= angleX))
                {
                    targetAngleX = angleX + 90.0f;
                    ifRotatingX = true;
                }
            }
            break;
        case GLFW_KEY_Q:
            // Close the window if Q is pressed
            glfwSetWindowShouldClose(window, GL_TRUE);
            break;
        default:
            break;
        }
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
