#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <shader.hpp>
#include <stb_image.hpp>

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
unsigned int VBO, VAO, EBO;

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
    glBindVertexArray(VAO);

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

    Shader shader("shaders/cube.vert", "shaders/cube.frag");
    unsigned int shaderProgram = shader.ID;

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    /* Load and create a texture */
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // Set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Load image, create texture and generate mipmaps
    int width, height, nrChannels;
    // read files
    unsigned char *data = stbi_load("resources/textures/stickerCenter.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

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
        glBindTexture(GL_TEXTURE_2D, texture);

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

        renderCube(shader, position0, rotationAngles0, orientation0);

        // Swap buffers and poll IO events
        glfwSwapBuffers(window);
    }

    // Clean up and exit
    shader.~Shader();
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
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
