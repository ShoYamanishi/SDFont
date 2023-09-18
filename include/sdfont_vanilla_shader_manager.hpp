#ifndef __SDFONT_VANILLA_SHADER_MANAGER_HPP__
#define __SDFONT_VANILLA_SHADER_MANAGER_HPP__

#include <glm/glm.hpp>
#include <GL/glew.h>

#include "sdfont_shader_manager.hpp"

namespace SDFont {

class VanillaShaderManager : public ShaderManager {

  public:

    VanillaShaderManager( GLuint textureObjectName, GLuint textureActiveNum );

    virtual ~VanillaShaderManager();

    void load() override;

    void draw(
        float*     attributes,
        int        attrLen,
        GLuint*    indices,
        int        indLen,
        int        effect,
        bool       useLight,
        float      lowThreshold,
        float      highThreshold,
        float      smoothing,
        glm::vec3& baseColor,
        glm::vec3& borderColor,
        glm::mat4& P,
        glm::mat4& M,
        glm::mat4& V,
        glm::vec3& lightWCS

    );

    virtual void unload() override;

  protected:

    GLuint mVertexBuffer;
    GLuint mVertexArray;
    GLuint mIndexBuffer;

    GLuint mTextureObjectName;
    GLuint mTextureUniform;
    GLint  mTextureActiveNum;

    GLuint mUniformP;
    GLuint mUniformM;
    GLuint mUniformV;
    GLuint mUniformLightWCS;

    GLuint mUniformLowThreshold;
    GLuint mUniformHighThreshold;
    GLuint mUniformSmoothing;
    GLuint mUniformEffect;
    GLuint mUniformUseLight;
    GLuint mUniformBaseColor;
    GLuint mUniformBorderColor;

    GLuint mVertexSlot;
    GLuint mNormalSlot;
    GLuint mTexCoordSlot;

    static const char* VERTEX_STR;
    static const char* FRAGMENT_STR;
};


} // namespace SDFont

#endif/*__SDFONT_VANILLA_SHADER_MANAGER_HPP__*/
