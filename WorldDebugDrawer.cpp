//
//  WorldDebugDrawer.cpp
//  macOS
//
//  Created by James Folk on 6/1/17.
//
//

#define DEBUG_DRAW_IMPLEMENTATION
#include "WorldDebugDrawer.h"
//#include "Camera.h"
//#include "SDL.h"
//#include "ShaderProgram.h"
//#include "World.h"
#include "glm/gtc/type_ptr.hpp"
//#define FORMATSTRING "{\"njli::WorldDebugDrawer\":[]}"
//#include "ImGuizmo.h"
//#include "JsonJLI.h"
//#include "NJLIInterface.h"
#include "glm/glm.hpp"
//#include "Renderer.h"
//#include "imgui.h"
//#include "uSynergy.h"
#include <string>

//glm::vec3 bulletToGlm(const btVector3 &v)
//{
//    return glm::vec3(v.getX(), v.getY(), v.getZ());
//}
//
//btVector3 glmToBullet(const glm::vec3 &v) { return btVector3(v.x, v.y, v.z); }
//
//glm::quat bulletToGlm(const btQuaternion &q)
//{
//    return glm::quat(q.getW(), q.getX(), q.getY(), q.getZ());
//}
//
//btQuaternion glmToBullet(const glm::quat &q)
//{
//    return btQuaternion(q.x, q.y, q.z, q.w);
//}
//
//btMatrix3x3 glmToBullet(const glm::mat3 &m)
//{
//    return btMatrix3x3(m[0][0], m[1][0], m[2][0], m[0][1], m[1][1], m[2][1],
//                       m[0][2], m[1][2], m[2][2]);
//}
//
//// btTransform does not contain a full 4x4 matrix, so this transform is lossy.
//// Affine transformations are OK but perspective transformations are not.
//btTransform glmToBullet(const glm::mat4 &m)
//{
//    glm::mat3 m3(m);
//    return btTransform(glmToBullet(m3),
//                       glmToBullet(glm::vec3(m[3][0], m[3][1], m[3][2])));
//}
//
//glm::mat4 bulletToGlm(const btTransform &t)
//{
//    glm::mat4 m(0);
//    const btMatrix3x3 &basis = t.getBasis();
//    // rotation
//    for (int r = 0; r < 3; r++)
//    {
//        for (int c = 0; c < 3; c++)
//        {
//            m[c][r] = basis[r][c];
//        }
//    }
//    // traslation
//    btVector3 origin = t.getOrigin();
//    m[3][0] = origin.getX();
//    m[3][1] = origin.getY();
//    m[3][2] = origin.getZ();
//    // unit scale
//    m[0][3] = 0.0f;
//    m[1][3] = 0.0f;
//    m[2][3] = 0.0f;
//    m[3][3] = 1.0f;
//    return m;
//}

//namespace njli
//{

static GLfloat modelView[] = {1.0, 0.0, 0.0, 0.0,
                              0.0, 1.0, 0.0, 0.0,
                              0.0, 0.0, 1.0, 0.0,
                              0.0, 0.0, 0.0, 1.0};
static GLfloat orthographicProjection[] = {1.0, 0.0, 0.0, 0.0,
                                           0.0, 1.0, 0.0, 0.0,
                                           0.0, 0.0, 1.0, 0.0,
                                           0.0, 0.0, 0.0, 1.0};

//    static const std::string textVertShaderSrc = R"(
//    #version 100
//
//    attribute vec2 in_Position;
//    attribute vec2 in_TexCoords;
//    attribute vec3 in_Color;
//
//    uniform vec2 u_screenDimensions;
//
//    varying vec2 v_TexCoords;
//    varying vec4 v_Color;
//
//    void main()
//    {
//        // Map to normalized clip coordinates:
//        float x = ((2.0 * (in_Position.x - 0.5)) / u_screenDimensions.x) - 1.0;
//        float y = 1.0 - ((2.0 * (in_Position.y - 0.5)) / u_screenDimensions.y);
//
//        gl_Position = vec4(x, y, 0.0, 1.0);
//        v_TexCoords = in_TexCoords;
//        v_Color     = vec4(in_Color, 1.0);
//    }
//
//    )";
//
//    static const std::string textFragShaderSrc = R"(
//    #version 100
//
//#ifdef GL_ES
//    precision mediump float;
//#endif
//
//    varying vec2 v_TexCoords;
//    varying vec4 v_Color;
//
//    uniform sampler2D u_glyphTexture;
//
//    void main()
//    {
//        vec4 out_FragColor = texture2D(u_glyphTexture, v_TexCoords) * v_Color;
//        gl_FragColor = vec4(1.0, 0, 0, 1.0);
//    }
//
//    )";

    static const std::string linePointVertShaderSource = R"(
    
    attribute vec3 in_Position;
    attribute vec4 in_ColorPointSize;
    
    uniform mat4 modelView;
    uniform mat4 projection;
    
    varying vec4 v_Color;
    
    void main()
    {
        gl_Position  = (modelView * projection) * vec4(in_Position, 1.0);
        gl_PointSize = in_ColorPointSize.w;
        v_Color      = vec4(in_ColorPointSize.xyz, 1.0);
    }
    
    )";

    static const std::string linePointFragShaderSource = R"(
    
#ifdef GL_ES
    precision mediump float;
#endif
    
    varying vec4 v_Color;
    
    void main()
    {
        vec4 color = v_Color;
        gl_FragColor = color;
    }
    
    )";
enum {
    ATTRIB_VERTEX,
    ATTRIB_COLOR,
    ATTRIB_TEXTUREPOSITON,
    NUM_ATTRIBUTES
};







bool Shader::compileShader(GLuint &shader, GLenum type, const std::string &source)
{
    GLint status;
    const GLchar *_source = (GLchar*)source.c_str();

    if (!_source)
    {
        LOG_ERROR("Failed to load vertex shader");
        return false;
    }

    shader = glCreateShader(type);
    glShaderSource(shader, 1, &_source, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        glDeleteShader(shader);

        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0)
        {
            GLchar *log = (GLchar *)malloc(logLength);
            glGetShaderInfoLog(shader, logLength, &logLength, log);
            LOG_ERROR("Shader compile log:\n%s", log);
            free(log);
        }

        return false;
    }

    return true;
}
bool Shader::linkProgram(GLuint programPointer)
{
    GLint status(GL_FALSE);

    glLinkProgram(programPointer);

    glGetProgramiv(programPointer, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint logLength;
        glGetProgramiv(programPointer, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0)
        {
            GLchar *log = (GLchar *)malloc(logLength);
            glGetProgramInfoLog(programPointer, logLength, &logLength, log);
            LOG_ERROR("Program link log:\n%s", log);
            free(log);
        }

        return false;
    }

    return true;
}

bool Shader::validateProgram(GLuint programPointer)
{
    GLint status(GL_FALSE);

    glValidateProgram(programPointer);

    glGetProgramiv(programPointer, GL_VALIDATE_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint logLength;
        glGetProgramiv(programPointer, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0)
        {
            GLchar *log = (GLchar *)malloc(logLength);
            glGetProgramInfoLog(programPointer, logLength, &logLength, log);
            LOG_ERROR("Program validate log:\n%s", log);
            free(log);
        }
        return false;
    }

    return true;
}

//bool Shader::load(const std::string &vertShaderSource, const std::string &fragShaderSource, GLuint &programPointer);
bool Shader::load(const std::string &vertShaderSource, const std::string &fragShaderSource, GLuint &programPointer)
{
    GLuint vertexShader(0), fragShader(0);

    programPointer = glCreateProgram();

    if(!compileShader(vertexShader, GL_VERTEX_SHADER, vertShaderSource))
    {
        return false;
    }

    if(!compileShader(fragShader, GL_FRAGMENT_SHADER, fragShaderSource))
    {
        return false;
    }


    // Attach vertex shader to program.
    glAttachShader(programPointer, vertexShader);

    // Attach fragment shader to program.
    glAttachShader(programPointer, fragShader);

    // Bind attribute locations.
    // This needs to be done prior to linking.
    glBindAttribLocation(programPointer, ATTRIB_VERTEX, "a_Position");
    glErrorCheck();
    glBindAttribLocation(programPointer, ATTRIB_COLOR, "a_Color");
    glErrorCheck();
    glBindAttribLocation(programPointer, ATTRIB_TEXTUREPOSITON, "a_Texture");
    glErrorCheck();
//    glBindAttribLocation(programPointer, ATTRIB_TEXTUREPOSITON, "inputTextureCoordinate");

    // Link program.
    if(!linkProgram(programPointer))
    {
        LOG_ERROR("Failed to link program: %d", programPointer);

        if (vertexShader) {
            glDeleteShader(vertexShader);
            vertexShader = 0;
        }
        if (fragShader) {
            glDeleteShader(fragShader);
            fragShader = 0;
        }
        if (programPointer) {
            glDeleteProgram(programPointer);
            programPointer = 0;
        }

        return false;
    }

    if(!validateProgram(programPointer))
    {
        return false;
    }

    // Get uniform locations.
//    GLint videoFrame = glGetUniformLocation(programPointer, "videoFrame");
//    uniforms[UNIFORM_VIDEOFRAME] = videoFrame;
//    uniforms[UNIFORM_INPUTCOLOR] = glGetUniformLocation(programPointer, "inputColor");
//    uniforms[UNIFORM_THRESHOLD] = glGetUniformLocation(programPointer, "threshold");

    // Release vertex and fragment shaders.
    if (vertexShader)
    {
        glDeleteShader(vertexShader);
    }
    if (fragShader)
    {
        glDeleteShader(fragShader);
    }

    return true;
}

WorldDebugDrawer::WorldDebugDrawer()
        :
//        m_DebugMode(btIDebugDraw::DBG_MAX_DEBUG_DRAW_MODE),
          m_Initialized(false),
//          m_LinePointShaderProgram(NULL),
//          m_TextShaderProgram(NULL),
            mLinePointShaderProgram(0),
          m_mat4Buffer(new float[16]),
          m_textMat4Buffer(new float[16]), linePointVAO(0), linePointVBO(0)//,
//          textVAO(0),
//          textVBO(0)
    {
    }

    WorldDebugDrawer::~WorldDebugDrawer()
    {
        delete[] m_textMat4Buffer;
        delete[] m_mat4Buffer;

        glDeleteVertexArrays_NJLIC(1, &linePointVAO);

        glDeleteBuffers(1, &linePointVBO);

//        glDeleteVertexArrays_NJLIC(1, &textVAO);
//
//        glDeleteBuffers(1, &textVBO);
    }

//    const char *WorldDebugDrawer::getClassName() const
//    {
//        return "WorldDebugDrawer";
//    }

//    s32 WorldDebugDrawer::getType() const
//    {
//        return JLI_OBJECT_TYPE_WorldDebugDrawer;
//    }
//
//    WorldDebugDrawer::operator std::string() const
//    {
//        return njli::JsonJLI::parse(string_format("%s", FORMATSTRING));
//    }

    void WorldDebugDrawer::beginDraw() { /*newFrameImgui();*/ }

    void WorldDebugDrawer::endDraw() { /*renderImgui();*/ }

    void WorldDebugDrawer::drawPointList(const dd::DrawVertex *points,
                                         int count, bool depthEnabled)
    {
//        SDL_assert(points != nullptr);
//        SDL_assert(count > 0 && count <= DEBUG_DRAW_VERTEX_BUFFER_SIZE);

        glBindVertexArray_NJLIC(linePointVAO);

//        m_LinePointShaderProgram->use();
        glUseProgram(mLinePointShaderProgram);



//        m_Camera->render(m_LinePointShaderProgram, true);
        GLint modelViewLocation = glGetUniformLocation(mLinePointShaderProgram, "modelView");
        glUniformMatrix4fv(modelViewLocation, 1, GL_FALSE, modelView);

        GLint projectionLocation = glGetUniformLocation(mLinePointShaderProgram, "projection");
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, orthographicProjection);

        GLboolean _depthEnabled = glIsEnabled(GL_DEPTH_TEST);

        if (depthEnabled)
        {
            glEnable(GL_DEPTH_TEST);
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
        }

        // NOTE: Could also use glBufferData to take advantage of the buffer
        // orphaning trick...
        glBindBuffer(GL_ARRAY_BUFFER, linePointVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(dd::DrawVertex),
                        points);

        // Issue the draw call:
        glDrawArrays(GL_POINTS, 0, count);

        glUseProgram(0);

        glBindVertexArray_NJLIC(0);

        if (_depthEnabled)
        {
            glEnable(GL_DEPTH_TEST);
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void WorldDebugDrawer::drawLineList(const dd::DrawVertex *lines, int count,
                                        bool depthEnabled)
    {
        assert(lines != nullptr);
        assert(count > 0 && count <= DEBUG_DRAW_VERTEX_BUFFER_SIZE);

        glBindVertexArray_NJLIC(linePointVAO);

//        m_LinePointShaderProgram->use();
        glUseProgram(mLinePointShaderProgram);

//        m_Camera->render(m_LinePointShaderProgram, true);

        GLint modelViewLocation = glGetUniformLocation(mLinePointShaderProgram, "modelView");
        glUniformMatrix4fv(modelViewLocation, 1, GL_FALSE, modelView);

        GLint projectionLocation = glGetUniformLocation(mLinePointShaderProgram, "projection");
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, orthographicProjection);

        GLboolean _depthEnabled = glIsEnabled(GL_DEPTH_TEST);

        if (depthEnabled)
        {
            glEnable(GL_DEPTH_TEST);
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
        }

        // NOTE: Could also use glBufferData to take advantage of the buffer
        // orphaning trick...
        glBindBuffer(GL_ARRAY_BUFFER, linePointVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(dd::DrawVertex),
                        lines);

        // Issue the draw call:
        glDrawArrays(GL_LINES, 0, count);

        glUseProgram(0);

        glBindVertexArray_NJLIC(0);

        if (_depthEnabled)
        {
            glEnable(GL_DEPTH_TEST);
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    void WorldDebugDrawer::drawGlyphList(const dd::DrawVertex *glyphs,
                                         int count,
                                         dd::GlyphTextureHandle glyphTex)
    {
//        assert(glyphs != nullptr);
//        assert(count > 0 && count <= DEBUG_DRAW_VERTEX_BUFFER_SIZE);
//
//        glBindVertexArray_NJLIC(textVAO);
//
//        m_TextShaderProgram->use();
//        GLfloat width = njli::World::getInstance()->getViewportDimensions().x();
//        GLfloat height =
//            njli::World::getInstance()->getViewportDimensions().y();
//        m_TextShaderProgram->setUniformValue("u_glyphTexture", (GLuint)0);
//        m_TextShaderProgram->setUniformValue("u_screenDimensions", width,
//                                             height);
//
//        if (glyphTex != nullptr)
//        {
//            glActiveTexture(GL_TEXTURE0);
//            glBindTexture(
//                GL_TEXTURE_2D,
//                static_cast<GLuint>(reinterpret_cast<std::size_t>(glyphTex)));
//        }
//
//        glEnable(GL_BLEND);
//        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//        glDisable(GL_DEPTH_TEST);
//
//        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
//        glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(dd::DrawVertex),
//                        glyphs);
//
//        glDrawArrays(GL_TRIANGLES, 0, count); // Issue the draw call
//
//        glDisable(GL_BLEND);
//        glUseProgram(0);
//        glBindVertexArray_NJLIC(0);
//        glBindBuffer(GL_ARRAY_BUFFER, 0);
//        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void WorldDebugDrawer::destroyGlyphTexture(dd::GlyphTextureHandle glyphTex)
    {
        if (glyphTex == nullptr)
        {
            return;
        }

        const GLuint textureId =
            static_cast<GLuint>(reinterpret_cast<std::size_t>(glyphTex));
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(1, &textureId);
    }

    dd::GlyphTextureHandle
    WorldDebugDrawer::createGlyphTexture(int width, int height,
                                         const void *pixels)
    {
        assert(width > 0 && height > 0);
        assert(pixels != nullptr);

        GLuint textureId = 0;
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0,
                     GL_LUMINANCE, GL_UNSIGNED_BYTE, pixels);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);
        ;

        return reinterpret_cast<dd::GlyphTextureHandle>(
            static_cast<std::size_t>(textureId));
    }

//    void WorldDebugDrawer::drawLine(const btVector3 &from, const btVector3 &to,
//                                    const btVector3 &color)
//    {
//        const float _from[] = {from.x(), from.y(), from.z()};
//        const float _to[] = {to.x(), to.y(), to.z()};
//        const float _color[] = {color.x(), color.y(), color.z()};
//
//        dd::line(_from, _to, _color);
//    }
//
//    void WorldDebugDrawer::drawContactPoint(const btVector3 &PointOnB,
//                                            const btVector3 &normalOnB,
//                                            btScalar distance, int lifeTime,
//                                            const btVector3 &color)
//    {
//        const float _pos[] = {PointOnB.x(), PointOnB.y(), PointOnB.z()};
//        const float _color[] = {color.x(), color.y(), color.z()};
//        dd::point(_pos, _color, 1.0, lifeTime);
//
//        btVector3 from(PointOnB);
//        btVector3 to(PointOnB + (normalOnB.normalized() * normalOnB.length()));
//        const float _from[] = {from.x(), from.y(), from.z()};
//        const float _to[] = {to.x(), to.y(), to.z()};
//
//        dd::line(_from, _to, _color, lifeTime);
//    }

    void WorldDebugDrawer::reportErrorWarning(const char *warningString)
    {
//        SDL_LogError(SDL_LOG_CATEGORY_TEST, "%s", warningString);
    }

//    void WorldDebugDrawer::draw3dText(const btVector3 &location,
//                                      const char *textString)
//    {
//        btVector3 color(1.0, 1.0, 1.0);
//
//        const float _pos[] = {location.x(), location.y(), location.z()};
//        const float _color[] = {color.x(), color.y(), color.z()};
//
//        dd::projectedText(
//            std::string(textString), _pos, _color, m_textMat4Buffer, 0, 0,
//            njli::World::getInstance()->getViewportDimensions().x(),
//            njli::World::getInstance()->getViewportDimensions().y());
//    }

//    void WorldDebugDrawer::setDebugMode(int debugMode)
//    {
//        m_DebugMode = debugMode;
//    }
//
//    int WorldDebugDrawer::getDebugMode() const { return m_DebugMode; }

    void WorldDebugDrawer::init()
    {
        if (!m_Initialized)
        {
            m_Initialized = true;

            dd::initialize(this);

            glEnable(GL_CULL_FACE);
            glEnable(GL_DEPTH_TEST);
            glDisable(GL_BLEND);

            // This has to be enabled since the point drawing shader will
            // use gl_PointSize.

            //        glEnable(GL_PROGRAM_POINT_SIZE);

//            m_LinePointShaderProgram = njli::ShaderProgram::create();
//            m_TextShaderProgram = njli::ShaderProgram::create();
//
//            m_LinePointShaderProgram->load(linePointVertShaderSource,
//                                           linePointFragShaderSource);
//            m_TextShaderProgram->load(textVertShaderSrc, textFragShaderSrc);

            Shader::load(linePointVertShaderSource, linePointFragShaderSource, mLinePointShaderProgram);

            setupVertexBuffers();

//            initImgui();
        }
    }

    void WorldDebugDrawer::unInit()
    {
        if (m_Initialized)
        {
            m_Initialized = false;

            if(0 == mLinePointShaderProgram)
            {
                glDeleteProgram(mLinePointShaderProgram);
                mLinePointShaderProgram = 0;
            }

//            njli::ShaderProgram::destroy(m_TextShaderProgram);
//            njli::ShaderProgram::destroy(m_LinePointShaderProgram);

//            unInitImgui();

            dd::shutdown();
        }
    }

    void WorldDebugDrawer::draw()//Camera *camera)
    {
//        m_Camera = camera;
//
//        if (m_Camera && m_Camera->hasParent())
//        {
//            (m_Camera->getProjection() * m_Camera->getModelView())
//                .getOpenGLMatrix(m_textMat4Buffer);
//        }

        if (dd::hasPendingDraws())
        {
            dd::flush(0);
        }
    }

//    void WorldDebugDrawer::point(const btVector3 &pos, const btVector3 &color,
//                                 float size, int durationMillis,
//                                 bool depthEnabled)
//    {
//        const float _pos[] = {pos.x(), pos.y(), pos.z()};
//        const float _color[] = {color.x(), color.y(), color.z()};
//
//        dd::point(_pos, _color, size, durationMillis, depthEnabled);
//    }

//    void WorldDebugDrawer::line(const btVector3 &from, const btVector3 &to,
//                                const btVector3 &color, int durationMillis,
//                                bool depthEnabled)
//    {
//        const float _from[] = {from.x(), from.y(), from.z()};
//        const float _to[] = {to.x(), to.y(), to.z()};
//        const float _color[] = {color.x(), color.y(), color.z()};
//
//        dd::line(_from, _to, _color, durationMillis, depthEnabled);
//    }

//    void WorldDebugDrawer::screenText(const std::string &str,
//                                      const btVector3 &pos,
//                                      const btVector3 &color, float scaling,
//                                      int durationMillis)
//    {
//        const float _pos[] = {pos.x(), pos.y(), pos.z()};
//        const float _color[] = {color.x(), color.y(), color.z()};
//
//        dd::screenText(str, _pos, _color, scaling, durationMillis);
//    }

//    void WorldDebugDrawer::projectedText(const std::string &str,
//                                         const btVector3 &pos,
//                                         const btVector3 &color, float scaling,
//                                         int durationMillis)
//    {
//        const float _pos[] = {pos.x(), pos.y(), pos.z()};
//        const float _color[] = {color.x(), color.y(), color.z()};
//
//        dd::projectedText(
//            str, _pos, _color, m_textMat4Buffer, 0, 0,
//            njli::World::getInstance()->getViewportDimensions().x(),
//            njli::World::getInstance()->getViewportDimensions().y());
//    }

//    void WorldDebugDrawer::axisTriad(const btTransform &transform, float size,
//                                     float length, int durationMillis,
//                                     bool depthEnabled)
//    {
//
//        transform.getOpenGLMatrix(m_mat4Buffer);
//
//        dd::axisTriad(m_mat4Buffer, size, length, durationMillis, depthEnabled);
//    }

//    void WorldDebugDrawer::arrow(const btVector3 &from, const btVector3 &to,
//                                 const btVector3 &color, float size,
//                                 int durationMillis, bool depthEnabled)
//    {
//        const float _from[] = {from.x(), from.y(), from.z()};
//        const float _to[] = {to.x(), to.y(), to.z()};
//        const float _color[] = {color.x(), color.y(), color.z()};
//
//        dd::arrow(_from, _to, _color, size, durationMillis, depthEnabled);
//    }

//    void WorldDebugDrawer::cross(const btVector3 &center, float length,
//                                 int durationMillis, bool depthEnabled)
//    {
//        const float _center[] = {center.x(), center.y(), center.z()};
//
//        dd::cross(_center, length, durationMillis, depthEnabled);
//    }

//    void WorldDebugDrawer::circle(const btVector3 &center,
//                                  const btVector3 &planeNormal,
//                                  const btVector3 &color, float radius,
//                                  float numSteps, int durationMillis,
//                                  bool depthEnabled)
//    {
//        const float _center[] = {center.x(), center.y(), center.z()};
//        const float _planeNormal[] = {planeNormal.x(), planeNormal.y(),
//                                      planeNormal.z()};
//        const float _color[] = {color.x(), color.y(), color.z()};
//
//        dd::circle(_center, _planeNormal, _color, radius, numSteps,
//                   durationMillis, depthEnabled);
//    }

//    void WorldDebugDrawer::plane(const btVector3 &center,
//                                 const btVector3 &planeNormal,
//                                 const btVector3 &planeColor,
//                                 const btVector3 &normalVecColor,
//                                 float planeScale, float normalVecScale,
//                                 int durationMillis, bool depthEnabled)
//    {
//        btVector3 pn(planeNormal.normalized());
//
//        const float _center[] = {center.x(), center.y(), center.z()};
//        const float _planeNormal[] = {pn.x(), pn.y(), pn.z()};
//        const float _planeColor[] = {planeColor.x(), planeColor.y(),
//                                     planeColor.z()};
//        const float _normalVecColor[] = {normalVecColor.x(), normalVecColor.y(),
//                                         normalVecColor.z()};
//
//        dd::plane(_center, _planeNormal, _planeColor, _normalVecColor,
//                  planeScale, normalVecScale, durationMillis, depthEnabled);
//    }

//    void WorldDebugDrawer::sphere(const btVector3 &center,
//                                  const btVector3 &color, float radius,
//                                  int durationMillis, bool depthEnabled)
//    {
//        const float _center[] = {center.x(), center.y(), center.z()};
//        const float _color[] = {color.x(), color.y(), color.z()};
//
//        dd::sphere(_center, _color, radius, durationMillis, depthEnabled);
//    }

//    void WorldDebugDrawer::cone(const btVector3 &apex, const btVector3 &dir,
//                                const btVector3 &color, float baseRadius,
//                                float apexRadius, int durationMillis,
//                                bool depthEnabled)
//    {
//        const float _apex[] = {apex.x(), apex.y(), apex.z()};
//        const float _dir[] = {dir.x(), dir.y(), dir.z()};
//        const float _color[] = {color.x(), color.y(), color.z()};
//
//        dd::cone(_apex, _dir, _color, baseRadius, apexRadius, durationMillis,
//                 depthEnabled);
//    }

//    void WorldDebugDrawer::box(const glm::vec3 &p0, const glm::vec3 &p1,
//                               const glm::vec3 &p2, const glm::vec3 &p3,
//                               const glm::vec3 &p4, const glm::vec3 &p5,
//                               const glm::vec3 &p6, const glm::vec3 &p7,
//                               const glm::vec3 &color, int durationMillis,
//                               bool depthEnabled)
//    {
//        //            const float _from[] = {from.x(), from.y(), from.z()};
//        //            const float _to[] = {to.x(), to.y(), to.z()};
//        //            const float _color[] = {color.x(), color.y(), color.z()};
//
//        const float _points[] = {p0.x, p0.y, p0.z, p1.x, p1.y, p1.z,
//                                 p2.x, p2.y, p2.z, p3.x, p3.y, p3.z,
//                                 p4.x, p4.y, p4.z, p5.x, p5.y, p5.z,
//                                 p6.x, p6.y, p6.z, p7.x, p7.y, p7.z};
//        const float _color[] = {color.x, color.y, color.z};
//
//        //            dd::box(_points, _color, durationMillis, depthEnabled);
//    }

//    void WorldDebugDrawer::box(const btVector3 &center, const btVector3 &color,
//                               float width, float height, float depth,
//                               int durationMillis, bool depthEnabled)
//    {
//        const float _center[] = {center.x(), center.y(), center.z()};
//        const float _color[] = {color.x(), color.y(), color.z()};
//
//        dd::box(_center, _color, width, height, depth, durationMillis,
//                depthEnabled);
//    }

//    void WorldDebugDrawer::aabb(const btVector3 &mins, const btVector3 &maxs,
//                                const btVector3 &color, int durationMillis,
//                                bool depthEnabled)
//    {
//        const float _mins[] = {mins.x(), mins.y(), mins.z()};
//        const float _maxs[] = {maxs.x(), maxs.y(), maxs.z()};
//        const float _color[] = {color.x(), color.y(), color.z()};
//
//        dd::aabb(_mins, _maxs, _color, durationMillis, depthEnabled);
//    }

//    void WorldDebugDrawer::frustum(const btTransform &view,
//                                   const btTransform &proj,
//                                   const btVector3 &color, int durationMillis,
//                                   bool depthEnabled)
//    {
//        btTransform invClipMatrix = (proj * view); //.inverse();
//
//        invClipMatrix.getOpenGLMatrix(m_mat4Buffer);
//        const float _color[] = {color.x(), color.y(), color.z()};
//
//        dd::frustum(m_mat4Buffer, _color);
//    }

//    void WorldDebugDrawer::vertexNormal(const btVector3 &origin,
//                                        const btVector3 &normal, float length,
//                                        int durationMillis, bool depthEnabled)
//    {
//        const float _origin[] = {origin.x(), origin.y(), origin.z()};
//        const float _normal[] = {normal.x(), normal.y(), normal.z()};
//
//        dd::vertexNormal(_origin, _normal, length, durationMillis,
//                         depthEnabled);
//    }

//    void WorldDebugDrawer::tangentBasis(const btVector3 &origin,
//                                        const btVector3 &normal,
//                                        const btVector3 &tangent,
//                                        const btVector3 &bitangent,
//                                        float lengths, int durationMillis,
//                                        bool depthEnabled)
//    {
//        const float _origin[] = {origin.x(), origin.y(), origin.z()};
//        const float _normal[] = {normal.x(), normal.y(), normal.z()};
//        const float _tangent[] = {tangent.x(), tangent.y(), tangent.z()};
//        const float _bitangent[] = {bitangent.x(), bitangent.y(),
//                                    bitangent.z()};
//
//        dd::tangentBasis(_origin, _normal, _tangent, _bitangent, lengths,
//                         durationMillis, depthEnabled);
//    }

//    void WorldDebugDrawer::xzSquareGrid(float mins, float maxs, float y,
//                                        float step, const btVector3 &color,
//                                        int durationMillis, bool depthEnabled)
//    {
//        const float _color[] = {color.x(), color.y(), color.z()};
//
//        dd::xzSquareGrid(mins, maxs, y, step, _color, durationMillis,
//                         depthEnabled);
//    }

    void WorldDebugDrawer::setupVertexBuffers()
    {
        // std::cout << "> DDRenderInterfaceCoreGL::setupVertexBuffers()" <<
        // std::endl;

        //
        // Lines/points vertex buffer:
        //
        glGenVertexArrays_NJLIC(1, &linePointVAO);
        glBindVertexArray_NJLIC(linePointVAO);
        {
            glGenBuffers(1, &linePointVBO);
            glBindBuffer(GL_ARRAY_BUFFER, linePointVBO);
            // RenderInterface will never be called with a batch larger than
            // DEBUG_DRAW_VERTEX_BUFFER_SIZE vertexes, so we can allocate the
            // same amount here.
            glBufferData(GL_ARRAY_BUFFER,
                         DEBUG_DRAW_VERTEX_BUFFER_SIZE * sizeof(dd::DrawVertex),
                         nullptr, GL_STREAM_DRAW);

            // Set the vertex format expected by 3D points and lines:
//            int inPositionAttrib =
//                m_LinePointShaderProgram->getAttributeLocation("in_Position");
//            int inColorPointSize =
//                m_LinePointShaderProgram->getAttributeLocation(
//                    "in_ColorPointSize");
            GLint inPositionAttrib = glGetAttribLocation(mLinePointShaderProgram, "in_Position");
            GLint inColorPointSize = glGetAttribLocation(mLinePointShaderProgram, "in_ColorPointSize");

            glEnableVertexAttribArray(inPositionAttrib); // in_Position (vec3)
            glVertexAttribPointer(
                /* index     = */ inPositionAttrib,
                /* size      = */ 3,
                /* type      = */ GL_FLOAT,
                /* normalize = */ GL_FALSE,
                /* stride    = */ sizeof(dd::DrawVertex),
                /* offset    = */
                (const GLvoid *)offsetof(dd::DrawVertex, line.x));

            glEnableVertexAttribArray(
                inColorPointSize); // in_ColorPointSize (vec4)
            glVertexAttribPointer(
                /* index     = */ inColorPointSize,
                /* size      = */ 4,
                /* type      = */ GL_FLOAT,
                /* normalize = */ GL_FALSE,
                /* stride    = */ sizeof(dd::DrawVertex),
                /* offset    = */
                (const GLvoid *)offsetof(dd::DrawVertex, line.r));

            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        glBindVertexArray_NJLIC(0);

        //
        // Text rendering vertex buffer:
        //
//        glGenVertexArrays_NJLIC(1, &textVAO);
//        glBindVertexArray_NJLIC(textVAO);
//        {
//            glGenBuffers(1, &textVBO);
//            glBindBuffer(GL_ARRAY_BUFFER, textVBO);
//
//            // NOTE: A more optimized implementation might consider combining
//            // both the lines/points and text buffers to save some memory!
//            glBufferData(GL_ARRAY_BUFFER,
//                         DEBUG_DRAW_VERTEX_BUFFER_SIZE * sizeof(dd::DrawVertex),
//                         nullptr, GL_STREAM_DRAW);
//
//            // Set the vertex format expected by the 2D text:
//            std::size_t offset = 0;
//
//            int in_Position =
//                m_TextShaderProgram->getAttributeLocation("in_Position");
//            int in_TexCoords =
//                m_TextShaderProgram->getAttributeLocation("in_TexCoords");
//            int in_Color =
//                m_TextShaderProgram->getAttributeLocation("in_Color");
//
//            glEnableVertexAttribArray(in_Position); // in_Position (vec2)
//            glVertexAttribPointer(
//                /* index     = */ in_Position,
//                /* size      = */ 2,
//                /* type      = */ GL_FLOAT,
//                /* normalize = */ GL_FALSE,
//                /* stride    = */ sizeof(dd::DrawVertex),
//                /* offset    = */ (const GLvoid *)offset);
//            offset += sizeof(float) * 2;
//
//            glEnableVertexAttribArray(in_TexCoords); // in_TexCoords (vec2)
//            glVertexAttribPointer(
//                /* index     = */ in_TexCoords,
//                /* size      = */ 2,
//                /* type      = */ GL_FLOAT,
//                /* normalize = */ GL_FALSE,
//                /* stride    = */ sizeof(dd::DrawVertex),
//                /* offset    = */ (const GLvoid *)offset);
//            offset += sizeof(float) * 2;
//
//            glEnableVertexAttribArray(in_Color); // in_Color (vec4)
//            glVertexAttribPointer(
//                /* index     = */ in_Color,
//                /* size      = */ 3,
//                /* type      = */ GL_FLOAT,
//                /* normalize = */ GL_FALSE,
//                /* stride    = */ sizeof(dd::DrawVertex),
//                /* offset    = */ (const GLvoid *)offset);
//
//            glBindBuffer(GL_ARRAY_BUFFER, 0);
//        }
//        glBindVertexArray_NJLIC(0);
    }

    // From Carbon HIToolbox/Events.h
    // FIXME: Keyboard mapping is hacked in because Synergy d_NJLICn't give us
    // character but only keycode which aren't really portable if you consider
    // keyboard locale. See https://github.com/ocornut/imgui/pull/247
    //    enum {
    //        kVK_ANSI_A                    = 0x00,
    //        kVK_ANSI_S                    = 0x01,
    //        kVK_ANSI_D                    = 0x02,
    //        kVK_ANSI_F                    = 0x03,
    //        kVK_ANSI_H                    = 0x04,
    //        kVK_ANSI_G                    = 0x05,
    //        kVK_ANSI_Z                    = 0x06,
    //        kVK_ANSI_X                    = 0x07,
    //        kVK_ANSI_C                    = 0x08,
    //        kVK_ANSI_V                    = 0x09,
    //        kVK_ANSI_B                    = 0x0B,
    //        kVK_ANSI_Q                    = 0x0C,
    //        kVK_ANSI_W                    = 0x0D,
    //        kVK_ANSI_E                    = 0x0E,
    //        kVK_ANSI_R                    = 0x0F,
    //        kVK_ANSI_Y                    = 0x10,
    //        kVK_ANSI_T                    = 0x11,
    //        kVK_ANSI_1                    = 0x12,
    //        kVK_ANSI_2                    = 0x13,
    //        kVK_ANSI_3                    = 0x14,
    //        kVK_ANSI_4                    = 0x15,
    //        kVK_ANSI_6                    = 0x16,
    //        kVK_ANSI_5                    = 0x17,
    //        kVK_ANSI_Equal                = 0x18,
    //        kVK_ANSI_9                    = 0x19,
    //        kVK_ANSI_7                    = 0x1A,
    //        kVK_ANSI_Minus                = 0x1B,
    //        kVK_ANSI_8                    = 0x1C,
    //        kVK_ANSI_0                    = 0x1D,
    //        kVK_ANSI_RightBracket         = 0x1E,
    //        kVK_ANSI_O                    = 0x1F,
    //        kVK_ANSI_U                    = 0x20,
    //        kVK_ANSI_LeftBracket          = 0x21,
    //        kVK_ANSI_I                    = 0x22,
    //        kVK_ANSI_P                    = 0x23,
    //        kVK_ANSI_L                    = 0x25,
    //        kVK_ANSI_J                    = 0x26,
    //        kVK_ANSI_Quote                = 0x27,
    //        kVK_ANSI_K                    = 0x28,
    //        kVK_ANSI_Semicolon            = 0x29,
    //        kVK_ANSI_Backslash            = 0x2A,
    //        kVK_ANSI_Comma                = 0x2B,
    //        kVK_ANSI_Slash                = 0x2C,
    //        kVK_ANSI_N                    = 0x2D,
    //        kVK_ANSI_M                    = 0x2E,
    //        kVK_ANSI_Period               = 0x2F,
    //        kVK_ANSI_Grave                = 0x32,
    //        kVK_ANSI_KeypadDecimal        = 0x41,
    //        kVK_ANSI_KeypadMultiply       = 0x43,
    //        kVK_ANSI_KeypadPlus           = 0x45,
    //        kVK_ANSI_KeypadClear          = 0x47,
    //        kVK_ANSI_KeypadDivide         = 0x4B,
    //        kVK_ANSI_KeypadEnter          = 0x4C,
    //        kVK_ANSI_KeypadMinus          = 0x4E,
    //        kVK_ANSI_KeypadEquals         = 0x51,
    //        kVK_ANSI_Keypad0              = 0x52,
    //        kVK_ANSI_Keypad1              = 0x53,
    //        kVK_ANSI_Keypad2              = 0x54,
    //        kVK_ANSI_Keypad3              = 0x55,
    //        kVK_ANSI_Keypad4              = 0x56,
    //        kVK_ANSI_Keypad5              = 0x57,
    //        kVK_ANSI_Keypad6              = 0x58,
    //        kVK_ANSI_Keypad7              = 0x59,
    //        kVK_ANSI_Keypad8              = 0x5B,
    //        kVK_ANSI_Keypad9              = 0x5C
    //    };
    //
    //    /* keycodes for keys that are independent of keyboard layout*/
    //    enum {
    //        kVK_Return                    = 0x24,
    //        kVK_Tab                       = 0x30,
    //        kVK_Space                     = 0x31,
    //        kVK_Delete                    = 0x33,
    //        kVK_Escape                    = 0x35,
    //        kVK_Command                   = 0x37,
    //        kVK_Shift                     = 0x38,
    //        kVK_CapsLock                  = 0x39,
    //        kVK_Option                    = 0x3A,
    //        kVK_Control                   = 0x3B,
    //        kVK_RightShift                = 0x3C,
    //        kVK_RightOption               = 0x3D,
    //        kVK_RightControl              = 0x3E,
    //        kVK_Function                  = 0x3F,
    //        kVK_F17                       = 0x40,
    //        kVK_VolumeUp                  = 0x48,
    //        kVK_VolumeDown                = 0x49,
    //        kVK_Mute                      = 0x4A,
    //        kVK_F18                       = 0x4F,
    //        kVK_F19                       = 0x50,
    //        kVK_F20                       = 0x5A,
    //        kVK_F5                        = 0x60,
    //        kVK_F6                        = 0x61,
    //        kVK_F7                        = 0x62,
    //        kVK_F3                        = 0x63,
    //        kVK_F8                        = 0x64,
    //        kVK_F9                        = 0x65,
    //        kVK_F11                       = 0x67,
    //        kVK_F13                       = 0x69,
    //        kVK_F16                       = 0x6A,
    //        kVK_F14                       = 0x6B,
    //        kVK_F10                       = 0x6D,
    //        kVK_F12                       = 0x6F,
    //        kVK_F15                       = 0x71,
    //        kVK_Help                      = 0x72,
    //        kVK_Home                      = 0x73,
    //        kVK_PageUp                    = 0x74,
    //        kVK_ForwardDelete             = 0x75,
    //        kVK_F4                        = 0x76,
    //        kVK_End                       = 0x77,
    //        kVK_F2                        = 0x78,
    //        kVK_PageDown                  = 0x79,
    //        kVK_F1                        = 0x7A,
    //        kVK_LeftArrow                 = 0x7B,
    //        kVK_RightArrow                = 0x7C,
    //        kVK_DownArrow                 = 0x7D,
    //        kVK_UpArrow                   = 0x7E
    //    };
    //
//    static char g_keycodeCharUnshifted[256] = {};
//    static char g_keycodeCharShifted[256] = {};
//
//    static double g_Time = 0.0f;
//    static bool g_MousePressed[3] = {false, false, false};
//    static float g_MouseWheel = 0.0f;
//    static float g_mouseWheelX = 0.0f;
//    static float g_mouseWheelY = 0.0f;
//
//    static GLuint g_FontTexture = 0;
//    static int g_ShaderHandle = 0, g_VertHandle = 0, g_FragHandle = 0;
//    static int g_AttribLocationTex = 0, g_AttribLocationProjMtx = 0;
//    static int g_AttribLocationPosition = 0, g_AttribLocationUV = 0,
//               g_AttribLocationColor = 0;
//    static size_t g_VboSize = 0;
//    static unsigned int g_VboHandle = 0, g_VaoHandle = 0, g_ElementsHandle = 0;
//    static float g_displayScale;
//
//    static int usynergy_sockfd;
//    static bool g_synergyPtrActive = false;
//    static uint16_t g_mousePosX = 0;
//    static uint16_t g_mousePosY = 0;

//    static const char *ImGui_ImplGlfw_GetClipboardText(void *user_data);
//    static void ImGui_ImplGlfw_SetClipboardText(void *user_data,
//                                                const char *text);
//    static void ImGui_ImplIOS_RenderDrawLists(ImDrawData *draw_data);
//    bool ImGui_ImplIOS_CreateDeviceObjects();

    //    static NSString *g_serverName;
//    static std::string g_serverName;

    //    void setupKeymaps()
    //    {
    //        // The keyboard mapping is a big headache. I tried for a while to
    //        find a better way to do this,
    //        // but this was the best I could come up with. There are some
    //        device independent API's available
    //        // to convert scan codes to unicode characters, but these are only
    //        available on mac and not
    //        // on iOS as far as I can tell (it's part of Carbon). I didn't see
    //        any better way to do
    //        // this or  any way to get the character codes out of usynergy.
    //        g_keycodeCharUnshifted[ kVK_ANSI_A ]='a';
    //        g_keycodeCharUnshifted[ kVK_ANSI_S ]='s';
    //        g_keycodeCharUnshifted[ kVK_ANSI_D ]='d';
    //        g_keycodeCharUnshifted[ kVK_ANSI_F ]='f';
    //        g_keycodeCharUnshifted[ kVK_ANSI_H ]='h';
    //        g_keycodeCharUnshifted[ kVK_ANSI_G ]='g';
    //        g_keycodeCharUnshifted[ kVK_ANSI_Z ]='z';
    //        g_keycodeCharUnshifted[ kVK_ANSI_X ]='x';
    //        g_keycodeCharUnshifted[ kVK_ANSI_C ]='c';
    //        g_keycodeCharUnshifted[ kVK_ANSI_V ]='v';
    //        g_keycodeCharUnshifted[ kVK_ANSI_B ]='b';
    //        g_keycodeCharUnshifted[ kVK_ANSI_Q ]='q';
    //        g_keycodeCharUnshifted[ kVK_ANSI_W ]='w';
    //        g_keycodeCharUnshifted[ kVK_ANSI_E ]='e';
    //        g_keycodeCharUnshifted[ kVK_ANSI_R ]='r';
    //        g_keycodeCharUnshifted[ kVK_ANSI_Y ]='y';
    //        g_keycodeCharUnshifted[ kVK_ANSI_T ]='t';
    //        g_keycodeCharUnshifted[ kVK_ANSI_1 ]='1';
    //        g_keycodeCharUnshifted[ kVK_ANSI_2 ]='2';
    //        g_keycodeCharUnshifted[ kVK_ANSI_3 ]='3';
    //        g_keycodeCharUnshifted[ kVK_ANSI_4 ]='4';
    //        g_keycodeCharUnshifted[ kVK_ANSI_6 ]='6';
    //        g_keycodeCharUnshifted[ kVK_ANSI_5 ]='5';
    //        g_keycodeCharUnshifted[ kVK_ANSI_Equal ]='=';
    //        g_keycodeCharUnshifted[ kVK_ANSI_9 ]='9';
    //        g_keycodeCharUnshifted[ kVK_ANSI_7 ]='7';
    //        g_keycodeCharUnshifted[ kVK_ANSI_Minus ]='-';
    //        g_keycodeCharUnshifted[ kVK_ANSI_8 ]='8';
    //        g_keycodeCharUnshifted[ kVK_ANSI_0 ]='0';
    //        g_keycodeCharUnshifted[ kVK_ANSI_RightBracket ]=']';
    //        g_keycodeCharUnshifted[ kVK_ANSI_O ]='o';
    //        g_keycodeCharUnshifted[ kVK_ANSI_U ]='u';
    //        g_keycodeCharUnshifted[ kVK_ANSI_LeftBracket ]='[';
    //        g_keycodeCharUnshifted[ kVK_ANSI_I ]='i';
    //        g_keycodeCharUnshifted[ kVK_ANSI_P ]='p';
    //        g_keycodeCharUnshifted[ kVK_ANSI_L ]='l';
    //        g_keycodeCharUnshifted[ kVK_ANSI_J ]='j';
    //        g_keycodeCharUnshifted[ kVK_ANSI_Quote ]='\'';
    //        g_keycodeCharUnshifted[ kVK_ANSI_K ]='k';
    //        g_keycodeCharUnshifted[ kVK_ANSI_Semicolon ]=';';
    //        g_keycodeCharUnshifted[ kVK_ANSI_Backslash ]='\\';
    //        g_keycodeCharUnshifted[ kVK_ANSI_Comma ]=',';
    //        g_keycodeCharUnshifted[ kVK_ANSI_Slash ]='/';
    //        g_keycodeCharUnshifted[ kVK_ANSI_N ]='n';
    //        g_keycodeCharUnshifted[ kVK_ANSI_M ]='m';
    //        g_keycodeCharUnshifted[ kVK_ANSI_Period ]='.';
    //        g_keycodeCharUnshifted[ kVK_ANSI_Grave ]='`';
    //        g_keycodeCharUnshifted[ kVK_ANSI_KeypadDecimal ]='.';
    //        g_keycodeCharUnshifted[ kVK_ANSI_KeypadMultiply ]='*';
    //        g_keycodeCharUnshifted[ kVK_ANSI_KeypadPlus ]='+';
    //        g_keycodeCharUnshifted[ kVK_ANSI_KeypadDivide ]='/';
    //        g_keycodeCharUnshifted[ kVK_ANSI_KeypadEnter ]='\n';
    //        g_keycodeCharUnshifted[ kVK_ANSI_KeypadMinus ]='-';
    //        g_keycodeCharUnshifted[ kVK_ANSI_KeypadEquals ]='=';
    //        g_keycodeCharUnshifted[ kVK_ANSI_Keypad0 ]='0';
    //        g_keycodeCharUnshifted[ kVK_ANSI_Keypad1 ]='1';
    //        g_keycodeCharUnshifted[ kVK_ANSI_Keypad2 ]='2';
    //        g_keycodeCharUnshifted[ kVK_ANSI_Keypad3 ]='3';
    //        g_keycodeCharUnshifted[ kVK_ANSI_Keypad4 ]='4';
    //        g_keycodeCharUnshifted[ kVK_ANSI_Keypad5 ]='5';
    //        g_keycodeCharUnshifted[ kVK_ANSI_Keypad6 ]='6';
    //        g_keycodeCharUnshifted[ kVK_ANSI_Keypad7 ]='7';
    //        g_keycodeCharUnshifted[ kVK_ANSI_Keypad8 ]='8';
    //        g_keycodeCharUnshifted[ kVK_ANSI_Keypad9 ]='9';
    //        g_keycodeCharUnshifted[ kVK_Space ]=' ';
    //
    //        g_keycodeCharShifted[ kVK_ANSI_A ]='A';
    //        g_keycodeCharShifted[ kVK_ANSI_S ]='S';
    //        g_keycodeCharShifted[ kVK_ANSI_D ]='D';
    //        g_keycodeCharShifted[ kVK_ANSI_F ]='F';
    //        g_keycodeCharShifted[ kVK_ANSI_H ]='H';
    //        g_keycodeCharShifted[ kVK_ANSI_G ]='G';
    //        g_keycodeCharShifted[ kVK_ANSI_Z ]='Z';
    //        g_keycodeCharShifted[ kVK_ANSI_X ]='X';
    //        g_keycodeCharShifted[ kVK_ANSI_C ]='C';
    //        g_keycodeCharShifted[ kVK_ANSI_V ]='V';
    //        g_keycodeCharShifted[ kVK_ANSI_B ]='B';
    //        g_keycodeCharShifted[ kVK_ANSI_Q ]='Q';
    //        g_keycodeCharShifted[ kVK_ANSI_W ]='W';
    //        g_keycodeCharShifted[ kVK_ANSI_E ]='E';
    //        g_keycodeCharShifted[ kVK_ANSI_R ]='R';
    //        g_keycodeCharShifted[ kVK_ANSI_Y ]='Y';
    //        g_keycodeCharShifted[ kVK_ANSI_T ]='T';
    //        g_keycodeCharShifted[ kVK_ANSI_1 ]='!';
    //        g_keycodeCharShifted[ kVK_ANSI_2 ]='@';
    //        g_keycodeCharShifted[ kVK_ANSI_3 ]='#';
    //        g_keycodeCharShifted[ kVK_ANSI_4 ]='$';
    //        g_keycodeCharShifted[ kVK_ANSI_6 ]='^';
    //        g_keycodeCharShifted[ kVK_ANSI_5 ]='%';
    //        g_keycodeCharShifted[ kVK_ANSI_Equal ]='+';
    //        g_keycodeCharShifted[ kVK_ANSI_9 ]='(';
    //        g_keycodeCharShifted[ kVK_ANSI_7 ]='&';
    //        g_keycodeCharShifted[ kVK_ANSI_Minus ]='_';
    //        g_keycodeCharShifted[ kVK_ANSI_8 ]='*';
    //        g_keycodeCharShifted[ kVK_ANSI_0 ]=')';
    //        g_keycodeCharShifted[ kVK_ANSI_RightBracket ]='}';
    //        g_keycodeCharShifted[ kVK_ANSI_O ]='O';
    //        g_keycodeCharShifted[ kVK_ANSI_U ]='U';
    //        g_keycodeCharShifted[ kVK_ANSI_LeftBracket ]='{';
    //        g_keycodeCharShifted[ kVK_ANSI_I ]='I';
    //        g_keycodeCharShifted[ kVK_ANSI_P ]='P';
    //        g_keycodeCharShifted[ kVK_ANSI_L ]='L';
    //        g_keycodeCharShifted[ kVK_ANSI_J ]='J';
    //        g_keycodeCharShifted[ kVK_ANSI_Quote ]='\"';
    //        g_keycodeCharShifted[ kVK_ANSI_K ]='K';
    //        g_keycodeCharShifted[ kVK_ANSI_Semicolon ]=':';
    //        g_keycodeCharShifted[ kVK_ANSI_Backslash ]='|';
    //        g_keycodeCharShifted[ kVK_ANSI_Comma ]='<';
    //        g_keycodeCharShifted[ kVK_ANSI_Slash ]='?';
    //        g_keycodeCharShifted[ kVK_ANSI_N ]='N';
    //        g_keycodeCharShifted[ kVK_ANSI_M ]='M';
    //        g_keycodeCharShifted[ kVK_ANSI_Period ]='>';
    //        g_keycodeCharShifted[ kVK_ANSI_Grave ]='~';
    //        g_keycodeCharShifted[ kVK_ANSI_KeypadDecimal ]='.';
    //        g_keycodeCharShifted[ kVK_ANSI_KeypadMultiply ]='*';
    //        g_keycodeCharShifted[ kVK_ANSI_KeypadPlus ]='+';
    //        g_keycodeCharShifted[ kVK_ANSI_KeypadDivide ]='/';
    //        g_keycodeCharShifted[ kVK_ANSI_KeypadEnter ]='\n';
    //        g_keycodeCharShifted[ kVK_ANSI_KeypadMinus ]='-';
    //        g_keycodeCharShifted[ kVK_ANSI_KeypadEquals ]='=';
    //        g_keycodeCharShifted[ kVK_ANSI_Keypad0 ]='0';
    //        g_keycodeCharShifted[ kVK_ANSI_Keypad1 ]='1';
    //        g_keycodeCharShifted[ kVK_ANSI_Keypad2 ]='2';
    //        g_keycodeCharShifted[ kVK_ANSI_Keypad3 ]='3';
    //        g_keycodeCharShifted[ kVK_ANSI_Keypad4 ]='4';
    //        g_keycodeCharShifted[ kVK_ANSI_Keypad5 ]='5';
    //        g_keycodeCharShifted[ kVK_ANSI_Keypad6 ]='6';
    //        g_keycodeCharShifted[ kVK_ANSI_Keypad7 ]='7';
    //        g_keycodeCharShifted[ kVK_ANSI_Keypad8 ]='8';
    //        g_keycodeCharShifted[ kVK_ANSI_Keypad9 ]='9';
    //        g_keycodeCharShifted[ kVK_Space ]=' ';
    //    }

//    void setupImGuiHooks()
//    {
//        ImGuiIO &io = ImGui::GetIO();
//
//        //        setupKeymaps();
//
//        // Account for retina display for glScissor
//        g_displayScale = 1.0f; //[[UIScreen mainScreen] scale];
//
//        ImGuiStyle &style = ImGui::GetStyle();
//        style.TouchExtraPadding = ImVec2(4.0, 4.0);
//
////        io.RenderDrawListsFn = ImGui_ImplIOS_RenderDrawLists;
////        io.SetClipboardTextFn = ImGui_ImplGlfw_SetClipboardText;
////        io.GetClipboardTextFn = ImGui_ImplGlfw_GetClipboardText;
//
//        //        UIPanGestureRecognizer *panRecognizer =
//        //        [[UIPanGestureRecognizer alloc] initWithTarget:self
//        //        action:@selector(viewDidPan:) ]; [self.view
//        //        addGestureRecognizer:panRecognizer];
//        //
//        //        UITapGestureRecognizer *tapRecoginzer =
//        //        [[UITapGestureRecognizer alloc] initWithTarget:self
//        //        action:@selector( viewDidTap:)]; [self.view
//        //        addGestureRecognizer:tapRecoginzer];
//
//        // Fill out the Synergy key map
//        // (for some reason synergy scan codes are off by 1)
//        //        io.KeyMap[ImGuiKey_Tab] = kVK_Tab;//+1;
//        //        io.KeyMap[ImGuiKey_LeftArrow] = kVK_LeftArrow+1;
//        //        io.KeyMap[ImGuiKey_RightArrow] = kVK_RightArrow+1;
//        //        io.KeyMap[ImGuiKey_UpArrow] = kVK_UpArrow+1;
//        //        io.KeyMap[ImGuiKey_DownArrow] = kVK_DownArrow+1;
//        //        io.KeyMap[ImGuiKey_Home] = kVK_Home+1;
//        //        io.KeyMap[ImGuiKey_End] = kVK_End+1;
//        //        io.KeyMap[ImGuiKey_Delete] = kVK_ForwardDelete+1;
//        //        io.KeyMap[ImGuiKey_Backspace] = kVK_Delete+1;
//        //        io.KeyMap[ImGuiKey_Enter] = kVK_Return+1;
//        //        io.KeyMap[ImGuiKey_Escape] = kVK_Escape+1;
//        //        io.KeyMap[ImGuiKey_A] = kVK_ANSI_A+1;
//        //        io.KeyMap[ImGuiKey_C] = kVK_ANSI_C+1;
//        //        io.KeyMap[ImGuiKey_V] = kVK_ANSI_V+1;
//        //        io.KeyMap[ImGuiKey_X] = kVK_ANSI_X+1;
//        //        io.KeyMap[ImGuiKey_Y] = kVK_ANSI_Y+1;
//        //        io.KeyMap[ImGuiKey_Z] = kVK_ANSI_Z+1;
//
//        io.KeyMap[ImGuiKey_Tab] = SDLK_TAB; // Keyboard mapping. ImGui will use
//        // those indices to peek into the
//        // io.KeyDown[] array.
//        io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
//        io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
//        io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
//        io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
//        io.KeyMap[ImGuiKey_PageUp] = SDL_SCANCODE_PAGEUP;
//        io.KeyMap[ImGuiKey_PageDown] = SDL_SCANCODE_PAGEDOWN;
//        io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
//        io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
//        io.KeyMap[ImGuiKey_Delete] = SDLK_DELETE;
//        io.KeyMap[ImGuiKey_Backspace] = SDLK_BACKSPACE;
//        io.KeyMap[ImGuiKey_Enter] = SDLK_RETURN;
//        io.KeyMap[ImGuiKey_Escape] = SDLK_ESCAPE;
//        io.KeyMap[ImGuiKey_A] = SDLK_a;
//        io.KeyMap[ImGuiKey_C] = SDLK_c;
//        io.KeyMap[ImGuiKey_V] = SDLK_v;
//        io.KeyMap[ImGuiKey_X] = SDLK_x;
//        io.KeyMap[ImGuiKey_Y] = SDLK_y;
//        io.KeyMap[ImGuiKey_Z] = SDLK_z;
//    }

//    void WorldDebugDrawer::initImgui() { setupImGuiHooks(); }
//
//    void WorldDebugDrawer::unInitImgui()
//    {
//        if (g_FontTexture)
//        {
//            glDeleteTextures(1, &g_FontTexture);
//            ImGui::GetIO().Fonts->TexID = 0;
//            g_FontTexture = 0;
//        }
//        ImGui::Shutdown();
//    }
//
//    void WorldDebugDrawer::renderImgui() { ImGui::Render(); }

//#if !defined(_WIN32) && !defined(__ANDROID__) && !defined(__EMSCRIPTEN__)
//    static void GetPrimaryIp(char *buffer, size_t buflen)
//    {
//        setenv("LANG", "C", 1);
//        FILE *fp = popen("ifconfig", "r");
//        if (fp)
//        {
//            char *p = NULL, *e;
//            size_t n;
//            while ((getline(&p, &n, fp) > 0) && p)
//            {
//                if ((p = strstr(p, "inet ")))
//                {
//                    p += 5;
//                    if ((p = strchr(p, ':')))
//                    {
//                        ++p;
//                        if ((e = strchr(p, ' ')))
//                        {
//                            *e = '\0';
//                            printf("%s\n", p);
//                        }
//                    }
//                }
//            }
//        }
//        pclose(fp);
//    }
//#endif

//    void WorldDebugDrawer::newFrameImgui()
//    {
//#if !defined(_WIN32) && !defined(__ANDROID__) && !defined(__EMSCRIPTEN__)
//        static char buffer[256];
//        GetPrimaryIp(buffer, 256);
//#endif
//
//        ImGuiIO &io = ImGui::GetIO();
//        ImGuiStyle &style = ImGui::GetStyle();
//
//        if (!g_FontTexture)
//        {
//            ImGui_ImplIOS_CreateDeviceObjects();
//        }
//
//        io.DisplaySize =
//            ImVec2(njli::World::getInstance()->getViewportDimensions().x(),
//                   njli::World::getInstance()->getViewportDimensions().y());
//
//        // Setup time step
//        Uint32 time = SDL_GetTicks();
//        double current_time = time / 1000.0;
//        io.DeltaTime = g_Time > 0.0 ? (float)(current_time - g_Time)
//                                    : (float)(1.0f / 60.0f);
//        g_Time = current_time;
//
//        io.MouseDrawCursor = g_synergyPtrActive;
//        if (g_synergyPtrActive)
//        {
//            style.TouchExtraPadding = ImVec2(0.0, 0.0);
//            io.MousePos = ImVec2(g_mousePosX, g_mousePosY);
//            for (int i = 0; i < 3; i++)
//            {
//                io.MouseDown[i] = g_MousePressed[i];
//            }
//
//            io.MouseWheel = g_mouseWheelY / 500.0;
//        }
//        else
//        {
//#if !defined(__ANDROID__) && !defined(__IPHONEOS__)
//            int mx, my;
//            Uint32 mouseMask = SDL_GetMouseState(&mx, &my);
//            if (SDL_GetWindowFlags(gWindow) & SDL_WINDOW_MOUSE_FOCUS)
//            {
//                io.MousePos = ImVec2((float)mx, (float)my);
//            }
//            else
//            {
//                io.MousePos = ImVec2(-1, -1);
//            }
//
//            // If a mouse press event came, always pass it as "mouse held
//            // this frame", so we don't miss click-release events that are
//            // shorter than 1 frame.
//            io.MouseDown[0] = g_MousePressed[0] ||
//                              (mouseMask & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;
//            io.MouseDown[1] = g_MousePressed[1] ||
//                              (mouseMask & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
//            io.MouseDown[2] = g_MousePressed[2] ||
//                              (mouseMask & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0;
//            g_MousePressed[0] = g_MousePressed[1] = g_MousePressed[2] = false;
//
//            io.MouseWheel = g_MouseWheel;
//            g_MouseWheel = 0.0f;
//            SDL_ShowCursor(io.MouseDrawCursor ? 0 : 1);
//#endif
//        }
//
//        ImGui::NewFrame();
//        ImGuizmo::BeginFrame();
//    }

//#if defined(USE_USYNERGY_LIBRARY)
//    void WorldDebugDrawer::connectSynergyServer(const std::string serverName)
//    {
//        //        self.serverName = serverName;
//        //        g_serverName = serverName;
//        //
//        //        // Init synergy
//        //        NSString *bundleName = [[[NSBundle mainBundle] infoDictionary]
//        //        objectForKey:(NSString*)kCFBundleNameKey];
//        //
//        //        uSynergyInit( &_synergyCtx );
//        //        _synergyCtx.m_clientName = strdup( [bundleName UTF8String] );
//        //        _synergyCtx.m_clientWidth = self.view.bounds.size.width;
//        //        _synergyCtx.m_clientHeight = self.view.bounds.size.height;
//        //
//        //        _synergyCtx.m_connectFunc = ImGui_ConnectFunc;
//        //        _synergyCtx.m_sendFunc = ImGui_SendFunc;
//        //        _synergyCtx.m_receiveFunc = ImGui_RecvFunc;
//        //        _synergyCtx.m_sleepFunc = ImGui_SleepFunc;
//        //        _synergyCtx.m_traceFunc = ImGui_TraceFunc;
//        //        _synergyCtx.m_getTimeFunc = ImGui_GetTimeFunc;
//        //
//        //        _synergyCtx.m_traceFunc = ImGui_TraceFunc;
//        //        _synergyCtx.m_screenActiveCallback =
//        //        ImGui_ScreenActiveCallback; _synergyCtx.m_mouseCallback =
//        //        ImGui_MouseCallback; _synergyCtx.m_keyboardCallback =
//        //        ImGui_KeyboardCallback;
//        //
//        //        _synergyCtx.m_cookie = (uSynergyCookie)&_synergyCtx;
//        //
//        //        // Create a background thread for synergy
//        //        _synergyQueue = dispatch_queue_create( "imgui-usynergy", NULL
//        //        ); dispatch_async( _synergyQueue, ^{
//        //            while (1) {
//        //                uSynergyUpdate( &_synergyCtx );
//        //            }
//        //        });
//    }
//#endif

//    bool WorldDebugDrawer::processSdlEvent(SDL_Event *event)
//    {
//        ImGuiIO &io = ImGui::GetIO();
//        switch (event->type)
//        {
//        case SDL_MOUSEWHEEL:
//        {
//            if (event->wheel.y > 0)
//                g_MouseWheel = 1;
//            if (event->wheel.y < 0)
//                g_MouseWheel = -1;
//            return true;
//        }
//        case SDL_MOUSEBUTTONDOWN:
//        {
//            if (event->button.button == SDL_BUTTON_LEFT)
//                g_MousePressed[0] = true;
//            if (event->button.button == SDL_BUTTON_RIGHT)
//                g_MousePressed[1] = true;
//            if (event->button.button == SDL_BUTTON_MIDDLE)
//                g_MousePressed[2] = true;
//            return true;
//        }
//
//        case SDL_FINGERMOTION:
//        {
//            g_MousePressed[0] = true;
//            float x = event->tfinger.x * gDisplayMode.w;
//            float y = gDisplayMode.h - (event->tfinger.y * gDisplayMode.h);
//            io.MousePos = ImVec2(x, y);
//        }
//            return true;
//        case SDL_FINGERDOWN:
//        {
//            g_MousePressed[0] = true;
//            float x = event->tfinger.x * gDisplayMode.w;
//            float y = gDisplayMode.h - (event->tfinger.y * gDisplayMode.h);
//            io.MousePos = ImVec2(x, y);
//        }
//            return true;
//        case SDL_FINGERUP:
//        {
//            g_MousePressed[0] = false;
//            float x = event->tfinger.x * gDisplayMode.w;
//            float y = gDisplayMode.h - (event->tfinger.y * gDisplayMode.h);
//            io.MousePos = ImVec2(x, y);
//        }
//            return true;
//        case SDL_TEXTINPUT:
//        {
//            ImGuiIO &io = ImGui::GetIO();
//            io.AddInputCharactersUTF8(event->text.text);
//            return true;
//        }
//        case SDL_KEYDOWN:
//        case SDL_KEYUP:
//        {
//            int key = event->key.keysym.sym & ~SDLK_SCANCODE_MASK;
//            io.KeysDown[key] = (event->type == SDL_KEYDOWN);
//            io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
//            io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
//            io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
//            return true;
//        }
//        }
//        return false;
//    }

    //    - (void)viewDidPan: (UIPanGestureRecognizer *)recognizer
    //    {
    //
    //        if ((recognizer.state == UIGestureRecognizerStateBegan) ||
    //            (recognizer.state == UIGestureRecognizerStateChanged))
    //        {
    //            _mouseDown = YES;
    //            _touchPos = [recognizer locationInView:self.view];
    //        }
    //        else
    //        {
    //            _mouseDown = NO;
    //            _touchPos = CGPointMake( -1, -1 );
    //        }
    //    }
    //
    //    - (void)viewDidTap: (UITapGestureRecognizer*)recognizer
    //    {
    //        _touchPos = [recognizer locationInView:self.view];
    //        _mouseTapped = YES;
    //    }

//    static const char *ImGui_ImplGlfw_GetClipboardText(void *user_data)
//    {
//        return SDL_GetClipboardText();
//    }

//    static void ImGui_ImplGlfw_SetClipboardText(void *user_data,
//                                                const char *text)
//    {
//        SDL_SetClipboardText(text);
//    }

    // This is the main rendering function that you have to implement and
    // provide to ImGui (via setting up 'RenderDrawListsFn' in the ImGuiIO
    // structure) If text or lines are blurry when integrating ImGui in your
    // engine:
    // - in your Render function, try translating your projection matrix by
    // (0.5f,0.5f) or (0.375f,0.375f)
    // NOTE: this is copied pretty much entirely from the opengl3_example, with
    // only minor changes for ES
//    static void ImGui_ImplIOS_RenderDrawLists(ImDrawData *draw_data)
//    {
//        // Setup render state: alpha-blending enabled, no face culling, no depth
//        // testing, scissor enabled
//        GLint last_program, last_texture;
//        glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
//        glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
//        glEnable(GL_BLEND);
//        glBlendEquation(GL_FUNC_ADD);
//        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//
//        glDisable(GL_CULL_FACE);
//        glDisable(GL_DEPTH_TEST);
//        glEnable(GL_SCISSOR_TEST);
//        glActiveTexture(GL_TEXTURE0);
//
//        // Setup orthographic projection matrix
//        const float width = ImGui::GetIO().DisplaySize.x;
//        const float height = ImGui::GetIO().DisplaySize.y;
//        const float ortho_projection[4][4] = {
//            {2.0f / width, 0.0f, 0.0f, 0.0f},
//            {0.0f, 2.0f / -height, 0.0f, 0.0f},
//            {0.0f, 0.0f, -1.0f, 0.0f},
//            {-1.0f, 1.0f, 0.0f, 1.0f},
//        };
//        glUseProgram(g_ShaderHandle);
//        glUniform1i(g_AttribLocationTex, 0);
//        glUniformMatrix4fv(g_AttribLocationProjMtx, 1, GL_FALSE,
//                           &ortho_projection[0][0]);
//        glBindVertexArray_NJLIC(g_VaoHandle);
//
//        for (int n = 0; n < draw_data->CmdListsCount; n++)
//        {
//            const ImDrawList *cmd_list = draw_data->CmdLists[n];
//            const ImDrawIdx *idx_buffer_offset = 0;
//
//            glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);
//            glBufferData(
//                GL_ARRAY_BUFFER,
//                (GLsizeiptr)cmd_list->VtxBuffer.size() * sizeof(ImDrawVert),
//                (GLvoid *)&cmd_list->VtxBuffer.front(), GL_STREAM_DRAW);
//
//            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ElementsHandle);
//            glBufferData(
//                GL_ELEMENT_ARRAY_BUFFER,
//                (GLsizeiptr)cmd_list->IdxBuffer.size() * sizeof(ImDrawIdx),
//                (GLvoid *)&cmd_list->IdxBuffer.front(), GL_STREAM_DRAW);
//
//            glEnableVertexAttribArray(g_AttribLocationPosition);
//            glEnableVertexAttribArray(g_AttribLocationUV);
//            glEnableVertexAttribArray(g_AttribLocationColor);
//
//#define OFFSETOF(TYPE, ELEMENT) ((size_t) & (((TYPE *)0)->ELEMENT))
//            glVertexAttribPointer(g_AttribLocationPosition, 2, GL_FLOAT,
//                                  GL_FALSE, sizeof(ImDrawVert),
//                                  (GLvoid *)OFFSETOF(ImDrawVert, pos));
//            glVertexAttribPointer(g_AttribLocationUV, 2, GL_FLOAT, GL_FALSE,
//                                  sizeof(ImDrawVert),
//                                  (GLvoid *)OFFSETOF(ImDrawVert, uv));
//            glVertexAttribPointer(g_AttribLocationColor, 4, GL_UNSIGNED_BYTE,
//                                  GL_TRUE, sizeof(ImDrawVert),
//                                  (GLvoid *)OFFSETOF(ImDrawVert, col));
//#undef OFFSETOF
//            for (const ImDrawCmd *pcmd = cmd_list->CmdBuffer.begin();
//                 pcmd != cmd_list->CmdBuffer.end(); pcmd++)
//            {
//                if (pcmd->UserCallback)
//                {
//                    pcmd->UserCallback(cmd_list, pcmd);
//                }
//                else
//                {
//                    glBindTexture(GL_TEXTURE_2D,
//                                  (GLuint)(intptr_t)pcmd->TextureId);
//                    //                    glScissor((int)pcmd->ClipRect.x,
//                    //                              (int)(fb_height
//                    //                              -
//                    //                              pcmd->ClipRect.w),
//                    //                              (int)(pcmd->ClipRect.z
//                    //                              -
//                    //                              pcmd->ClipRect.x),
//                    //                              (int)(pcmd->ClipRect.w
//                    //                              -
//                    //                              pcmd->ClipRect.y));
//
//                    glScissor(
//                        (int)(pcmd->ClipRect.x * g_displayScale),
//                        (int)((height - pcmd->ClipRect.w) * g_displayScale),
//                        (int)((pcmd->ClipRect.z - pcmd->ClipRect.x) *
//                              g_displayScale),
//                        (int)((pcmd->ClipRect.w - pcmd->ClipRect.y) *
//                              g_displayScale));
//                    glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount,
//                                   GL_UNSIGNED_SHORT, idx_buffer_offset);
//                }
//                idx_buffer_offset += pcmd->ElemCount;
//            }
//        }
//
//        // Restore modified state
//        glBindVertexArray_NJLIC(0);
//        glBindBuffer(GL_ARRAY_BUFFER, 0);
//        glEnable(GL_CULL_FACE);
//        glEnable(GL_DEPTH_TEST);
//        glUseProgram(last_program);
//        glDisable(GL_SCISSOR_TEST);
//        glBindTexture(GL_TEXTURE_2D, last_texture);
//    }

//    void ImGui_ImplIOS_CreateFontsTexture()
//    {
//        // Build texture atlas
//        ImGuiIO &io = ImGui::GetIO();
//        unsigned char *pixels;
//        int width, height;
//        io.Fonts->GetTexDataAsRGBA32(
//            &pixels, &width, &height); // Load as RGBA 32-bits for OpenGL3 demo
//        // because it is more likely to be compatible
//        // with user's existing shader.
//
//        // Upload texture to graphics system
//        GLint last_texture;
//        glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
//        glGenTextures(1, &g_FontTexture);
//        glBindTexture(GL_TEXTURE_2D, g_FontTexture);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
//                     GL_UNSIGNED_BYTE, pixels);
//
//        // Store our identifier
//        io.Fonts->TexID = (void *)(intptr_t)g_FontTexture;
//
//        // Restore state
//        glBindTexture(GL_TEXTURE_2D, last_texture);
//    }

//    bool ImGui_ImplIOS_CreateDeviceObjects()
//    {
//        const GLchar *vertex_shader =
//            "#version 100\n"
//            "uniform mat4 ProjMtx;\n"
//            "attribute vec2 Position;\n"
//            "attribute vec2 UV;\n"
//            "attribute vec4 Color;\n"
//            "varying vec2 Frag_UV;\n"
//            "varying vec4 Frag_Color;\n"
//            "void main()\n"
//            "{\n"
//            "    Frag_UV = UV;\n"
//            "    Frag_Color = Color;\n"
//            "    gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
//            "}\n";
//
//        const GLchar *fragment_shader =
//            "#version 100\n"
//#if defined(__GL_ES2__) || defined(__GL_ES2__)
//
//            "#ifdef GL_ES\n"
//            "      precision highp float;\n"
//            "#endif\n"
//#endif
//            "uniform sampler2D Texture;\n"
//            "varying vec2 Frag_UV;\n"
//            "varying vec4 Frag_Color;\n"
//            "void main()\n"
//            "{\n"
//            "    gl_FragColor = Frag_Color * texture2D( Texture, Frag_UV.st);\n"
//            "}\n";
//
//        g_ShaderHandle = glCreateProgram();
//        g_VertHandle = glCreateShader(GL_VERTEX_SHADER);
//        g_FragHandle = glCreateShader(GL_FRAGMENT_SHADER);
//        glShaderSource(g_VertHandle, 1, &vertex_shader, 0);
//        glShaderSource(g_FragHandle, 1, &fragment_shader, 0);
//        glCompileShader(g_VertHandle);
//
//#if !(defined(NDEBUG))
//        GLint logLength;
//        glGetShaderiv(g_VertHandle, GL_INFO_LOG_LENGTH, &logLength);
//        if (logLength > 0)
//        {
//            GLchar *log = (GLchar *)malloc(logLength);
//            glGetShaderInfoLog(g_VertHandle, logLength, &logLength, log);
//            SDL_LogInfo(SDL_LOG_CATEGORY_TEST, "%s", log);
//            free(log);
//        }
//#endif
//
//        glCompileShader(g_FragHandle);
//
//#if !(defined(NDEBUG))
//        glGetShaderiv(g_FragHandle, GL_INFO_LOG_LENGTH, &logLength);
//        if (logLength > 0)
//        {
//            GLchar *log = (GLchar *)malloc(logLength);
//            glGetShaderInfoLog(g_FragHandle, logLength, &logLength, log);
//            SDL_LogInfo(SDL_LOG_CATEGORY_TEST, "%s", log);
//            free(log);
//        }
//#endif
//
//        glAttachShader(g_ShaderHandle, g_VertHandle);
//        glAttachShader(g_ShaderHandle, g_FragHandle);
//        glLinkProgram(g_ShaderHandle);
//
//        g_AttribLocationTex = glGetUniformLocation(g_ShaderHandle, "Texture");
//        g_AttribLocationProjMtx =
//            glGetUniformLocation(g_ShaderHandle, "ProjMtx");
//        g_AttribLocationPosition =
//            glGetAttribLocation(g_ShaderHandle, "Position");
//        g_AttribLocationUV = glGetAttribLocation(g_ShaderHandle, "UV");
//        g_AttribLocationColor = glGetAttribLocation(g_ShaderHandle, "Color");
//
//        glGenBuffers(1, &g_VboHandle);
//        glGenBuffers(1, &g_ElementsHandle);
//
//        glGenVertexArrays_NJLIC(1, &g_VaoHandle);
//        glBindVertexArray_NJLIC(g_VaoHandle);
//        glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);
//        glEnableVertexAttribArray(g_AttribLocationPosition);
//        glEnableVertexAttribArray(g_AttribLocationUV);
//        glEnableVertexAttribArray(g_AttribLocationColor);
//
//#define OFFSETOF(TYPE, ELEMENT) ((size_t) & (((TYPE *)0)->ELEMENT))
//        glVertexAttribPointer(g_AttribLocationPosition, 2, GL_FLOAT, GL_FALSE,
//                              sizeof(ImDrawVert),
//                              (GLvoid *)OFFSETOF(ImDrawVert, pos));
//        glVertexAttribPointer(g_AttribLocationUV, 2, GL_FLOAT, GL_FALSE,
//                              sizeof(ImDrawVert),
//                              (GLvoid *)OFFSETOF(ImDrawVert, uv));
//        glVertexAttribPointer(g_AttribLocationColor, 4, GL_UNSIGNED_BYTE,
//                              GL_TRUE, sizeof(ImDrawVert),
//                              (GLvoid *)OFFSETOF(ImDrawVert, col));
//#undef OFFSETOF
//        glBindVertexArray_NJLIC(0);
//        glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//        ImGui_ImplIOS_CreateFontsTexture();
//
//        return true;
//    }
//} // namespace njli
