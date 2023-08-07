#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

#include "texture_loader.hpp"

namespace SDFont {

TextureLoader::TextureLoader ( GLubyte* pixMap, int width ):

    mOk             ( false  ),
    mPixMap         ( pixMap ),
    mPixMapAllocated( false  ),
    mWidth          ( width  )

{
    mOk = true;
    generateOpenGLTexture();
}


TextureLoader::~TextureLoader ()
{
    if ( mPixMapAllocated && mPixMap != nullptr ) {

        free ( mPixMap );
    }   
}


void TextureLoader::generateOpenGLTexture()
{

    glGenTextures( 1, &mGLtexture );

    glBindTexture( GL_TEXTURE_2D, mGLtexture );

    glTexImage2D( GL_TEXTURE_2D,
                  0,
                  GL_RGBA,
                  mWidth,
                  mWidth,
                  0,
                  GL_RGBA,
                  GL_UNSIGNED_BYTE,
                  mPixMap            );

    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_REPEAT  );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_REPEAT  );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR  );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR  );

}


#ifdef USE_LIBPNG

#include "png.h"

TextureLoader::TextureLoader( string filePath ):
    mOk             ( false   ),
    mPixMap         ( nullptr ),
    mPixMapAllocated( true    ),
    mWidth          ( 0       )
{
    mOk = loadPngImage( filePath, mWidth, &mPixMap );

    if (mOk) {

        generateOpenGLTexture();
    }
}


bool TextureLoader::checkPNG(
    int width,
    int height,
    int color,
    int interlace,
    int depth
) {
    if ( width != height ) {

        return false;
    }

    bool goodWidth = false ;

    for ( long shift = 1 ; shift < 32 ; shift++ ) {

        long power2 = ( 1 << shift ) ;

        if ( width == power2 ) {

            goodWidth = true;
            break;
        }
    }

    if (!goodWidth) {

        return false;
    }

    if ( color != PNG_COLOR_TYPE_RGB_ALPHA ) {

        return false;
    }

    if ( interlace != PNG_INTERLACE_NONE ) {

        return false;
    }

    if ( depth != 8 ) {
        return false;
    }

    return true;
}


/** @brief load a PNG image file.
 *
 *  @param filepath (in):  path to the PNG file
 *
 *  @param width     (out): from png_get_IHDR(). upto 2^31
 *
 *  @param data      (out): the pixmap data loaded
 *
 *  @reference https://gist.github.com/mortennobel/5299151
 */
bool TextureLoader::loadPngImage(

    string         filePath,
    unsigned long& width,
    GLubyte**      data

) {

    FILE* fp = fopen( filePath.c_str(), "rb" );

    if (fp == nullptr ) {

        cerr << "Can not open [" << filePath << "\n";
        return false;
    }

    png_structp pngStruct = png_create_read_struct (

        PNG_LIBPNG_VER_STRING,
        NULL,
        NULL,
        NULL
    );

    if ( pngStruct == nullptr ) {

        cerr << "error in png_create_read_struct\n";
        fclose(fp);
        return false;
    }

    png_infop pngInfo = png_create_info_struct( pngStruct );

    if ( pngInfo == NULL ) {

        cerr << "error in png_create_info_struct\n";
        fclose(fp);
        png_destroy_read_struct( &pngStruct, NULL, NULL );
        return false;
    }

    if ( setjmp( png_jmpbuf(pngStruct) ) != 0 ) {

        cerr << "error in setjmp\n";
        png_destroy_read_struct( &pngStruct, &pngInfo, NULL );
        fclose(fp);
        return false;
    }

    png_init_io( pngStruct, fp);

    unsigned int sigRead = 0;

    png_set_sig_bytes( pngStruct, sigRead );

    png_read_png (

        pngStruct,
        pngInfo,
        PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND,
        NULL
    );


    png_uint_32   png_width = width;
    png_uint_32   png_height;
    int           color ;
    int           interlace ;
    int           depth ;


    png_get_IHDR (

        pngStruct,
        pngInfo,
        &png_width,
        &png_height,
        &depth,
        &color,
        &interlace,
        NULL,
        NULL
    );

    width = png_width;
    int height = png_height;

    auto rowBytes = png_get_rowbytes( pngStruct, pngInfo );

    (*data) = (unsigned char*) malloc( rowBytes * height );

    auto rowPointers = png_get_rows( pngStruct, pngInfo );

    for ( int i = 0; i < height; i++ ) {

        memcpy(
            *data + ( rowBytes * (height - 1 - i) ),
            rowPointers[i],
            rowBytes
        );
    }

    png_destroy_read_struct( &pngStruct, &pngInfo, NULL );

    fclose( fp );


    auto valid = checkPNG( width, height, color, interlace, depth );

    if ( !valid ){
        
        free( *data );
        *data = nullptr;
    }

    return valid;
}

#endif /* USE_LIBPNG */

} // namespace SDFont
