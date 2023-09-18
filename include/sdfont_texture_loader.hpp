#ifndef __SDFONT_TEXTURE_LOADER_HPP__
#define __SDFONT_TEXTURE_LOADER_HPP__

#include <string>

#ifdef __MAC_LIB__

  #define GL_SILENCE_DEPRECATION
  #include <gl.h>

#else

  #include <GL/gl.h>

#endif

#include <png.h>

using namespace std;

namespace SDFont {

class TextureLoader {

  public:

    TextureLoader ( string filePath );
    TextureLoader ( GLubyte* pixMap, int width );

    virtual ~TextureLoader();

    bool   isOK() const { return mOk; }
    GLuint GLtexture() const { return mGLtexture; }
    int    size() const { return mWidth ; }

  private:

    static bool checkPNG(
        int width,
        int height,
        int color,
        int interlace,
        int depth
    );

    static bool loadPngImage(
        string         filePath,
        unsigned long& width,
        GLubyte**      data
    );

    void generateOpenGLTexture();

    bool          mOk ;
    GLubyte*      mPixMap ;
    bool          mPixMapAllocated ;
    unsigned long mWidth ;
    GLuint        mGLtexture ;

};

} // namespace SDFont

#endif /*__SDFONT_TEXTURE_LOADER_HPP__*/
