#pragma once

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
    unsigned int ID;

    Shader() { }

    Shader& Use();

    void    Compile(const char* vertexSource, const char* fragmentSource);

    // utility functions
    void    SetInteger(const char* name, int value);
    void    SetVector3f(const char* name, const glm::vec3& value);
    void    SetMatrix4(const char* name, const glm::mat4& matrix);

    static Shader    LoadShader(const char* vShaderFile, const char* fShaderFile, std::string name);

    static Shader    GetShader(std::string name);

private:
    void    checkCompileErrors(unsigned int object, std::string type);

    static Shader    loadShaderFromFile(const char* vShaderFile, const char* fShaderFile);
};