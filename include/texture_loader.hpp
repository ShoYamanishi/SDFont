#ifndef __TEXTURE_LOADER_HPP__
#define __TEXTURE_LOADER_HPP__

#include <string>
#include <GL/gl.h>

#ifdef USE_LIBPNG
#include <png.h>
#endif

using namespace std;

namespace SDFont {

class TextureLoader {

  public:

#ifdef USE_LIBPNG
    TextureLoader ( string filePath );
#endif

    TextureLoader ( GLubyte* pixMap, int width );

    virtual ~TextureLoader();

    bool   isOK() const { return mOk; }
    GLuint GLtexture() const { return mGLtexture; }
    int    size() const { return mWidth ; }

  private:

#ifdef USE_LIBPNG

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

#endif

    void generateOpenGLTexture();

    bool          mOk ;
    GLubyte*      mPixMap ;
    bool          mPixMapAllocated ;
    unsigned long mWidth ;
    GLuint        mGLtexture ;

};

} // namespace SDFont

#endif /*__TEXTURE_LOADER_HPP__*/
