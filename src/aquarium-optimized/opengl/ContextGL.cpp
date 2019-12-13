//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// ContextGL.cpp: Implements accessing functions to the graphics API of OpenGL.

#include "common/AQUARIUM_ASSERT.h"

#include <algorithm>
#include <iostream>
#include <sstream>

#include "BufferGL.h"
#include "ContextGL.h"
#include "FishModelGL.h"
#include "GenericModelGL.h"
#include "InnerModelGL.h"
#include "OutsideModelGL.h"
#include "ProgramGL.h"
#include "SeaweedModelGL.h"
#include "TextureGL.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#ifdef EGL_EGL_PROTOTYPES
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif

ContextGL::ContextGL(BACKENDTYPE backendType) : mWindow(nullptr)
{
    initAvailableToggleBitset(backendType);
}

ContextGL::~ContextGL()
{
    delete mResourceHelper;
    destoryImgUI();
}

bool ContextGL::initialize(BACKENDTYPE backend,
                           const std::bitset<static_cast<size_t>(TOGGLE::TOGGLEMAX)> &toggleBitset,
                           int windowWidth,
                           int windowHeight)
{
    // initialise GLFW
    if (!glfwInit())
    {
        std::cout << "Failed to initialise GLFW" << std::endl;
        return false;
    }

#ifdef GL_GLEXT_PROTOTYPES
    // TODO(yizhou) : Enable msaa in angle. Render into a multisample Texture and then blit to a
    // none multisample texture.
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    mResourceHelper = new ResourceHelper("opengl", std::string("100"), backend);
#else
    if (toggleBitset.test(static_cast<size_t>(TOGGLE::ENABLEMSAAx4)))
    {
        glfwWindowHint(GLFW_SAMPLES, 4);
    }

    mResourceHelper = new ResourceHelper("opengl", "450", backend);

#ifdef __APPLE__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    mGLSLVersion = "#version 410";
#elif _WIN32 || __linux__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    mGLSLVersion = "#version 450";
#endif
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWmonitor *pMonitor   = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(pMonitor);
    mClientWidth            = mode->width;
    mClientHeight           = mode->height;

    setWindowSize(windowWidth, windowHeight);

    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
    if (toggleBitset.test(static_cast<size_t>(TOGGLE::ENABLEFULLSCREENMODE)))
    {
        mWindow = glfwCreateWindow(mClientWidth, mClientHeight, "Aquarium", pMonitor, nullptr);
    }
    else
    {
        mWindow = glfwCreateWindow(mClientWidth, mClientHeight, "Aquarium", nullptr, nullptr);
    }

    if (mWindow == nullptr)
    {
        std::cout << "Failed to open GLFW window." << std::endl;
        glfwTerminate();
        return false;
    }

    setWindowTitle("Aquarium");
    glfwSetFramebufferSizeCallback(mWindow, framebufferResizeCallback);
    glfwSetWindowUserPointer(mWindow, this);

#ifndef GL_GLES_PROTOTYPES 
    glfwWindowHint(GLFW_DECORATED, GL_FALSE);
    glfwMakeContextCurrent(mWindow);
#else
    mGLSLVersion = "#version 300 es";

    std::vector<EGLAttrib> display_attribs;

    display_attribs.push_back(EGL_PLATFORM_ANGLE_TYPE_ANGLE);
    //display_attribs.push_back(EGL_PLATFORM_ANGLE_TYPE_OPENGL_ANGLE);
    display_attribs.push_back(EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE);
    display_attribs.push_back(EGL_PLATFORM_ANGLE_MAX_VERSION_MAJOR_ANGLE);
    display_attribs.push_back(-1);
    display_attribs.push_back(EGL_PLATFORM_ANGLE_MAX_VERSION_MINOR_ANGLE);
    display_attribs.push_back(-1);
    display_attribs.push_back(EGL_PLATFORM_ANGLE_DEVICE_TYPE_ANGLE);
    display_attribs.push_back(EGL_PLATFORM_ANGLE_DEVICE_TYPE_HARDWARE_ANGLE);
    display_attribs.push_back(EGL_NONE);

    HWND hwnd = glfwGetWin32Window(mWindow);
    mDisplay = eglGetPlatformDisplay(EGL_PLATFORM_ANGLE_ANGLE,
        reinterpret_cast<void *>(GetDC(hwnd)),
        &display_attribs[0]);
    if (mDisplay == EGL_NO_DISPLAY) {
        std::cout << "EGL display query failed with error " << std::endl;
    }
    GLint mEGLMajorVersion = 0;
    GLint mEGLMinorVersion = 0;
    if (eglInitialize(mDisplay, &mEGLMajorVersion, &mEGLMinorVersion) == EGL_FALSE)
    {
        return false;
    }

    const char *displayExtensions = eglQueryString(mDisplay, EGL_EXTENSIONS);

    std::vector<EGLint> configAttributes = {
        EGL_RED_SIZE,       8,
        EGL_GREEN_SIZE,     8,
        EGL_BLUE_SIZE,      8,
        EGL_ALPHA_SIZE,     8,
        EGL_DEPTH_SIZE,     24,
        EGL_STENCIL_SIZE,   8,
        EGL_SAMPLE_BUFFERS, 0,
        EGL_SAMPLES,        EGL_DONT_CARE
    };

    // Add dynamic attributes
    bool hasPixelFormatFloat = strstr(displayExtensions, "EGL_EXT_pixel_format_float") != nullptr;
    if (!hasPixelFormatFloat)
    {
        return false;
    }
    if (hasPixelFormatFloat)
    {
        configAttributes.push_back(EGL_COLOR_COMPONENT_TYPE_EXT);
        configAttributes.push_back(EGL_COLOR_COMPONENT_TYPE_FIXED_EXT);
    }

    // Finish the attribute list
    configAttributes.push_back(EGL_NONE);

    if (!FindEGLConfig(mDisplay, configAttributes.data(), &mConfig))
    {
        std::cout << "Could not find a suitable EGL config!" << std::endl;
        return false;
    }

    GLint mRedBits, mGreenBits, mBlueBits, mSamples, mAlphaBits, mDepthBits, mStencilBits;
    eglGetConfigAttrib(mDisplay, mConfig, EGL_RED_SIZE, &mRedBits);
    eglGetConfigAttrib(mDisplay, mConfig, EGL_GREEN_SIZE, &mGreenBits);
    eglGetConfigAttrib(mDisplay, mConfig, EGL_BLUE_SIZE, &mBlueBits);
    eglGetConfigAttrib(mDisplay, mConfig, EGL_ALPHA_SIZE, &mAlphaBits);
    eglGetConfigAttrib(mDisplay, mConfig, EGL_DEPTH_SIZE, &mDepthBits);
    eglGetConfigAttrib(mDisplay, mConfig, EGL_STENCIL_SIZE, &mStencilBits);
    eglGetConfigAttrib(mDisplay, mConfig, EGL_SAMPLES, &mSamples);

    std::vector<EGLint> surfaceAttributes;
    if (strstr(displayExtensions, "EGL_NV_post_sub_buffer") != nullptr)
    {
        surfaceAttributes.push_back(EGL_POST_SUB_BUFFER_SUPPORTED_NV);
        surfaceAttributes.push_back(EGL_TRUE);
    }

    surfaceAttributes.push_back(EGL_NONE);

    mSurface = eglCreateWindowSurface(mDisplay, mConfig, reinterpret_cast<EGLNativeWindowType>(hwnd), &surfaceAttributes[0]);

    if (eglGetError() != EGL_SUCCESS)
    {
        return false;
    }
    ASSERT(mSurface != EGL_NO_SURFACE);

    mContext = createContext(EGL_NO_CONTEXT);
    if (mContext == EGL_NO_CONTEXT)
    {
        return false;
    }

    eglMakeCurrent(mDisplay, mSurface, mSurface, mContext);
    if (eglGetError() != EGL_SUCCESS)
    {
        return false;
    }

    eglSwapInterval(mDisplay, 0);

#endif

    // Set the window full screen
    // glfwSetWindowPos(window, 0, 0);

    #ifndef EGL_EGL_PROTOTYPES
    if (!gladLoadGL())
    {
        std::cout << "Something went wrong!" << std::endl;
        exit(-1);
    }
    #endif

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(mWindow, true);
    ImGui_ImplOpenGL3_Init(mGLSLVersion.c_str());

    std::string renderer((const char *)glGetString(GL_RENDERER));
    size_t index = renderer.find("/");
    renderer     = renderer.substr(0, index);
    std::cout << renderer << std::endl;
    mResourceHelper->setRenderer(renderer);

    return true;
}

#ifdef GL_GLEXT_PROTOTYPES
EGLContext ContextGL::createContext(EGLContext share) const
{
    const char *displayExtensions = eglQueryString(mDisplay, EGL_EXTENSIONS);

    // EGL_KHR_create_context is required to request a ES3+ context.
    bool hasKHRCreateContext = strstr(displayExtensions, "EGL_KHR_create_context") != nullptr;

    eglBindAPI(EGL_OPENGL_ES_API);
    if (eglGetError() != EGL_SUCCESS)
    {
        return EGL_NO_CONTEXT;
    }

    std::vector<EGLint> contextAttributes;
    if (hasKHRCreateContext)
    {
        contextAttributes.push_back(EGL_CONTEXT_MAJOR_VERSION_KHR);
        contextAttributes.push_back(3);

        contextAttributes.push_back(EGL_CONTEXT_MINOR_VERSION_KHR);
        contextAttributes.push_back(0);

        contextAttributes.push_back(EGL_CONTEXT_OPENGL_DEBUG);
        contextAttributes.push_back(EGL_TRUE);

        contextAttributes.push_back(EGL_CONTEXT_OPENGL_NO_ERROR_KHR);
        contextAttributes.push_back(EGL_TRUE);
    }
    contextAttributes.push_back(EGL_NONE);

    EGLContext context = eglCreateContext(mDisplay, mConfig, nullptr, &contextAttributes[0]);
    if (eglGetError() != EGL_SUCCESS)
    {
        return EGL_NO_CONTEXT;
    }

    return context;
}

EGLBoolean ContextGL::FindEGLConfig(EGLDisplay dpy, const EGLint *attrib_list, EGLConfig *config)
{
    EGLint numConfigs = 0;
    eglGetConfigs(dpy, nullptr, 0, &numConfigs);
    std::vector<EGLConfig> allConfigs(numConfigs);
    eglGetConfigs(dpy, allConfigs.data(), static_cast<EGLint>(allConfigs.size()), &numConfigs);

    for (size_t i = 0; i < allConfigs.size(); i++)
    {
        bool matchFound = true;
        for (const EGLint *curAttrib = attrib_list; curAttrib[0] != EGL_NONE; curAttrib += 2)
        {
            if (curAttrib[1] == EGL_DONT_CARE)
            {
                continue;
            }

            EGLint actualValue = EGL_DONT_CARE;
            eglGetConfigAttrib(dpy, allConfigs[i], curAttrib[0], &actualValue);
            if (curAttrib[1] != actualValue)
            {
                matchFound = false;
                break;
            }
        }

        if (matchFound)
        {
            *config = allConfigs[i];
            return EGL_TRUE;
        }
    }

    return EGL_FALSE;
}
#endif

void ContextGL::framebufferResizeCallback(GLFWwindow *window, int width, int height)
{
    ContextGL *contextGL     = reinterpret_cast<ContextGL *>(glfwGetWindowUserPointer(window));
    contextGL->mClientWidth  = width;
    contextGL->mClientHeight = height;
    glViewport(0, 0, width, height);
}

Texture *ContextGL::createTexture(const std::string &name, const std::string &url)
{
    TextureGL *texture = new TextureGL(this, name, url);
    texture->loadTexture();
    return texture;
}

Texture *ContextGL::createTexture(const std::string &name, const std::vector<std::string> &urls)
{
    TextureGL *texture = new TextureGL(this, name, urls);
    texture->loadTexture();
    return texture;
}

unsigned int ContextGL::generateTexture()
{
    unsigned int texture;
    glGenTextures(1, &texture);
    return texture;
}

void ContextGL::bindTexture(unsigned int target, unsigned int textureId)
{
    glBindTexture(target, textureId);
}

void ContextGL::deleteTexture(unsigned int texture)
{
    glDeleteTextures(1, &texture);
}

void ContextGL::uploadTexture(unsigned int target,
                              unsigned int format,
                              int width,
                              int height,
                              unsigned char *pixels)
{
    glTexImage2D(target, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);
    ASSERT(glGetError() == GL_NO_ERROR);
}

void ContextGL::setParameter(unsigned int target, unsigned int pname, int param)
{
    glTexParameteri(target, pname, param);
}

void ContextGL::generateMipmap(unsigned int target)
{
    glGenerateMipmap(target);
}

void ContextGL::reallocResource(int preTotalInstance,
                                int curTotalInstance,
                                bool enableDynamicBufferOffset)
{
}

void ContextGL::updateAllFishData(
    const std::bitset<static_cast<size_t>(TOGGLE::TOGGLEMAX)> &toggleBitset)
{
}

void ContextGL::initState()
{
    glEnable(GL_DEPTH_TEST);
    glColorMask(true, true, true, true);
    glClearColor(0, 0.8f, 1, 0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
    glEnable(GL_CULL_FACE);
    glDepthMask(true);
}

void ContextGL::initAvailableToggleBitset(BACKENDTYPE backendType)
{
    mAvailableToggleBitset.set(static_cast<size_t>(TOGGLE::ENABLEMSAAx4));
    mAvailableToggleBitset.set(static_cast<size_t>(TOGGLE::UPATEANDDRAWFOREACHMODEL));
    mAvailableToggleBitset.set(static_cast<size_t>(TOGGLE::ENABLEFULLSCREENMODE));
}

Buffer *ContextGL::createBuffer(int numComponents, std::vector<float> *buf, bool isIndex)
{
    BufferGL *buffer = new BufferGL(this, static_cast<int>(buf->size()), numComponents, isIndex, GL_FLOAT, false);
    buffer->loadBuffer(*buf);

    return buffer;
}

Buffer *ContextGL::createBuffer(int numComponents, std::vector<unsigned short> *buf, bool isIndex)
{
    BufferGL *buffer = new BufferGL(this, static_cast<int>(buf->size()), numComponents, isIndex,
                                    GL_UNSIGNED_SHORT, true);
    buffer->loadBuffer(*buf);

    return buffer;
}

Program *ContextGL::createProgram(const std::string &mVId, const std::string &mFId)
{
    ProgramGL *program = new ProgramGL(this, mVId, mFId);

    return program;
}

void ContextGL::setWindowTitle(const std::string &text)
{
    glfwSetWindowTitle(mWindow, text.c_str());
}

bool ContextGL::ShouldQuit()
{
    return glfwWindowShouldClose(mWindow);
}

void ContextGL::KeyBoardQuit()
{
    if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(mWindow, GL_TRUE);
}

void ContextGL::DoFlush(const std::bitset<static_cast<size_t>(TOGGLE::TOGGLEMAX)> &toggleBitset)
{
#ifdef GL_GLEXT_PROTOTYPES
    eglSwapBuffers(mDisplay, mSurface);
#else
    glfwSwapBuffers(mWindow);
#endif
    glfwPollEvents();
}

void ContextGL::Terminate()
{
    glfwTerminate();
}

void ContextGL::showWindow()
{
    glfwGetFramebufferSize(mWindow, &mClientWidth, &mClientHeight);
    glViewport(0, 0, mClientWidth, mClientHeight);
    glfwShowWindow(mWindow);
}

void ContextGL::updateFPS(const FPSTimer &fpsTimer,
                          int *fishCount,
                          std::bitset<static_cast<size_t>(TOGGLE::TOGGLEMAX)> *toggleBitset)
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    renderImgui(fpsTimer, fishCount, toggleBitset);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ContextGL::destoryImgUI()
{
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

int ContextGL::getUniformLocation(unsigned int programId, const std::string &name) const
{
    GLint index = glGetUniformLocation(programId, name.c_str());
    ASSERT(glGetError() == GL_NO_ERROR);
    return index;
}

int ContextGL::getAttribLocation(unsigned int programId, const std::string &name) const
{
    GLint index = glGetAttribLocation(programId, name.c_str());
    ASSERT(glGetError() == GL_NO_ERROR);
    return index;
}

void ContextGL::enableBlend(bool flag) const
{
    if (flag)
    {
        glEnable(GL_BLEND);
    }
    else
    {
        glDisable(GL_BLEND);
    }
}

void ContextGL::drawElements(const BufferGL &buffer) const
{
    GLint totalComponents = buffer.getTotalComponents();
    GLenum type           = buffer.getType();
    glDrawElements(GL_TRIANGLES, totalComponents, type, 0);

    ASSERT(glGetError() == GL_NO_ERROR);
}

Model *ContextGL::createModel(Aquarium *aquarium, MODELGROUP type, MODELNAME name, bool blend)
{
    Model *model;
    switch (type)
    {
        case MODELGROUP::FISH:
            model = new FishModelGL(this, aquarium, type, name, blend);
            break;
        case MODELGROUP::GENERIC:
            model = new GenericModelGL(this, aquarium, type, name, blend);
            break;
        case MODELGROUP::INNER:
            model = new InnerModelGL(this, aquarium, type, name, blend);
            break;
        case MODELGROUP::SEAWEED:
            model = new SeaweedModelGL(this, aquarium, type, name, blend);
            break;
        case MODELGROUP::OUTSIDE:
            model = new OutsideModelGL(this, aquarium, type, name, blend);
            break;
        default:
            model = nullptr;
            std::cout << "can not create model type" << std::endl;
    }

    return model;
}

void ContextGL::preFrame()
{
    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    ASSERT(glGetError() == GL_NO_ERROR);
}

void ContextGL::setUniform(int index, const float *v, int type) const
{
    ASSERT(index != -1);
    switch (type)
    {
        case GL_FLOAT:
        {
            glUniform1f(index, *v);
            break;
        }
        case GL_FLOAT_VEC4:
        {
            glUniform4fv(index, 1, v);
            break;
        }
        case GL_FLOAT_VEC3:
        {
            glUniform3fv(index, 1, v);
            break;
        }
        case GL_FLOAT_VEC2:
        {
            glUniform2fv(index, 1, v);
            break;
        }
        case GL_FLOAT_MAT4:
        {
            glUniformMatrix4fv(index, 1, false, v);
            break;
        }
        default:
        {
            std::cout << "set uniform error" << std::endl;
        }
    }

    ASSERT(glGetError() == GL_NO_ERROR);
}

void ContextGL::setTexture(const TextureGL &texture, int index, int unit) const
{
    ASSERT(index != -1);
    glUniform1i(index, unit);
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(texture.getTarget(), texture.getTextureId());

    ASSERT(glGetError() == GL_NO_ERROR);
}

void ContextGL::setAttribs(const BufferGL &bufferGL, int index) const
{
    ASSERT(index != -1);
    glBindBuffer(bufferGL.getTarget(), bufferGL.getBuffer());

    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, bufferGL.getNumComponents(), bufferGL.getType(),
                          bufferGL.getNormalize(), bufferGL.getStride(), bufferGL.getOffset());

    ASSERT(glGetError() == GL_NO_ERROR);
}

void ContextGL::setIndices(const BufferGL &bufferGL) const
{
    glBindBuffer(bufferGL.getTarget(), bufferGL.getBuffer());
}

unsigned int ContextGL::generateVAO()
{
    unsigned int vao;
    glGenVertexArrays(1, &vao);
    return vao;
}

void ContextGL::bindVAO(unsigned int vao) const
{
    glBindVertexArray(vao);
}

void ContextGL::deleteVAO(unsigned int mVAO)
{
    glDeleteVertexArrays(1, &mVAO);
}

unsigned int ContextGL::generateBuffer()
{
    unsigned int buf;
    glGenBuffers(1, &buf);
    return buf;
}

void ContextGL::deleteBuffer(unsigned int buf)
{
    glDeleteBuffers(1, &buf);
}

void ContextGL::bindBuffer(unsigned int target, unsigned int buf)
{
    glBindBuffer(target, buf);
}

void ContextGL::uploadBuffer(unsigned int target, const std::vector<float> &buf)
{
    glBufferData(target, sizeof(GLfloat) * buf.size(), buf.data(), GL_STATIC_DRAW);

    ASSERT(glGetError() == GL_NO_ERROR);
}

void ContextGL::uploadBuffer(unsigned int target, const std::vector<unsigned short> &buf)
{
    glBufferData(target, sizeof(GLushort) * buf.size(), buf.data(), GL_STATIC_DRAW);

    ASSERT(glGetError() == GL_NO_ERROR);
}

unsigned int ContextGL::generateProgram()
{
    return glCreateProgram();
}

void ContextGL::setProgram(unsigned int program)
{
    glUseProgram(program);
}

void ContextGL::deleteProgram(unsigned int program)
{
    glDeleteProgram(program);
}

bool ContextGL::compileProgram(unsigned int programId,
                               const std::string &VertexShaderCode,
                               const std::string &FragmentShaderCode)
{
    // Create the shaders
    GLuint VertexShaderID   = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Compile Vertex Shader
    char const *VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer, nullptr);
    glCompileShader(VertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    if (!Result)
    {
        glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        std::vector<char> VertexShaderErrorMessage(InfoLogLength);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, nullptr, &VertexShaderErrorMessage[0]);
        std::cout << stdout << &VertexShaderErrorMessage[0] << std::endl;
    }

    // Compile Fragment Shader
    char const *FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, nullptr);
    glCompileShader(FragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    if (!Result)
    {
        glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, nullptr, &FragmentShaderErrorMessage[0]);
        std::cout << stdout << &FragmentShaderErrorMessage[0] << std::endl;
    }

    // Link the program
    glAttachShader(programId, VertexShaderID);
    glAttachShader(programId, FragmentShaderID);
    glLinkProgram(programId);

    // Check the program
    glGetProgramiv(programId, GL_LINK_STATUS, &Result);
    if (!Result)
    {
        glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &InfoLogLength);
        std::vector<char> ProgramErrorMessage(std::max(InfoLogLength, int(1)));
        glGetProgramInfoLog(programId, InfoLogLength, nullptr, &ProgramErrorMessage[0]);
        std::cout << stdout << &ProgramErrorMessage[0] << std::endl;
    }
    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return true;
}
