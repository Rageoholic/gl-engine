#include "rgl.h"
#include "glad.h"
#include "rutils/file.h"

void UseShaderProg(ShaderProg s)
{
    glUseProgram(s._id);
}

void SetUniformVec3fShaderProg(ShaderProg s, char *uniformName, Vec3f v)
{
    glUniform3fv(glGetUniformLocation(s._id, uniformName), 1, (float *)&v);
}

void SetUniformMat4fShaderProg(ShaderProg s, char *uniformName, const Mat4f *m)
{
    glUniformMatrix4fv(glGetUniformLocation(s._id, uniformName),
                       1, GL_FALSE, (float *)m);
}
void SetUniformFloatShaderProg(ShaderProg s, char *uniformName, float f)
{
    glUniform1f(glGetUniformLocation(s._id, uniformName), f);
}

void SetUniformIntShaderProg(ShaderProg s, char *uniformName, int i)
{
    glUniform1i(glGetUniformLocation(s._id, uniformName), i);
}

ShaderProg CreateShaderProg(char *vertShaderPath, char *fragShaderPath)
{
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    {

        ssize_t vertShaderSourceSize;
        char *vertShaderSource = MapFileToROBuffer(vertShaderPath, NULL, &vertShaderSourceSize);

        glShaderSource(vertShader, 1, (const char **)&vertShaderSource, NULL);
        glCompileShader(vertShader);

        int success;
        glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char infoLog[512];
            glGetShaderInfoLog(vertShader, 512, NULL, infoLog);
            fprintf(stderr, "COMPILE ERROR IN VERT SHADER %s\n", infoLog);
        }
        UnmapMappedBuffer(vertShaderSource, vertShaderSourceSize);
    }
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    {

        ssize_t fragShaderSourceSize;
        char *fragShaderSource = MapFileToROBuffer(fragShaderPath, NULL, &fragShaderSourceSize);

        glShaderSource(fragShader, 1, (const char **)&fragShaderSource, NULL);
        glCompileShader(fragShader);

        int success;
        glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char infoLog[512];
            glGetShaderInfoLog(fragShader, 512, NULL, infoLog);
            fprintf(stderr, "COMPILE ERROR IN VERT SHADER %s\n", infoLog);
        }
        UnmapMappedBuffer(fragShaderSource, fragShaderSourceSize);
    }

    ShaderProg shaderProg = {glCreateProgram()};
    {
        glAttachShader(shaderProg._id, vertShader);
        glAttachShader(shaderProg._id, fragShader);
        glLinkProgram(shaderProg._id);
        int success;
        glGetShaderiv(shaderProg._id, GL_LINK_STATUS, &success);
        if (!success)
        {
            char infoLog[512];
            glGetProgramInfoLog(shaderProg._id, 512, NULL, infoLog);
            fprintf(stderr, "LINK ERROR IN SHADER PROG %s\n", infoLog);
        }
    }
    return shaderProg;
}
