#include "glad.h"
#include "rgl.h"
#include "rutils/debug.h"
#include "rutils/def.h"
#include <SDL.h>

#define WIDTH 1280
#define HEIGHT 720

#define VERT_SHADER_PATH "shaders/basic-render.vert"
#define FRAG_SHADER_PATH "shaders/basic-render.frag"

typedef struct Vertex
{
    Vec3f pos;
    Vec3f c;
} Vertex;

local Vertex vertices[] = {
    {{.5, .5, .5}, {1, 0, 0}},
    {{-.5, .5, .5}, {0, 1, 0}},
    {{.5, -.5, .5}, {0, 0, 1}},
    {{.5, -.5, .5}, {0, 0, 1}},
    {{-.5, .5, .5}, {0, 1, 0}},
    {{-.5, -.5, .5}, {1, 1, 1}},
    {{.5, .5, 0}, {1, 0, 0}},
    {{-.5, .5, 0}, {0, 1, 0}},
    {{.5, -.5, 0}, {0, 0, 1}},
    {{.5, -.5, 0}, {0, 0, 1}},
    {{-.5, .5, 0}, {0, 1, 0}},
    {{-.5, -.5, 0}, {1, 1, 1}}

};

f32 SDLGetAspectRatio(SDL_Window *win)
{
    int width, height;
    SDL_GetWindowSize(win, &width, &height);

    return (f32)width / (f32)height;
}

int main(int argc, char **argv)
{
    ignore argc;
    ignore argv;

    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    /* TODO: properly get multisample values */
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);

    SDL_Window *win = SDL_CreateWindow("Title", 0, 0, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);

    SDL_GLContext *c = SDL_GL_CreateContext(win);

    SDL_GL_MakeCurrent(win, c);

    if (SDL_GL_SetSwapInterval(-1) == -1)
    {
        SDL_GL_SetSwapInterval(1);
    }

    INVARIANT(gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress), "Could not load GL");

    u32 vertexBuffer;

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);

    glBindVertexArray(vertexArrayObject);

    /* Vertex position in worldspace */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, pos));
    glEnableVertexAttribArray(0);

    /* Vertex color */
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, c));
    glEnableVertexAttribArray(1);

    Mat4f proj = CreatePerspectiveMat4f(DegToRad(45), SDLGetAspectRatio(win), .1, 10);
    Mat4f view = CalcLookAtMat4f(vec3f(3, 3, 3), vec3f(0, 0, 0), vec3f(0, 0, 1));

    ShaderProg s = CreateShaderProg(VERT_SHADER_PATH, FRAG_SHADER_PATH);

    UseShaderProg(s);
    SetUniformMat4fShaderProg(s, "proj", &proj);
    SetUniformMat4fShaderProg(s, "view", &view);

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);

    glClearColor(.1, .1, .1, 1);

    bool running = true;
    f32 totalTime = 0;
    ufast32 lastTime = SDL_GetTicks();

    while (running)
    {
        /* Input and housekeeping */
        ufast32 startTime = SDL_GetTicks();
        f32 dt = (f32)(startTime - lastTime);
        totalTime += (f32)dt / 1000;
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            switch (e.type)
            {
            case SDL_QUIT:
            {
                running = false;
                break;
            }
            default:
            {
                break;
            }
            }
        }
        /* update */
        Mat4f model = RotateMat4f(&IdMat4f, totalTime * DegToRad(20), vec3f(0, 0, 1));

        /* Render */
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            /* Set relevant state */
            SetUniformMat4fShaderProg(s, "model", &model);

            glBindVertexArray(vertexArrayObject);
            UseShaderProg(s);
            glDrawArrays(GL_TRIANGLES, 0, countof(vertices));
        }

        /* End of frame housekeeping */
        SDL_GL_SwapWindow(win);
        lastTime = startTime;
    }

    glDeleteVertexArrays(1, &vertexArrayObject);

    glDeleteBuffers(1, &vertexBuffer);

    SDL_GL_DeleteContext(c);

    SDL_DestroyWindow(win);

    SDL_Quit();
}
