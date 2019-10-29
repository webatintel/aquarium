//
// Copyright (c) 2019 The Aquarium Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// ContextGL.h: Defines the accessing to graphics API of OpenGL.

#ifndef ContextGL_H
#define ContextGL_H 1

#ifdef EGL_EGL_PROTOTYPES
#include <angle_gl.h>
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "EGL/eglplatform.h"
#include "EGL/eglext_angle.h"
#include <memory>
#include "EGLWindow.h"
#else
#include "glad/glad.h"
#endif

#include <vector>

#include "GLFW/glfw3.h"

#include "../Context.h"

class BufferGL;
class TextureGL;
enum BACKENDTYPE: short;

class ContextGL : public Context
{
  public:
    ContextGL(BACKENDTYPE backendType);
    ~ContextGL();
    bool initialize(BACKENDTYPE backend,
                    const std::bitset<static_cast<size_t>(TOGGLE::TOGGLEMAX)> &toggleBitset,
                    int windowWidth,
                    int windowHeight) override;
    void setWindowTitle(const std::string &text) override;
    bool ShouldQuit() override;
    void KeyBoardQuit() override;
    void DoFlush(const std::bitset<static_cast<size_t>(TOGGLE::TOGGLEMAX)> &toggleBitset) override;
    void Terminate() override;
    void showWindow() override;
    void showFPS(const FPSTimer &fpsTimer,
                 int *fishCount,
                 std::bitset<static_cast<size_t>(TOGGLE::TOGGLEMAX)> *toggleBitset) override;
    void destoryImgUI() override;

    void preFrame() override;
    void enableBlend(bool flag) const;

    Model *createModel(Aquarium *aquarium, MODELGROUP type, MODELNAME name, bool blend) override;
    int getUniformLocation(unsigned int programId, const std::string &name) const;
    int getAttribLocation(unsigned int programId, const std::string & name) const;
    void setUniform(int index, const float *v, int type) const;
    void setTexture(const TextureGL &texture, int index, int unit) const;
    void setAttribs(const BufferGL &bufferGL, int index) const;
    void setIndices(const BufferGL &bufferGL) const;
    void drawElements(const BufferGL &buffer) const;

    Buffer *createBuffer(int numComponents, std::vector<float> *buffer, bool isIndex) override;
    Buffer *createBuffer(int numComponents,
                         std::vector<unsigned short> *buffer,
                         bool isIndex) override;
    unsigned int generateBuffer();
    void deleteBuffer(unsigned int buf);
    void bindBuffer(unsigned int target, unsigned int buf);
    void uploadBuffer(unsigned int target, const std::vector<float> &buf);
    void uploadBuffer(unsigned int target, const std::vector<unsigned short> &buf);

    Program *createProgram(const std::string &mVId, const std::string &mFId) override;
    unsigned int generateProgram();
    void setProgram(unsigned int program);
    void deleteProgram(unsigned int program);
    bool compileProgram(unsigned int programId,
                        const std::string &VertexShaderCode,
                        const std::string &FragmentShaderCode);
    void bindVAO(unsigned int vao) const;
    unsigned int generateVAO();
    void deleteVAO(unsigned int vao);

    Texture *createTexture(const std::string &name, const std::string &url) override;
    Texture *createTexture(const std::string &name, const std::vector<std::string> &urls) override;
    unsigned int generateTexture();
    void bindTexture(unsigned int target, unsigned int texture);
    void deleteTexture(unsigned int texture);
    void uploadTexture(unsigned int target,
                       unsigned int format,
                       int width,
                       int height,
                       unsigned char *pixel);
    void setParameter(unsigned int target, unsigned int pname, int param);
    void generateMipmap(unsigned int target);
    void reallocResource(int preTotalInstance,
                         int curTotalInstance,
                         bool enableDynamicBufferOffset) override;
    void updateAllFishData(
        const std::bitset<static_cast<size_t>(TOGGLE::TOGGLEMAX)> &toggleBitset) override;

  private:
    void initState();
    void initAvailableToggleBitset(BACKENDTYPE backendType) override;
    static void framebufferResizeCallback(GLFWwindow *window, int width, int height);

    GLFWwindow *mWindow;
    std::string mGLSLVersion;

#ifdef EGL_EGL_PROTOTYPES
    EGLBoolean FindEGLConfig(EGLDisplay dpy, const EGLint *attrib_list, EGLConfig *config);
    EGLContext createContext(EGLContext share) const;

    EGLSurface mSurface;
    EGLContext mContext;
    EGLDisplay mDisplay;
    EGLConfig mConfig;
#endif
};

#endif  // !ContextGL_H
