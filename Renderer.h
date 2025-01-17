//
// Copyright 2011 Tero Saarni
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#ifndef RENDERER_H
#define RENDERER_H

//#define USE_EGL

#include <pthread.h>
#ifdef USE_EGL
#include <EGL/egl.h> // requires ndk r5 or newer
#include <GLES/gl.h>
#endif
#include "WorldDebugDrawer.h"

class WorldDebugDrawer;

class Renderer {

public:
    Renderer();
    virtual ~Renderer();

    // Following methods can be called from any thread.
    // They send message to render thread which executes required actions.
    void start();
    void stop();

#ifdef USE_EGL
    void setWindow(ANativeWindow* window);
#endif


private:

    enum RenderThreadMessage {
        MSG_NONE = 0,
        MSG_WINDOW_SET,
        MSG_RENDER_LOOP_EXIT
    };

    pthread_t _threadId;
    pthread_mutex_t _mutex;
    enum RenderThreadMessage _msg;

#ifdef USE_EGL
    // android window, supported by NDK r5 and newer
    ANativeWindow* _window;

    EGLDisplay _display;
    EGLSurface _surface;
    EGLContext _context;


#endif
    int mWidth;
    int mHeight;
    GLfloat _angle;

    // RenderLoop is called in a rendering thread started in start() method
    // It creates rendering context and renders scene until stop() is called
    void renderLoop();

    bool initialize();
    void destroy();

    void drawFrame();

    // Helper method for starting the thread
    static void* threadStartCallback(void *myself);
private:


    GLuint mProgram;
    GLuint mFrameBuffer;
    GLuint mTexture;

    GLuint mVao;
    GLuint mVertexBuffer;
    GLuint mIndexBuffer;
    GLuint mVideoFrameTexture;

    WorldDebugDrawer *mDebugDrawer;

};

//class Shader {
//    static bool compileShader(GLuint &shader, GLenum type, const std::string &source);
//    static bool linkProgram(GLuint programPointer);
//    static bool validateProgram(GLuint programPointer);
//public:
//    static bool load(const std::string &vertShaderSource, const std::string &fragShaderSource, GLuint &programPointer);
//private:
//
//};


#endif // RENDERER_H
