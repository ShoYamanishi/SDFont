#ifndef __GLYPH_HPP__
#define __GLYPH_HPP__

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

    /*
     *  key:   code point
     *  value: kerning
     */
    std::map< long, float > mKernings;

};


} // namespace SDFont

#endif /*__GLYPH_HPP__*/

