#define _DEFAULT_SOURCE
#include "glad.h"
#include "rgl.h"
#include "rutils/debug.h"
#include "rutils/def.h"
#include <SDL.h>
#include <float.h>
#include <math.h>
#include <sys/mman.h>

#define WIDTH 1280
#define HEIGHT 720

#define KILOBYTE (1024)
#define MEGABYTE (KILOBYTE * 1024)

#define MEMSIZE (512 * MEGABYTE)

#define VERT_SHADER_PATH "shaders/basic-render.vert"
#define FRAG_SHADER_PATH "shaders/basic-render.frag"

typedef struct Vertex
{
    Vec3f pos;
    Vec3f c;
} Vertex;

local void DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                         GLsizei len, const GLchar *message, void *userParam)
{
    ignore userParam;
    ignore len;

    const char *srcStr;
    switch (source)
    {
    case GL_DEBUG_SOURCE_API:
    {
        srcStr = "API";
        break;
    }
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
    {
        srcStr = "WINDOW SYSTEM";
        break;
    }
    case GL_DEBUG_SOURCE_APPLICATION:
    {
        srcStr = "APP";
        break;
    }
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
    {
        srcStr = "SHADER COMPILER";
        break;
    }
    case GL_DEBUG_SOURCE_THIRD_PARTY:
    {
        srcStr = "THIRD PARTY";
        break;
    }
    case GL_DEBUG_SOURCE_OTHER:
    {
        srcStr = "OTHER";
        break;
    }
    }

    const char *typeStr;
    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:
    {
        typeStr = "ERROR";
        break;
    }
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    {
        typeStr = "DEPRECATED BEHAVIOR";
        break;
    }
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
    {
        typeStr = "UNDEFINED BEHAVIOR";
        break;
    }
    case GL_DEBUG_TYPE_PORTABILITY:
    {
        typeStr = "PORTABILITY";
        break;
    }
    case GL_DEBUG_TYPE_PERFORMANCE:
    {
        typeStr = "PERFORMANCE";
        break;
    }
    case GL_DEBUG_TYPE_MARKER:
    {
        typeStr = "MARKER";
        break;
    }
    case GL_DEBUG_TYPE_OTHER:
    {
        typeStr = "OTHER";
        break;
    }
    }

    const char *severityStr;
    switch (severity)
    {
    case GL_DEBUG_SEVERITY_NOTIFICATION:
    {
        severityStr = "NOTIFICATION";
        break;
    }
    case GL_DEBUG_SEVERITY_LOW:
    {
        severityStr = "LOW";
        break;
    }
    case GL_DEBUG_SEVERITY_MEDIUM:
    {
        severityStr = "MEDIUM";
        break;
    }
    case GL_DEBUG_SEVERITY_HIGH:
    {
        severityStr = "HIGH";
        break;
    }
    }
    fprintf(stderr, "GL DEBUG NOTIFICATION\nSOURCE: %s TYPE: %s, SEVERITY: %s, ID: %" PRIi32 "\n%s\n\n",
            srcStr, typeStr, severityStr, id, message);
}

local void SetProperViewport(ifast32 w, ifast32 h)
{

    f32 aspectRatio = (f32)w / (f32)h;
    f32 gameAspectRatio = 16.0f / 9.0f;

    if (F32Eq(aspectRatio, gameAspectRatio, FLT_EPSILON))
    {
        glViewport(0, 0, w, h);
    }
    else
    {
        ifast32 properX = 0;
        ifast32 properY = 0;
        ifast32 properWidth = w;
        ifast32 properHeight = h;
        if (F32Cmp(aspectRatio, gameAspectRatio, FLT_EPSILON) > 0)
        {
            //TOO WIDE
            properWidth = rintf(gameAspectRatio * (f32)h);

            properX = (w - properWidth) / 2;
        }
        else
        {
            //TOO TALL
            properHeight = rintf((f32)w / gameAspectRatio);
            properY = (h - properHeight) / 2;
        }
        glViewport(properX, properY, properWidth, properHeight);
    }
}

local void SDLResizeWindow(SDL_Window *win, ileast32 w, ileast32 h)
{
    SDL_SetWindowSize(win, w, h);
    SetProperViewport(w, h);
}

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
    {{-.5, -.5, 0}, {1, 1, 1}}};

int main(int argc, char **argv)
{
    ignore argc;
    ignore argv;

    void *gameMem = mmap(NULL, MEMSIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (gameMem == (void *)-1)
    {
        puts("Could not allocate memory");
    }

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

    glCreateBuffers(1, &vertexBuffer);

    glNamedBufferData(vertexBuffer, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);

    glBindVertexArray(vertexArrayObject);
    glBindVertexBuffer(0, vertexBuffer, 0, sizeof(Vertex));
    /* Vertex position in worldspace */
    glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, pos));
    glEnableVertexAttribArray(0);

    /* Vertex color */
    glVertexAttribFormat(1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, c));
    glEnableVertexAttribArray(1);

    glVertexAttribBinding(0, 0);
    glVertexAttribBinding(1, 0);

    Mat4f proj = CreatePerspectiveMat4f(DegToRad(45), 16.0 / 9.0, .1, 10);
    Mat4f view = CalcLookAtMat4f(vec3f(2, 2, 2), vec3f(0, 0, 0), vec3f(0, 0, 1));

    ShaderProg s = CreateShaderProg(VERT_SHADER_PATH, FRAG_SHADER_PATH);

    UseShaderProg(s);
    SetUniformMat4fShaderProg(s, "proj", &proj);
    SetUniformMat4fShaderProg(s, "view", &view);
    {
        int w, h;
        SDL_GetWindowSize(win, &w, &h);

        SetProperViewport(w, h);
    }

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);

#if defined(DEBUG) && !defined(NO_DEBUG_OUTPUT)
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback((GLDEBUGPROC)DebugCallback, NULL);
#endif

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
            case SDL_WINDOWEVENT:
            {
                SDL_WindowEvent we = e.window;
                if (we.event == SDL_WINDOWEVENT_RESIZED)
                {
                    SetProperViewport(we.data1, we.data2);
                }
                break;
            }
            case SDL_KEYDOWN:
            {
                SDL_KeyboardEvent k = e.key;
                if (!k.repeat)
                {
                    if (k.keysym.scancode == SDL_SCANCODE_F1)
                    {
                        SDLResizeWindow(win, 1136, 630);
                        puts("F1");
                    }
                    else if (k.keysym.scancode == SDL_SCANCODE_F2)
                    {
                        SDLResizeWindow(win, 1280, 720);
                    }
                    else if (k.keysym.scancode == SDL_SCANCODE_ESCAPE)
                    {
                        running = false;
                    }
                }
                break;
            }

            case SDL_KEYUP:
            {
                if (e.key.keysym.scancode == SDL_SCANCODE_F1)
                {
                    puts("F1 release");
                }
            }
            default:
            {
                break;
            }
            }
        }
        int windowWidth, windowHeight;
        SDL_GetWindowSize(win, &windowWidth, &windowHeight);

        /* Mouse calculations */
        int windowX, windowY;
        SDL_GetWindowPosition(win, &windowX, &windowY);

        int absoluteMouseX, absoluteMouseY;
        ufast32 mouseState = SDL_GetGlobalMouseState(&absoluteMouseX, &absoluteMouseY);
        ufast32 mouseLeft = SDL_BUTTON(1) & mouseState;
        ufast32 mouseRight = SDL_BUTTON(3) & mouseState;
        ufast32 mouseMid = SDL_BUTTON(2) & mouseState;

        ifast32 relativeMouseX = absoluteMouseX - windowX;
        ifast32 relativeMouseY = absoluteMouseY - windowY;
        /* update */
        Mat4f model = RotateMat4f(&IdMat4f, totalTime * DegToRad(90), vec3f(0, 0, 1));

        if (mouseLeft)
        {
            glClearColor(1, 0, 0, 1);
        }
        else if (mouseRight)
        {
            glClearColor(0, 1, 0, 1);
        }
        else if (mouseMid)
        {
            glClearColor(0, 0, 1, 1);
        }
        else
        {
            glClearColor(.1, .1, .1, 1);
        }

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
    munmap(gameMem, MEMSIZE);

    SDL_Quit();
}
