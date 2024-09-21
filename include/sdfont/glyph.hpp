#ifndef __SDFONT_GLYPH_HPP__
#define __SDFONT_GLYPH_HPP__

#include <string>

namespace SDFont {


class Glyph {

  public:

    long  mCodePoint;
    float mWidth;
    float mHeight;
    float mHorizontalBearingX;
    float mHorizontalBearingY;
    float mHorizontalAdvance;
    float mVerticalBearingX;
    float mVerticalBearingY;
    float mVerticalAdvance;
    float mTextureCoordX;
    float mTextureCoordY;
    float mTextureWidth;
    float mTextureHeight;

    std::string mGlyphName;

    /*
     *  key:   code point
     *  value: kerning
     */
    std::map< long, float > mKernings;

};


} // namespace SDFont

#endif /*__SDFONT_GLYPH_HPP__*/

