#ifndef __SDFONT_PNG_LOADER__HPP__
#define __SDFONT_PNG_LOADER__HPP__

#include <string>

namespace SDFont {

using namespace std;

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
bool loadPngImage(

    string          filePath,
    unsigned long&  width,
    unsigned long&  height,
    unsigned char** data
);

} // namespace SDFont

#endif/*__SDFONT_PNG_LOADER__HPP__*/
