#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>

#include "shader_manager.hpp"


namespace SDFont {


string ShaderManager::fileToString( string path )
{
    ifstream is( path );

    if ( !is ) {

        cerr << "failed to open:[" << path << "]\n";
        return "";
    }

    string code;

    string line = "";

    while ( getline( is, line ) ) {

        code += "\n" + line;
    }

    is.close();

    return code;
}


bool ShaderManager::compile (const string& code, GLuint id )
{
    char const * strp = code.c_str();

    glShaderSource  ( id, 1, &strp , NULL ) ;
    glCompileShader ( id ) ;

    GLint res = GL_FALSE;
    glGetShaderiv( id, GL_COMPILE_STATUS, &res );

    int infoLogLength;
    glGetShaderiv( id, GL_INFO_LOG_LENGTH, &infoLogLength );

    if ( res == GL_FALSE && infoLogLength > 0 ) {

        char* message = new char[infoLogLength + 1];
        if ( message != nullptr ) {

            glGetShaderInfoLog( id, infoLogLength, NULL, message );

            cerr << message << "\n";

            delete[] message;
        }

        return false;
    }

    return true;
}


bool ShaderManager::link( GLuint prog, GLuint vertex, GLuint fragment )
{

    glAttachShader( prog, vertex   );
    glAttachShader( prog, fragment );
    glLinkProgram ( prog );

    GLint res = GL_FALSE;
    glGetProgramiv( prog, GL_LINK_STATUS, &res );

    int infoLogLength;
    glGetProgramiv( prog, GL_INFO_LOG_LENGTH, &infoLogLength);

    if ( res == GL_FALSE && infoLogLength > 0 ) {

        char* message = new char[infoLogLength + 1];
        if ( message != nullptr ) {

            glGetProgramInfoLog( prog, infoLogLength, NULL, message );
            std::cerr << message << "\n";
            delete[] message;

        }

        glDetachShader( prog, vertex   );
        glDetachShader( prog, fragment );

        return false;
    }

    glDetachShader( prog, vertex   );
    glDetachShader( prog, fragment );

    return true;
}


bool ShaderManager::loadShaderPair (
    string  vertexPath,
    string  fragmentPath,
    GLuint& progID
) {

    string vertexShaderCode   = fileToString( vertexPath   ) ;
    string fragmentShaderCode = fileToString( fragmentPath ) ;

    if ( vertexShaderCode == "" || fragmentShaderCode == "" ) {
        return false;
    }

    GLuint vertexShaderID   = glCreateShader( GL_VERTEX_SHADER   );
    GLuint fragmentShaderID = glCreateShader( GL_FRAGMENT_SHADER );

    if ( !compile( vertexShaderCode, vertexShaderID ) ) {

        glDeleteShader( vertexShaderID   );
        glDeleteShader( fragmentShaderID );
        return false;
    }

    if ( !compile( fragmentShaderCode, fragmentShaderID ) ) {

        glDeleteShader( vertexShaderID   );
        glDeleteShader( fragmentShaderID );
        return false;
    }

    progID = glCreateProgram();
    if ( !link( progID, vertexShaderID, fragmentShaderID ) ) {

        glDeleteShader( vertexShaderID   );
        glDeleteShader( fragmentShaderID );
        glDeleteProgram( progID );
        return false;
    }

    glDeleteShader( vertexShaderID   );
    glDeleteShader( fragmentShaderID );

    return true;
}


void ShaderManager::unloadShaderPair( GLuint progID )
{
    glDeleteProgram( progID );
}


} // namespace SDFont

