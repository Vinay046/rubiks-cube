#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


class Shader
{
public:
    // the program ID
    unsigned int ID;

    // constructor reads and builds the shader
    Shader(const char* vertexPath, const char* fragmentPath);
    void Delete();
    // use/activate the shader
    void use();

    // utility uniform functions
    // Add more Uniform type set functions.
    // void setUniformType(const std::string &name, uniformType value) const;
    void setMatrix4(const std::string &name, glm::mat4 value) const;

private:
    void checkCompileErrors(GLuint shader, std::string type);
};

#endif