#ifndef __SDFONT_SHADER_MANAGER_HPP__
#define __SDFONT_SHADER_MANAGER_HPP__

#include <string>

#ifdef __MAC_LIB__

  #define GL_SILENCE_DEPRECATION
  #include <gl.h>

#else

  #include <GL/gl.h>

#endif

using namespace std;

namespace SDFont {

class ShaderManager {

  public:

    ShaderManager () : mOK(false), mProgramID(0) { /*loadShaders();*/ }

    virtual ~ShaderManager () { unloadShaders(); }


    virtual void load()
    {
        // glGetUniformLocation ( mProgramID, "VAR1" );
        // glGetAttribLocation  ( mProgramID, "VAR2" );
        // glEnableVertexAttribArray( slot );

        // glUniform**()
    }

    virtual void unload()
    {
        // glDisableVertexAttribArray( slot );
    }

  protected:

    void loadShaders( string vertexPath, string fragmentPath )
    {
        mOK = loadShaderPair( vertexPath, fragmentPath, mProgramID );
    }

    void loadShadersFromStrings( string vertexStr, string fragmentStr )
    {
        mOK = loadShaderPairFromStrings( vertexStr, fragmentStr, mProgramID );
    }

    void unloadShaders()
    {
        if ( mOK ) {
            unloadShaderPair( mProgramID );
            mOK = false;
        }

    }

    bool   mOK;
    GLuint mProgramID;

  private:

    static bool loadShaderPair (
        string  vertexPath,
        string  fragmentPath,
        GLuint& progID
    );

    static bool loadShaderPairFromStrings (
        string  vertexShaderCode,
        string  fragmentShaderCode,
        GLuint& progID
    );

    static string fileToString ( string path );

    static bool   compile (const string& code, GLuint id );


    static bool   link ( GLuint prog, GLuint vertex, GLuint frag );

    static void   unloadShaderPair ( GLuint progID );

};

} // namespace SDFont

#endif/*__SDFONT_SHADER_MANAGER_HPP__*/
