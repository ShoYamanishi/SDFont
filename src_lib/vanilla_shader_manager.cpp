#include "vanilla_shader_manager.hpp"

namespace SDFont {


VanillaShaderManager::VanillaShaderManager(
    GLuint textureObjectName,
    GLuint textureActiveNum
):

    ShaderManager      (),
    mTextureObjectName ( textureObjectName ),
    mTextureUniform    ( 0                 ),
    mTextureActiveNum  ( textureActiveNum  )

{

    loadShaders( "shaders/VanillaSignedDistFontVertex.glsl",
                 "shaders/VanillaSignedDistFontFragment.glsl" );

    glGenVertexArrays ( 1, &mVertexArray  );
    glGenBuffers      ( 1, &mVertexBuffer );
    glGenBuffers      ( 1, &mIndexBuffer  );
}


VanillaShaderManager::~VanillaShaderManager() {

    glDeleteVertexArrays ( 1, &mVertexArray  );
    glDeleteBuffers      ( 1, &mVertexBuffer );
    glDeleteBuffers      ( 1, &mIndexBuffer  );

}


void VanillaShaderManager::load()
{

    glUseProgram( mProgramID );

    mTextureUniform = glGetUniformLocation ( mProgramID, "fontTexture");
    mVertexSlot     = glGetAttribLocation  ( mProgramID, "vertexLCS"  );
    mNormalSlot     = glGetAttribLocation  ( mProgramID, "normalLCS"  );
    mTexCoordSlot   = glGetAttribLocation  ( mProgramID, "TexCoordIn" );

    glActiveTexture ( GL_TEXTURE0 + mTextureActiveNum     );

    glBindTexture   ( GL_TEXTURE_2D,   mTextureObjectName );

    glUniform1i     ( mTextureUniform, mTextureActiveNum  );

    mUniformEffect        = glGetUniformLocation( mProgramID, "effect"       );
    mUniformUseLight      = glGetUniformLocation( mProgramID, "useLight"     );
    mUniformLowThreshold  = glGetUniformLocation( mProgramID, "lowThreshold" );
    mUniformHighThreshold = glGetUniformLocation( mProgramID, "highThreshold");
    mUniformSmoothing     = glGetUniformLocation( mProgramID, "smoothing"    );
    mUniformBaseColor     = glGetUniformLocation( mProgramID, "baseColor"    );
    mUniformBorderColor   = glGetUniformLocation( mProgramID, "borderColor"  );
    mUniformP             = glGetUniformLocation( mProgramID, "P"            );
    mUniformM             = glGetUniformLocation( mProgramID, "M"            );
    mUniformV             = glGetUniformLocation( mProgramID, "V"            );
    mUniformLightWCS      = glGetUniformLocation( mProgramID, "lightWCS"     );

}


void VanillaShaderManager::draw(

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

) {

    glUniform1i        ( mUniformEffect,        effect                );
    glUniform1i        ( mUniformUseLight,      useLight              );
    glUniform1f        ( mUniformLowThreshold,  lowThreshold          );
    glUniform1f        ( mUniformHighThreshold, highThreshold         );
    glUniform1f        ( mUniformSmoothing,     smoothing             );
    glUniform3fv       ( mUniformBaseColor,     1, &baseColor[0]      );
    glUniform3fv       ( mUniformBorderColor,   1, &borderColor[0]    );
    glUniformMatrix4fv ( mUniformP,             1, GL_FALSE, &P[0][0] );
    glUniformMatrix4fv ( mUniformM,             1, GL_FALSE, &M[0][0] );
    glUniformMatrix4fv ( mUniformV,             1, GL_FALSE, &V[0][0] );
    glUniform3fv       ( mUniformLightWCS,      1, &lightWCS[0]       );

    glBindBuffer( GL_ARRAY_BUFFER, mVertexBuffer );

    glBindVertexArray( mVertexArray );

    glBufferData( GL_ARRAY_BUFFER,
                  sizeof(float) * attrLen,
                  attributes,
                  GL_STATIC_DRAW        );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer );

    glBufferData( GL_ELEMENT_ARRAY_BUFFER,
                  sizeof(GLuint) * indLen,
                  indices,
                  GL_STATIC_DRAW        );

    glEnable     ( GL_BLEND );

    glDepthMask  ( GL_FALSE );

    glBlendFunc  ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    mVertexSlot     = glGetAttribLocation  ( mProgramID, "vertexLCS"  );

    mNormalSlot     = glGetAttribLocation  ( mProgramID, "normalLCS"  );

    mTexCoordSlot   = glGetAttribLocation  ( mProgramID, "texCoordIn" );

    glEnableVertexAttribArray( mVertexSlot );

    glEnableVertexAttribArray( mNormalSlot );

    glEnableVertexAttribArray( mTexCoordSlot );

    glVertexAttribPointer( mVertexSlot,
                           3,
                           GL_FLOAT,
                           GL_FALSE,
                           sizeof(float) * 8,
                           0                 );

    glVertexAttribPointer( mNormalSlot,
                           3,
                           GL_FLOAT,
                           GL_FALSE,
                           sizeof(float) * 8,
                           (GLvoid*) (sizeof(float) * 3) );

    glVertexAttribPointer( mTexCoordSlot,
                           2,
                           GL_FLOAT,
                           GL_FALSE,
                           sizeof(float) * 8,
                           (GLvoid*) (sizeof(float) * 6) );

    glDrawElements( GL_TRIANGLES, indLen, GL_UNSIGNED_INT, (GLvoid*)0 );

    glDisableVertexAttribArray( mVertexSlot   );
    glDisableVertexAttribArray( mNormalSlot   );
    glDisableVertexAttribArray( mTexCoordSlot );

}

void VanillaShaderManager::unload() { }

} // namespace SDFont

