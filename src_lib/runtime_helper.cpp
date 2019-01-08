#include <string>

#include "runtime_helper.hpp"

namespace SDFont {

const int RuntimeHelper::NUM_POINTS_PER_GLYPH  = 4;
const int RuntimeHelper::NUM_FLOATS_PER_POINT  = 8;
const int RuntimeHelper::NUM_FLOATS_PER_GLYPH  = 4 * 8;
const int RuntimeHelper::NUM_INDICES_PER_GLYPH = 6;

RuntimeHelper::RuntimeHelper(): mMargin(0.0) {;}

RuntimeHelper::~RuntimeHelper() {;}

Glyph* RuntimeHelper::getMetrics( long c )
{
    auto git = mGlyphs.find( c );

    if ( git != mGlyphs.end() ) {

         return &( git->second );
    }
    else {

        return nullptr;
    }
}


void RuntimeHelper::getMetrics(

    string            s,
    float&            width,
    vector< float >&  posXs,
    float&            bearingY,
    float&            belowBaseLineY,
    float&            advanceY,
    vector< Glyph* >& glyphs

) {

    width           = 0.0;
    bearingY        = 0.0;
    belowBaseLineY  = 0.0;
    advanceY        = 0.0;

    float curX      = 0.0;
    float lastWidth = 0.0;
    long  chPrev    = 0;
    bool  chPrevSet = false;
    auto  len       = s.length();

    glyphs.clear();

    for ( auto i = 0 ; i < len ; i++ ) {

        auto ch  = s.at(i);
        auto git = mGlyphs.find(ch);

        if ( git != mGlyphs.end() ) {

            auto& g = git->second;

            bearingY       = max( bearingY, g.mHorizontalBearingY );

            belowBaseLineY = min( belowBaseLineY,
                                  g.mHorizontalBearingY - g.mHeight );

            advanceY       = max( advanceY, g.mVerticalAdvance );


            if ( chPrevSet ) {

                auto& gPrev = mGlyphs[ chPrev ];
                auto gitKern = gPrev.mKernings.find( ch );

                if ( gitKern != gPrev.mKernings.end() ) {
                    curX += (gitKern->second);
                }
            }

            posXs.push_back( curX + g.mHorizontalBearingX );

            curX += g.mHorizontalAdvance;

            lastWidth = g.mHorizontalBearingX + g.mWidth;

            if ( !chPrevSet ) {

                chPrevSet = true ;
            }
            chPrev = ch;

            glyphs.push_back( &g );

        }
        else {
            posXs.push_back( 0.0 );
            glyphs.push_back( nullptr );

        }
    }

    curX += lastWidth;

    width = curX;

}


void RuntimeHelper::generateOpenGLDrawElementsForOneChar (

    Glyph&  g,
    float   leftX,
    float   baseLineY,
    float   scale,
    float   Z,

    float*  arrayBuf,
    GLuint  indexStart,
    GLuint* indices

) {

    indices[0] = indexStart;
    indices[1] = indexStart + 1;
    indices[2] = indexStart + 3;
    indices[3] = indexStart + 2;
    indices[4] = indexStart + 3;
    indices[5] = indexStart + 1;

    float scaledMargin  = mMargin * scale;

    float belowBaseLine =   baseLineY
                          + ( g.mHorizontalBearingY - g.mHeight )
                            * scale
                          - scaledMargin ;

    float aboveBaseLine =   baseLineY
                          + g.mHorizontalBearingY * scale
                          + scaledMargin ;

    float leftPos       = leftX - scaledMargin;

    float rightPos      = leftX + g.mWidth * scale + scaledMargin;

    arrayBuf[ 0]  = leftPos;
    arrayBuf[ 1]  = belowBaseLine;
    arrayBuf[ 2]  = Z ;
    arrayBuf[ 3]  = 0.0;
    arrayBuf[ 4]  = 0.0;
    arrayBuf[ 5]  = 1.0;
    arrayBuf[ 6]  = g.mTextureCoordX - mMargin;
    arrayBuf[ 7]  = g.mTextureCoordY + g.mHeight + mMargin;

    arrayBuf[ 8]  = rightPos;
    arrayBuf[ 9]  = belowBaseLine;
    arrayBuf[10]  = Z ;
    arrayBuf[11]  = 0.0;
    arrayBuf[12]  = 0.0;
    arrayBuf[13]  = 1.0;
    arrayBuf[14]  = g.mTextureCoordX + g.mWidth  + mMargin;
    arrayBuf[15]  = g.mTextureCoordY + g.mHeight + mMargin;

    arrayBuf[16]  = rightPos;
    arrayBuf[17]  = aboveBaseLine;
    arrayBuf[18]  = Z ;
    arrayBuf[19]  = 0.0;
    arrayBuf[20]  = 0.0;
    arrayBuf[21]  = 1.0;
    arrayBuf[22]  = g.mTextureCoordX + g.mWidth + mMargin;
    arrayBuf[23]  = g.mTextureCoordY - mMargin;

    arrayBuf[24]  = leftPos;
    arrayBuf[25]  = aboveBaseLine;
    arrayBuf[26]  = Z ;
    arrayBuf[27]  = 0.0;
    arrayBuf[28]  = 0.0;
    arrayBuf[29]  = 1.0;
    arrayBuf[30]  = g.mTextureCoordX - mMargin;
    arrayBuf[31]  = g.mTextureCoordY - mMargin;

}


void RuntimeHelper::generateOpenGLDrawElements (

    vector< Glyph* >& glyphs,
    vector< float >&  posXs,
    float             leftX,
    float             baselineY,
    float             scale,
    float             distribution,
    float             Z,
    float*            arrayBuf,
    GLuint            indexStart,
    GLuint*           indices

) {

    for ( auto i = 0 ; i < glyphs.size() ; i++ ) {

        if ( glyphs[i] != nullptr ) {

            auto& g = *( glyphs[i] );

            generateOpenGLDrawElementsForOneChar (

                g ,

                leftX + posXs[i] * scale * distribution ,

                baselineY ,

                scale ,

                Z ,

                &( arrayBuf[ i * NUM_FLOATS_PER_GLYPH ] ) ,

                indexStart + i * NUM_POINTS_PER_GLYPH ,

                &( indices[ i * NUM_INDICES_PER_GLYPH ] )
            );
        }
    }
}


} // namespace SDFont
