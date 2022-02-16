#pragma once

#include <glad/glad.h>
#include <string>

class Texture
{
public:
    // ID of texture object
    unsigned int ID;

    // texture image size
    unsigned int Width, Height;

    // texture Format
    unsigned int Internal_Format;
    unsigned int Image_Format;

    // texture configuration
    unsigned int Wrap_S;
    unsigned int Wrap_T;
    unsigned int Filter_Min;
    unsigned int Filter_Max;

    Texture();

    void Generate(unsigned int width, unsigned int height, unsigned char* data);

    void Bind() const;

    static Texture LoadTexture(const char* file, bool alpha, std::string name);

    static Texture GetTexture(std::string name);

private:
    static Texture loadTextureFromFile(const char* file, bool alpha);
};