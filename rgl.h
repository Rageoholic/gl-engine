#ifndef RGL_H
#define RGL_H
#include "glad.h"
#include "rutils/def.h"
#include "rutils/math.h"
#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct ColorRGB3f32
    {
        f32 r;
        f32 g;
        f32 b;
    } ColorRGB3f32;

    typedef struct
    {
        GLuint _id;
    } ShaderProg;

    void UseShaderProg(ShaderProg s);

    void SetUniformVec3fShaderProg(ShaderProg s, char *uniformName, Vec3f v);

    void SetUniformMat4fShaderProg(ShaderProg s, char *uniformName, const Mat4f *m);

    void SetUniformFloatShaderProg(ShaderProg s, char *uniformName, float f);

    void SetUniformIntShaderProg(ShaderProg s, char *uniformName, int i);

    ShaderProg CreateShaderProg(char *vertShaderPath, char *fragShaderPath);
#ifdef __cplusplus
}
#endif
#endif
