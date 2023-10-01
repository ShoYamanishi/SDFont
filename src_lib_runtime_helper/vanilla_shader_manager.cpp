#include "sdfont/runtime_helper/vanilla_shader_manager.hpp"

namespace SDFont {


const char* VanillaShaderManager::VERTEX_STR = "#version 330 core\n\
\n\
in vec3 vertexLCS;\n\
in vec3 normalLCS;\n\
in vec2 texCoordIn;\n\
\n\
uniform mat4 P;\n\
uniform mat4 M;\n\
uniform mat4 V;\n\
uniform vec3 lightWCS;\n\
\n\
out vec3 vertexWCS;\n\
out vec2 texCoordOut;\n\
out vec3 normalECS;\n\
out vec3 vertexToEyeECS;\n\
out vec3 vertexToLightECS;\n\
\n\
\n\
void main() {\n\
\n\
    mat4 MV  = V * M;\n\
\n\
    mat4 MVP = P * MV;\n\
\n\
    gl_Position      = ( MVP * vec4( vertexLCS, 1.0 ) );\n\
\n\
    texCoordOut      = texCoordIn;\n\
\n\
    vertexWCS        = ( M   * vec4( vertexLCS, 1.0 ) ).xyz;\n\
\n\
    vec3 vertexECS   = ( MV  * vec4( vertexLCS, 1.0 ) ).xyz;\n\
\n\
    normalECS        = ( MV  * vec4( normalLCS, 0.0 ) ).xyz;\n\
\n\
    vec3 lightECS    = ( V   * vec4( lightWCS,  1.0 ) ).xyz;\n\
\n\
    vertexToEyeECS   = vec3(0,0,0) - vertexECS;\n\
\n\
    vertexToLightECS = lightECS - vertexECS;\n\
\n\
}\n\
";


const char* VanillaShaderManager::FRAGMENT_STR = "#version 330 core\n\
\n\
precision mediump float;\n\
\n\
in vec2 texCoordOut;\n\
in vec3 vertexWCS;\n\
in vec3 normalECS;\n\
in vec3 vertexToEyeECS;\n\
in vec3 vertexToLightECS;\n\
\n\
out vec4 color;\n\
\n\
uniform sampler2D fontTexture;\n\
uniform int       effect;\n\
uniform bool      useLight;\n\
uniform float     lowThreshold;\n\
uniform float     highThreshold;\n\
uniform float     smoothing;\n\
uniform vec3      baseColor;\n\
uniform vec3      borderColor;\n\
\n\
void main (void) {\n\
\n\
    if ( effect == 0 ) {\n\
\n\
        // Raw output with interpolation.\n\
\n\
        color.rgb = baseColor;\n\
        color.a   = texture( fontTexture, texCoordOut ).r;\n\
    }\n\
    else if ( effect == 1 ) {\n\
\n\
        // Softened edge.\n\
\n\
        color.rgb = baseColor;\n\
        color.a   = smoothstep( 0.5 - smoothing,\n\
                                0.5 + smoothing,\n\
                                texture( fontTexture, texCoordOut ).r );\n\
    }\n\
    else if ( effect == 2 ) {\n\
\n\
        // Sharp edge.\n\
\n\
        float alpha = texture(fontTexture, texCoordOut).r;\n\
\n\
        if ( alpha >= lowThreshold ) {\n\
\n\
            color.rgb = baseColor;\n\
            color.a   = 1.0;\n\
        }\n\
        else {\n\
\n\
            color.rgb = baseColor;\n\
            color.a   = 0.0;\n\
        }\n\
    }\n\
    else if ( effect == 3 ) {\n\
\n\
        // Sharp edge with outer glow.\n\
\n\
        float alpha = texture(fontTexture, texCoordOut).r;\n\
\n\
        if ( alpha >= lowThreshold ) {\n\
\n\
            color.rgb = baseColor;\n\
            color.a   = 1.0;\n\
        }\n\
        else {\n\
\n\
            color.rgb = borderColor;\n\
            color.a   = alpha;\n\
        }\n\
    }\n\
    else if ( effect == 4 ) {\n\
\n\
        // With border.\n\
\n\
        float alpha = texture(fontTexture, texCoordOut).r;\n\
\n\
        if ( alpha >= lowThreshold && alpha <= highThreshold ) {\n\
\n\
            color.rgb = borderColor;\n\
            color.a   = 1.0;\n\
        }\n\
        else if ( alpha >= highThreshold ) {\n\
\n\
            color.rgb = baseColor;\n\
            color.a   = 1.0;\n\
        }\n\
        else {\n\
\n\
            color.rgb = baseColor;\n\
            color.a = 0.0;\n\
        }\n\
    }\n\
    else if ( effect == 5 ) {\n\
        // Softened edge.\n\
        float alpha = texture(fontTexture, texCoordOut).r;\n\
        color.rgb = baseColor;\n\
\n\
        if ( alpha < 0.5) {\n\
            color.a = smoothstep( 0.5 - smoothing,\n\
                                  0.5 + smoothing, \n\
                                  alpha            );\n\
        }\n\
        else {\n\
            color.a = 0.5 -  smoothstep( 0.5 - smoothing,\n\
                                         0.5 + smoothing,\n\
                                         0.75 * alpha     ); \n\
        }\n\
    }\n\
    else {\n\
\n\
        // Rect box for debugging\n\
\n\
        color.rgb = baseColor;\n\
        color.a   = 1.0;\n\
\n\
    }\n\
\n\
    float dist = distance( vertexToLightECS, vec3( 0.0, 0.0, 0.0 ) );\n\
\n\
    if ( useLight ) {\n\
\n\
        color.rgb = color.rgb / sqrt(dist);\n\
    }\n\
}\n\
";


VanillaShaderManager::VanillaShaderManager(
    GLuint textureObjectName,
    GLuint textureActiveNum
):

    ShaderManager      (),
    mTextureObjectName ( textureObjectName ),
    mTextureUniform    ( 0                 ),
    mTextureActiveNum  ( textureActiveNum  )

{
    loadShadersFromStrings( VERTEX_STR, FRAGMENT_STR );

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

