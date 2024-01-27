#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

GLfloat WINDOW_WIDTH = 600;
GLfloat WINDOW_HEIGHT = 400;

GLFWwindow *window;

float angleX = 0.0f;
float angleY = 0.0f;
float angleZ = 0.0f;
double prevXPos = 0.0;
double prevYPos = 0.0;
bool mousePressedLeft = false;
bool mousePressedRight = false;

float rotationSpeed = 0.0f;         // Speed of rotation
float maxRotationSpeed = 2.0f;      // Maximum speed
float rotationAcceleration = 0.01f; // Acceleration (speed change per frame)
float rotationAngle = 0.0f;         // Current rotation angle
bool isRotating = true;             // Flag to control the rotation
float rotationTarget = 0.0f;        // Target rotation angle (90 degrees)
bool startRotation = false;
bool reverseRotation = false;

// glm::vec3 positions[26] = {
//     // center pieces
//     glm::vec3(0.0f, 1.0f, 0.0f),    // position0
//     glm::vec3(0.0f, -1.0f, 0.0f),   // position1
//     glm::vec3(1.0f, 0.0f, 0.0f),    // position2
//     glm::vec3(-1.0f, 0.0f, 0.0f),   // position3
//     glm::vec3(0.0f, 0.0f, -1.0f),   // position4
//     glm::vec3(0.0f, 0.0f, 1.0f),    // position5
//     // edge pieces
//     glm::vec3(0.0f, 1.0f, 1.0f),    // position6
//     glm::vec3(0.0f, 1.0f, -1.0f),   // position7
//     glm::vec3(0.0f, -1.0f, 1.0f),   // position8
//     glm::vec3(0.0f, -1.0f, -1.0f),  // position9
//     glm::vec3(1.0f, 1.0f, 0.0f),    // position10
//     glm::vec3(1.0f, -1.0f, 0.0f),   // position11
//     glm::vec3(-1.0f, 1.0f, 0.0f),   // position12
//     glm::vec3(-1.0f, -1.0f, 0.0f),  // position13
//     glm::vec3(1.0f, 0.0f, 1.0f),    // position14
//     glm::vec3(-1.0f, 0.0f, 1.0f),   // position15
//     glm::vec3(1.0f, 0.0f, -1.0f),   // position16
//     glm::vec3(-1.0f, 0.0f, -1.0f),  // position17
//     // Corner pieces
//     glm::vec3(1.0f, 1.0f, -1.0f),   // position18
//     glm::vec3(1.0f, 1.0f, 1.0f),    // position19
//     glm::vec3(-1.0f, 1.0f, -1.0f),  // position20
//     glm::vec3(-1.0f, 1.0f, 1.0f),   // position21
//     glm::vec3(1.0f, -1.0f, -1.0f),  // position22
//     glm::vec3(1.0f, -1.0f, 1.0f),   // position23
//     glm::vec3(-1.0f, -1.0f, -1.0f), // position24
//     glm::vec3(-1.0f, -1.0f, 1.0f)   // position25
// };

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

// void performRotation(float& angle, float& speed, float acceleration, float maxSpeed, float targetAngle, bool& rotating) {
//     if (!startRotation && rotating) return;

//     // Accelerate rotation speed until it reaches the maximum
//     if (speed < maxSpeed) {
//         speed += acceleration;
//     } else {
//         speed = maxSpeed; // Ensure speed does not exceed the maximum
//     }

//     // Perform the rotation
//     angle += speed;

//     // Check if the rotation has reached or exceeded the target angle
//     if (angle >= targetAngle) {
//         angle = targetAngle; // Set the angle to exactly the target angle
//         speed = 0.0f; // Reset the speed
//         rotating = false; // Mark that the rotation is finished
//         startRotation = false; // Reset the start flag
//     } else {
//         rotating = true; // Mark that the cube is currently rotating
//     }
// }

void performRotation(float& angle, float& speed, float acceleration, float maxSpeed, float targetAngle, bool& rotating, bool reverse) {
    if ((!startRotation && rotating) || (rotating && angle == (reverse ? -targetAngle : targetAngle))) return;

    // Accelerate rotation speed until it reaches the maximum
    if (speed < maxSpeed) {
        speed += acceleration;
    } else {
        speed = maxSpeed; // Ensure speed does not exceed the maximum
    }

    // Perform the rotation
    angle += reverse ? -speed : speed; // Reverse the direction if needed

    // Check if the rotation has reached or exceeded the target angle
    if (reverse ? (angle <= -targetAngle) : (angle >= targetAngle)) {
        angle = reverse ? -targetAngle : targetAngle; // Set the angle to exactly the target angle
        speed = 0.0f; // Reset the speed
        rotating = false; // Mark that the rotation is finished
        startRotation = false; // Reset the start flag
    } else {
        rotating = true; // Mark that the cube is currently rotating
    }
}


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

        // Perform rotation
        performRotation(rotationAngle, rotationSpeed, rotationAcceleration, maxRotationSpeed, rotationTarget, isRotating, reverseRotation);
        glm::vec3 orientation(1.0f, 1.0f, 1.0f);
        glm::vec3 rotationAngles(angleX, rotationAngle + angleY, angleZ);
        glm::vec3 position(0.0f, 0.0f, 0.0f);
        renderCube(shaderProgram, position, rotationAngles, orientation);

        std::cout << rotationTarget << "." << std::endl;
        // glm::vec3 rotationAngles[26];
        // for (int i = 0; i < 26; ++i)
        // {
        //     // Update rotation angles dynamically
        //     if (i == 0 || i == 6 || i == 7 || i == 10 || i == 12 || i == 18 || i == 19 || i == 20 || i == 21)
        //     {
        //         rotationAngles[i] = glm::vec3(angleX, (GLfloat)glfwGetTime() * 30, 0.0f); // Rotating continuously
        //     }
        //     else
        //     {
        //         rotationAngles[i] = glm::vec3(angleX, angleY, 0.0f); // Static rotation
        //     }

        //     glm::vec3 orientation(1.0f, 1.0f, 1.0f); // Assuming uniform scaling
        //     renderCube(shaderProgram, positions[i], rotationAngles[i], orientation);
        // }

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

    // Start the rotation when the user presses the 'R' key
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        rotationTarget += 90.0f;
        startRotation = true;
        reverseRotation = true;
    }

    // Start the rotation when the user presses the 'R' key
    if (key == GLFW_KEY_E && action == GLFW_PRESS)
    {
        rotationTarget -= 90.0f;
        startRotation = true;
        reverseRotation = false;
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
