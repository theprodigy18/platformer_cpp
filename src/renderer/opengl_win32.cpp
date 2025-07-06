#include "renderer/opengl.hpp"
#include "renderer/render_interface.hpp"
#include "core/input.hpp"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#include <gl/GL.h>
#include "opengl/wglext.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

namespace drop::renderer
{
    namespace
    {
        HMODULE g_openglDLL {nullptr};

#define LOAD_GL_FUNCTION(type, name)                      \
    name = (type) wglGetProcAddress(#name);               \
    if (!name)                                            \
    {                                                     \
        name = (type) GetProcAddress(g_openglDLL, #name); \
    }                                                     \
    D_ASSERT(name, "Failed to load OpenGL function: %s", #name);

#pragma region OpenGL Functions
        static PFNGLCREATEPROGRAMPROC           glCreateProgram;
        static PFNGLDELETETEXTURESPROC          glDeleteTextures;
        static PFNGLGENTEXTURESPROC             glGenTextures;
        static PFNGLBINDTEXTUREPROC             glBindTexture;
        static PFNGLDRAWBUFFERPROC              glDrawBuffer;
        static PFNGLDRAWARRAYSPROC              glDrawArrays;
        static PFNGLCREATESHADERPROC            glCreateShader;
        static PFNGLGETUNIFORMLOCATIONPROC      glGetUniformLocation;
        static PFNGLUNIFORM1FPROC               glUniform1f;
        static PFNGLUNIFORM2FVPROC              glUniform2fv;
        static PFNGLUNIFORM3FVPROC              glUniform3fv;
        static PFNGLUNIFORM1IPROC               glUniform1i;
        static PFNGLUNIFORMMATRIX4FVPROC        glUniformMatrix4fv;
        static PFNGLVERTEXATTRIBDIVISORPROC     glVertexAttribDivisor;
        static PFNGLACTIVETEXTUREPROC           glActiveTexture;
        static PFNGLBUFFERSUBDATAPROC           glBufferSubData;
        static PFNGLDRAWARRAYSINSTANCEDPROC     glDrawArraysInstanced;
        static PFNGLBINDFRAMEBUFFERPROC         glBindFramebuffer;
        static PFNGLCHECKFRAMEBUFFERSTATUSPROC  glCheckFramebufferStatus;
        static PFNGLGENFRAMEBUFFERSPROC         glGenFramebuffers;
        static PFNGLFRAMEBUFFERTEXTURE2DPROC    glFramebufferTexture2D;
        static PFNGLDRAWBUFFERSPROC             glDrawBuffers;
        static PFNGLDELETEFRAMEBUFFERSPROC      glDeleteFramebuffers;
        static PFNGLBLENDFUNCIPROC              glBlendFunci;
        static PFNGLBLENDEQUATIONPROC           glBlendEquation;
        static PFNGLCLEARBUFFERFVPROC           glClearBufferfv;
        static PFNGLSHADERSOURCEPROC            glShaderSource;
        static PFNGLCOMPILESHADERPROC           glCompileShader;
        static PFNGLGETSHADERIVPROC             glGetShaderiv;
        static PFNGLGETSHADERINFOLOGPROC        glGetShaderInfoLog;
        static PFNGLATTACHSHADERPROC            glAttachShader;
        static PFNGLLINKPROGRAMPROC             glLinkProgram;
        static PFNGLVALIDATEPROGRAMPROC         glValidateProgram;
        static PFNGLGETPROGRAMIVPROC            glGetProgramiv;
        static PFNGLGETPROGRAMINFOLOGPROC       glGetProgramInfoLog;
        static PFNGLGENBUFFERSPROC              glGenBuffers;
        static PFNGLGENVERTEXARRAYSPROC         glGenVertexArrays;
        static PFNGLGETATTRIBLOCATIONPROC       glGetAttribLocation;
        static PFNGLBINDVERTEXARRAYPROC         glBindVertexArray;
        static PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
        static PFNGLVERTEXATTRIBPOINTERPROC     glVertexAttribPointer;
        static PFNGLBINDBUFFERPROC              glBindBuffer;
        static PFNGLBINDBUFFERBASEPROC          glBindBufferBase;
        static PFNGLBUFFERDATAPROC              glBufferData;
        static PFNGLGETVERTEXATTRIBPOINTERVPROC glGetVertexAttribPointerv;
        static PFNGLUSEPROGRAMPROC              glUseProgram;
        static PFNGLDELETEVERTEXARRAYSPROC      glDeleteVertexArrays;
        static PFNGLDELETEBUFFERSPROC           glDeleteBuffers;
        static PFNGLDELETEPROGRAMPROC           glDeleteProgram;
        static PFNGLDETACHSHADERPROC            glDetachShader;
        static PFNGLDELETESHADERPROC            glDeleteShader;
        static PFNGLDRAWELEMENTSINSTANCEDPROC   glDrawElementsInstanced;
        static PFNGLGENERATEMIPMAPPROC          glGenerateMipmap;
        static PFNGLDEBUGMESSAGECALLBACKPROC    glDebugMessageCallback;
#pragma endregion

        void LoadOpenGLFunctions()
        {
            g_openglDLL = LoadLibraryA("opengl32.dll");
            D_ASSERT(g_openglDLL, "Failed to load opengl32.dll");
            LOAD_GL_FUNCTION(PFNGLCREATEPROGRAMPROC, glCreateProgram);
            LOAD_GL_FUNCTION(PFNGLDELETETEXTURESPROC, glDeleteTextures);
            LOAD_GL_FUNCTION(PFNGLGENTEXTURESPROC, glGenTextures);
            LOAD_GL_FUNCTION(PFNGLBINDTEXTUREPROC, glBindTexture);
            LOAD_GL_FUNCTION(PFNGLDRAWBUFFERPROC, glDrawBuffer);
            LOAD_GL_FUNCTION(PFNGLDRAWARRAYSPROC, glDrawArrays);
            LOAD_GL_FUNCTION(PFNGLCREATESHADERPROC, glCreateShader);
            LOAD_GL_FUNCTION(PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation);
            LOAD_GL_FUNCTION(PFNGLUNIFORM1FPROC, glUniform1f);
            LOAD_GL_FUNCTION(PFNGLUNIFORM2FVPROC, glUniform2fv);
            LOAD_GL_FUNCTION(PFNGLUNIFORM3FVPROC, glUniform3fv);
            LOAD_GL_FUNCTION(PFNGLUNIFORM1IPROC, glUniform1i);
            LOAD_GL_FUNCTION(PFNGLUNIFORMMATRIX4FVPROC, glUniformMatrix4fv);
            LOAD_GL_FUNCTION(PFNGLVERTEXATTRIBDIVISORPROC, glVertexAttribDivisor);
            LOAD_GL_FUNCTION(PFNGLACTIVETEXTUREPROC, glActiveTexture);
            LOAD_GL_FUNCTION(PFNGLBUFFERSUBDATAPROC, glBufferSubData);
            LOAD_GL_FUNCTION(PFNGLDRAWARRAYSINSTANCEDPROC, glDrawArraysInstanced);
            LOAD_GL_FUNCTION(PFNGLBINDFRAMEBUFFERPROC, glBindFramebuffer);
            LOAD_GL_FUNCTION(PFNGLCHECKFRAMEBUFFERSTATUSPROC, glCheckFramebufferStatus);
            LOAD_GL_FUNCTION(PFNGLGENFRAMEBUFFERSPROC, glGenFramebuffers);
            LOAD_GL_FUNCTION(PFNGLFRAMEBUFFERTEXTURE2DPROC, glFramebufferTexture2D);
            LOAD_GL_FUNCTION(PFNGLDRAWBUFFERSPROC, glDrawBuffers);
            LOAD_GL_FUNCTION(PFNGLDELETEFRAMEBUFFERSPROC, glDeleteFramebuffers);
            LOAD_GL_FUNCTION(PFNGLBLENDFUNCIPROC, glBlendFunci);
            LOAD_GL_FUNCTION(PFNGLBLENDEQUATIONPROC, glBlendEquation);
            LOAD_GL_FUNCTION(PFNGLCLEARBUFFERFVPROC, glClearBufferfv);
            LOAD_GL_FUNCTION(PFNGLSHADERSOURCEPROC, glShaderSource);
            LOAD_GL_FUNCTION(PFNGLCOMPILESHADERPROC, glCompileShader);
            LOAD_GL_FUNCTION(PFNGLGETSHADERIVPROC, glGetShaderiv);
            LOAD_GL_FUNCTION(PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog);
            LOAD_GL_FUNCTION(PFNGLATTACHSHADERPROC, glAttachShader);
            LOAD_GL_FUNCTION(PFNGLLINKPROGRAMPROC, glLinkProgram);
            LOAD_GL_FUNCTION(PFNGLVALIDATEPROGRAMPROC, glValidateProgram);
            LOAD_GL_FUNCTION(PFNGLGETPROGRAMIVPROC, glGetProgramiv);
            LOAD_GL_FUNCTION(PFNGLGETPROGRAMINFOLOGPROC, glGetProgramInfoLog);
            LOAD_GL_FUNCTION(PFNGLGENBUFFERSPROC, glGenBuffers);
            LOAD_GL_FUNCTION(PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays);
            LOAD_GL_FUNCTION(PFNGLGETATTRIBLOCATIONPROC, glGetAttribLocation);
            LOAD_GL_FUNCTION(PFNGLBINDVERTEXARRAYPROC, glBindVertexArray);
            LOAD_GL_FUNCTION(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray);
            LOAD_GL_FUNCTION(PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer);
            LOAD_GL_FUNCTION(PFNGLBINDBUFFERPROC, glBindBuffer);
            LOAD_GL_FUNCTION(PFNGLBINDBUFFERBASEPROC, glBindBufferBase);
            LOAD_GL_FUNCTION(PFNGLBUFFERDATAPROC, glBufferData);
            LOAD_GL_FUNCTION(PFNGLGETVERTEXATTRIBPOINTERVPROC, glGetVertexAttribPointerv);
            LOAD_GL_FUNCTION(PFNGLUSEPROGRAMPROC, glUseProgram);
            LOAD_GL_FUNCTION(PFNGLDELETEVERTEXARRAYSPROC, glDeleteVertexArrays);
            LOAD_GL_FUNCTION(PFNGLDELETEBUFFERSPROC, glDeleteBuffers);
            LOAD_GL_FUNCTION(PFNGLDELETEPROGRAMPROC, glDeleteProgram);
            LOAD_GL_FUNCTION(PFNGLDETACHSHADERPROC, glDetachShader);
            LOAD_GL_FUNCTION(PFNGLDELETESHADERPROC, glDeleteShader);
            LOAD_GL_FUNCTION(PFNGLDRAWELEMENTSINSTANCEDPROC, glDrawElementsInstanced);
            LOAD_GL_FUNCTION(PFNGLGENERATEMIPMAPPROC, glGenerateMipmap);
            LOAD_GL_FUNCTION(PFNGLDEBUGMESSAGECALLBACKPROC, glDebugMessageCallback);
        }

        PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB {nullptr};
        PFNWGLCHOOSEPIXELFORMATARBPROC    wglChoosePixelFormatARB {nullptr};

        constexpr char* TEXTURE_PATH {"assets/textures/TEXTURE_ATLAS.png"};

        HGLRC  g_context {nullptr};
        GLuint g_programID {0};
        GLuint g_vao {0};
        GLuint g_textureID {0};
        GLuint g_transformSBOID {0};
        GLuint g_screenSizeID {0};

        void CALLBACK GLDebugCallback(GLenum source, GLenum type, GLuint id,
                                      GLenum severity, GLsizei length, const GLchar* message,
                                      const void* userParam)
        {
            if (severity == GL_DEBUG_SEVERITY_LOW ||
                severity == GL_DEBUG_SEVERITY_MEDIUM ||
                severity == GL_DEBUG_SEVERITY_HIGH)
            {
                D_ASSERT(false, "OpenGL error: %s", message);
            }
            else
            {
                D_TRACE((char*) message);
            }
        }
    }

    bool GLInit()
    {
        HWND hwnd {CreateWindowExW(
            0,
            L"Static",
            L"Dummy",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            50,
            50,
            nullptr,
            nullptr,
            GetModuleHandleW(nullptr),
            nullptr)};
        if (!hwnd)
        {
            D_ASSERT(false, "Failed to create dummy window.");
            return false;
        }
        TRACK_LEAK_ALLOC(hwnd, LeakType::HANDLE, "Dummy window.");

        HDC hdc {GetDC(hwnd)};
        if (!hdc)
        {
            D_ASSERT(false, "Failed to get dummy window DC.");
            return false;
        }
        TRACK_LEAK_ALLOC(hdc, LeakType::HANDLE, "Dummy window DC.");

        PIXELFORMATDESCRIPTOR pfd {};
        pfd.nSize        = sizeof(PIXELFORMATDESCRIPTOR);
        pfd.nVersion     = 1;
        pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.iPixelType   = PFD_TYPE_RGBA;
        pfd.cColorBits   = 32;
        pfd.cDepthBits   = 24;
        pfd.cStencilBits = 8;

        i32 pixelFormat {ChoosePixelFormat(hdc, &pfd)};
        if (!pixelFormat)
        {
            D_ASSERT(false, "Failed to choose pixel format.");
            return false;
        }

        if (!SetPixelFormat(hdc, pixelFormat, &pfd))
        {
            D_ASSERT(false, "Failed to set pixel format.");
            return false;
        }

        HGLRC hglrc {wglCreateContext(hdc)};
        if (!hglrc)
        {
            D_ASSERT(false, "Failed to create OpenGL context.");
            return false;
        }
        TRACK_LEAK_ALLOC(hglrc, LeakType::OPENGL, "OpenGL dummy context.");

        if (!wglMakeCurrent(hdc, hglrc))
        {
            D_ASSERT(false, "Failed to make OpenGL context current.");
            return false;
        }

        // Load OpenGL functions.
        LoadOpenGLFunctions();
        LOAD_GL_FUNCTION(PFNWGLCREATECONTEXTATTRIBSARBPROC, wglCreateContextAttribsARB);
        LOAD_GL_FUNCTION(PFNWGLCHOOSEPIXELFORMATARBPROC, wglChoosePixelFormatARB);

        // Enable debug messages.
        glDebugMessageCallback(&GLDebugCallback, nullptr);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glEnable(GL_DEBUG_OUTPUT);

        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(hglrc);
        TRACK_LEAK_FREE(hglrc);
        ReleaseDC(hwnd, hdc);
        TRACK_LEAK_FREE(hdc);
        DestroyWindow(hwnd);
        TRACK_LEAK_FREE(hwnd);

        D_TRACE("OpenGL initialized!");

        return true;
    }

    bool GLCreateContext(platform::WindowInfo info, utils::BumpAllocator* transientStorage)
    {
        HWND hwnd {(HWND) info.handle};
        HDC  hdc {(HDC) info.dc};

        const i32 pixelAttribs[] {
            WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
            WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
            WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
            WGL_SWAP_METHOD_ARB, WGL_SWAP_COPY_ARB,
            WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
            WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
            WGL_COLOR_BITS_ARB, 32,
            WGL_DEPTH_BITS_ARB, 24,
            WGL_ALPHA_BITS_ARB, 8,
            0}; // Last entry must be 0 to terminate the list.

        u32 numPixelFormats {0};
        i32 pixelFormat {0};
        if (!wglChoosePixelFormatARB(hdc, pixelAttribs, nullptr, 1, &pixelFormat, &numPixelFormats))
        {
            D_ASSERT(false, "Failed to choose pixel format.");
            return false;
        }

        PIXELFORMATDESCRIPTOR pfd {};
        if (!DescribePixelFormat(hdc, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd))
        {
            D_ASSERT(false, "Failed to describe pixel format.");
            return false;
        }

        if (!SetPixelFormat(hdc, pixelFormat, &pfd))
        {
            D_ASSERT(false, "Failed to set pixel format.");
            return false;
        }

        const i32 contextAttribs[] {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
            WGL_CONTEXT_MINOR_VERSION_ARB, 3,
            WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
            0}; // Last entry must be 0 to terminate the list.

        g_context = wglCreateContextAttribsARB(hdc, nullptr, contextAttribs);
        if (!g_context)
        {
            D_ASSERT(false, "Failed to create OpenGL context.");
            return false;
        }
        TRACK_LEAK_ALLOC(g_context, LeakType::OPENGL, "OpenGL context.");

        if (!wglMakeCurrent(hdc, g_context))
        {
            D_ASSERT(false, "Failed to make OpenGL context current.");
            return false;
        }

        // Initialize shaders.
        GLuint vertShaderID {glCreateShader(GL_VERTEX_SHADER)};
        GLuint fragShaderID {glCreateShader(GL_FRAGMENT_SHADER)};

        i32   fileSize {0};
        char* vertShader {utils::ReadFile("assets/shaders/quad.vert", transientStorage, &fileSize)};
        char* fragShader {utils::ReadFile("assets/shaders/quad.frag", transientStorage, &fileSize)};
        if (!vertShader || !fragShader)
        {
            D_ASSERT(false, "Failed to read shaders.");
            return false;
        }

        glShaderSource(vertShaderID, 1, &vertShader, 0);
        glShaderSource(fragShaderID, 1, &fragShader, 0);

        glCompileShader(vertShaderID);
        glCompileShader(fragShaderID);

        // Test the vertex shader.
        {
            i32  success {0};
            char shaderLog[2048] {};

            glGetShaderiv(vertShaderID, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(vertShaderID, 2048, nullptr, shaderLog);
                D_ASSERT(false, "Failed to compile vertex shader: %s", shaderLog);
                return false;
            }
        }
        // Test the fragment shader.
        {
            i32  success {0};
            char shaderLog[2048] {};

            glGetShaderiv(fragShaderID, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(fragShaderID, 2048, nullptr, shaderLog);
                D_ASSERT(false, "Failed to compile fragment shader: %s", shaderLog);
                return false;
            }
        }

        // Create new program.
        g_programID = glCreateProgram();
        glAttachShader(g_programID, vertShaderID);
        glAttachShader(g_programID, fragShaderID);
        glLinkProgram(g_programID);
        TRACK_LEAK_ALLOC(&g_programID, LeakType::OPENGL, "OpenGL program");

        // Delete shaders.
        glDetachShader(g_programID, vertShaderID);
        glDetachShader(g_programID, fragShaderID);
        glDeleteShader(vertShaderID);
        glDeleteShader(fragShaderID);

        // Create VAO.
        glGenVertexArrays(1, &g_vao);
        glBindVertexArray(g_vao);
        TRACK_LEAK_ALLOC(&g_vao, LeakType::OPENGL, "OpenGL VAO");

        // Texture loading using STBI.
        {
            i32   width {0}, height {0}, channels {0};
            char* data {(char*) stbi_load(TEXTURE_PATH, &width, &height, &channels, 4)};
            if (!data)
            {
                D_ASSERT(false, "Failed to load texture.");
                return false;
            }
            TRACK_LEAK_ALLOC(data, LeakType::CUSTOM, "Texture data");

            glGenTextures(1, &g_textureID);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, g_textureID);
            TRACK_LEAK_ALLOC(&g_textureID, LeakType::OPENGL, "OpenGL texture");

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

            stbi_image_free(data);
            TRACK_LEAK_FREE(data);
        }

        // Transform Storage Buffer.
        {
            glGenBuffers(1, &g_transformSBOID);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, g_transformSBOID);
            glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Transform) * MAX_TRANSFORMS, g_renderData->transforms, GL_DYNAMIC_DRAW);
            TRACK_LEAK_ALLOC(&g_transformSBOID, LeakType::OPENGL, "OpenGL transform buffer");
        }

        // Uniforms.
        {
            g_screenSizeID = glGetUniformLocation(g_programID, "screenSize");
        }

        glEnable(GL_FRAMEBUFFER_SRGB);
        glDisable(0x809D); // Disable multisampling.

        // Enable depth testing.
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_GREATER);

        glUseProgram(g_programID);

        UpdateWindow(hwnd);

        D_TRACE("OpenGL context created!");

        return true;
    }

    void GLRenderFrame()
    {
        glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
        glClearDepth(0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glViewport(0, 0, core::g_input->screenSizeX, core::g_input->screenSizeY);

        // Copy screen size to the GPU.
        math::Vec2 screenSize {(f32) core::g_input->screenSizeX, (f32) core::g_input->screenSizeY};
        glUniform2fv(g_screenSizeID, 1, &screenSize.x);

        // Opaque objects.
        {
            // Copy transform to the GPU.
            glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(Transform) * g_renderData->transformCount, g_renderData->transforms);
            glDrawArraysInstanced(GL_TRIANGLES, 0, 6, g_renderData->transformCount);

            // Reset transform count.
            g_renderData->transformCount = 0;
        }
    }

    void GLDestroyContext()
    {
        glUseProgram(0);
        glDeleteProgram(g_programID);
        TRACK_LEAK_FREE(&g_programID);
        glDeleteVertexArrays(1, &g_vao);
        TRACK_LEAK_FREE(&g_vao);
        glDeleteTextures(1, &g_textureID);
        TRACK_LEAK_FREE(&g_textureID);
        glDeleteBuffers(1, &g_transformSBOID);
        TRACK_LEAK_FREE(&g_transformSBOID);

        g_programID      = 0;
        g_vao            = 0;
        g_textureID      = 0;
        g_transformSBOID = 0;

        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(g_context);
        TRACK_LEAK_FREE(g_context);

        g_context = nullptr;

        D_TRACE("OpenGL context destroyed!");
    }

    void GLShutdown()
    {
        FreeLibrary(g_openglDLL);
        g_openglDLL = nullptr;
        D_TRACE("OpenGL shutdown!");
    }
} // namespace drop::renderer
