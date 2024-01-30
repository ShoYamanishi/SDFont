#include <string>
#include <iostream>

#include "sdfont/runtime_helper/runtime_helper.hpp"

namespace SDFont {

const int RuntimeHelper::NUM_POINTS_PER_GLYPH  = 4;
const int RuntimeHelper::NUM_FLOATS_PER_POINT  = 8;
const int RuntimeHelper::NUM_FLOATS_PER_GLYPH  = 4 * 8;
const int RuntimeHelper::NUM_INDICES_PER_GLYPH = 6;

RuntimeHelper::RuntimeHelper( string fileName ): mSpreadInTexture(0.0), mSpreadInFontMetrics(0.0)
{
    MetricsParser parser( mGlyphs, mSpreadInTexture, mSpreadInFontMetrics );
    parser.parseSpec( fileName );
}

RuntimeHelper::~RuntimeHelper() {;}

const Glyph* RuntimeHelper::getGlyph( const long c ) const
{
    auto git = mGlyphs.find( c );

    if ( git != mGlyphs.end() ) {

         return &( git->second );
    }
    else {

        return nullptr;
    }
}


void RuntimeHelper::getGlyphOriginsWidthAndHeight(
        
    const string&           s,
    const float             fontSize,
    const float             letterSpacing,
    const float&            leftX,
    const float&            baselineY,

    vector< const Glyph* >& glyphs,
    vector< Point2D >&      instanceOrigins,
    float&                  width,
    float&                  height,
    float&                  aboveBaselineY,
    float&                  belowBaselineY
) const {

    glyphs.clear();
    instanceOrigins.clear();
    width          = 0.0f;
    height         = 0.0f;
    aboveBaselineY = 0.0f;
    belowBaselineY = 0.0f;    

    for ( auto i = 0 ; i < s.length() ; i++ ) {

        auto ch  = s.at(i);
        auto it = mGlyphs.find(ch);

        if ( it != mGlyphs.end() ) {

            glyphs.push_back( &(it->second) );
        }
    }

    if ( glyphs.size() == 0 ) {
        return;
    }

    instanceOrigins.emplace_back(

        leftX - glyphs[0]->mHorizontalBearingX * fontSize,
        baselineY
    );

    aboveBaselineY = std::max( 0.0f, glyphs[0]->mHorizontalBearingY ); 
    belowBaselineY = std::max( 0.0f,   glyphs[0]->mHeight
                                     - glyphs[0]->mHorizontalBearingY );

    const auto numGlyphs = glyphs.size();

    for ( int i = 1; i < numGlyphs; i++ ) {

        instanceOrigins.emplace_back(

                instanceOrigins[i-1].mX
              + glyphs[i-1]->mHorizontalAdvance * fontSize * letterSpacing

            , baselineY
        );

        aboveBaselineY = std::max( aboveBaselineY, glyphs[i]->mHorizontalBearingY ); 
        belowBaselineY = std::max( belowBaselineY,   glyphs[i]->mHeight
                                                   - glyphs[i]->mHorizontalBearingY );
    }

    width =   instanceOrigins[numGlyphs-1].mX
            + glyphs[numGlyphs-1]->mWidth * fontSize
            - instanceOrigins[0].mX;

    height = aboveBaselineY + belowBaselineY;
}

void RuntimeHelper::getBoundingBoxes(

    const float                   fontSize,
    const float                   spreadRatio,
    const vector< const Glyph* >& glyphs,
    const vector< Point2D >&      instanceOrigins,
    vector< GlyphBound >&         bounds
) const {

    bounds.clear();

    const auto numGlyphs = glyphs.size();

    const float spreadVertex  = spreadInFontMetrics() * spreadRatio;
    const float spreadTexture = spreadInTexture()     * spreadRatio;

    for ( int i = 0; i < numGlyphs; i++ ) {

        const auto* glyph = glyphs[i];

        const Point2D bottomLeft(

              instanceOrigins[i].mX
            + glyph->mHorizontalBearingX * fontSize

          ,   instanceOrigins[i].mY 
            +   ( glyph->mHorizontalBearingY - glyph->mHeight )
              * fontSize
        );

        const Rect frameBound(
            bottomLeft.mX - spreadVertex * fontSize,
            bottomLeft.mY - spreadVertex * fontSize,
            ( glyph->mWidth  + 2.0f * spreadVertex ) * fontSize,
            ( glyph->mHeight + 2.0f * spreadVertex ) * fontSize
        );

        const Rect textureBound(
            glyph->mTextureCoordX - spreadTexture,
            glyph->mTextureCoordY - spreadTexture,
            glyph->mTextureWidth  + 2.0f * spreadTexture,
            glyph->mTextureHeight + 2.0f * spreadTexture
        );

        bounds.emplace_back( frameBound, textureBound );
    }
}

void RuntimeHelper::getMetrics(

    const string&           s,
    const float             fontSize,
    float&                  width,
    vector< float >&        posXs,
    float&                  firstBearingX,
    float&                  bearingY,
    float&                  belowBaselineY,
    float&                  advanceY,
    vector< const Glyph* >& glyphs
) const {
    getMetricsNormalized(
        s,
        width,
        posXs,
        firstBearingX,
        bearingY,
        belowBaselineY,
        advanceY,
        glyphs
    );
    width *= fontSize;
    for ( auto& x : posXs ) {
        x *= fontSize;
    }
    firstBearingX  *= fontSize;
    bearingY       *= fontSize;
    belowBaselineY *= fontSize;
    advanceY       *= fontSize;
}


void RuntimeHelper::getMetricsNormalized(

    const string&     s,
    float&            width,
    vector< float >&  posXs,
    float&            firstBearingX,
    float&            bearingY,
    float&            belowBaselineY,
    float&            advanceY,
    vector< const Glyph* >& glyphs

) const {

    width          = 0.0;
    firstBearingX  = 0.0;
    bearingY       = 0.0;
    belowBaselineY = 0.0;
    advanceY       = 0.0;

    bool  firstFound     = false;
    float curX           = 0.0;
    float lastAdjustment = 0.0;
    long  chPrev         = 0;
    bool  chPrevSet      = false;
    auto  len            = s.length();

    glyphs.clear();


    for ( auto i = 0 ; i < len ; i++ ) {

        auto ch  = s.at(i);
        auto git = mGlyphs.find(ch);

        if ( git != mGlyphs.end() ) {

            auto& g = git->second;

            if ( !firstFound ) {

                firstBearingX = g.mHorizontalBearingX ;
                firstFound   = true;
            }

            bearingY       = max( bearingY, g.mHorizontalBearingY );

            belowBaselineY = min( belowBaselineY,
                                  g.mHorizontalBearingY - g.mHeight );

            advanceY       = max( advanceY, g.mVerticalAdvance );


            if ( chPrevSet ) {

                const auto  git     = mGlyphs.find( chPrev );
                const auto& gPrev   = git->second;
                const auto  gitKern = gPrev.mKernings.find( ch );

                if ( gitKern != gPrev.mKernings.end() ) {
                    curX += (gitKern->second);
                }
            }

            posXs.push_back( curX + g.mHorizontalBearingX );

            curX += g.mHorizontalAdvance;

            lastAdjustment =   g.mHorizontalAdvance
                             - ( g.mHorizontalBearingX + g.mWidth );

            if ( !chPrevSet ) {

                chPrevSet = true ;
            }
            chPrev = ch;

            glyphs.push_back( &g );

        }
        else {
            posXs.push_back( 0.0 );
            glyphs.push_back( nullptr );
            chPrevSet = false;
        }
    }

    for ( auto& x : posXs ) {
        x -= firstBearingX;
    }

    width = curX - ( firstBearingX + lastAdjustment );

}


bool RuntimeHelper::getBoundingBoxes(

    const vector< Glyph* >& glyphs,
    const vector< float >&  posXs,
    const float             startX,
    const float             baselineY,
    const float             fontSize,
    const float             spreadRatio,
    const float             characterSpacing,
    vector< GlyphBound >&   bounds

) {
    if ( glyphs.size() != posXs.size() ) {

        return false;
    }

    float spreadInFont    = mSpreadInFontMetrics * fontSize * spreadRatio;
    float spreadInTexture = mSpreadInTexture * spreadRatio;

    for ( int i = 0; i < glyphs.size(); i++ ) {

        const auto* g = glyphs[i];

        if ( g == nullptr ) {
            continue;
        }

        const auto leftX   = startX + posXs[i] * characterSpacing;
        const auto bottomY = baselineY + ( g->mHorizontalBearingY - g->mHeight ) * fontSize - spreadInFont;
        const auto width   = g->mWidth  * fontSize + 2.0f * spreadInFont;
        const auto height  = g->mHeight * fontSize + 2.0f * spreadInFont;

        const auto textureX      = g->mTextureCoordX - spreadInTexture;
        const auto textureY      = g->mTextureCoordY - spreadInTexture;
        const auto textureWidth  = g->mTextureWidth  + 2.0f * spreadInTexture;
        const auto textureHeight = g->mTextureHeight + 2.0f * spreadInTexture;

        const Rect frame  ( leftX, bottomY, width, height );
        const Rect texture( textureX, textureY, textureWidth, textureHeight );

        bounds.emplace_back( frame, texture );
    }

    return true;
}

void RuntimeHelper::generateOpenGLDrawElements (

    const vector< GlyphBound >& bounds,
    const float                 Z,
    float*                      arrayBuf,
    const unsigned int          indexStart,
    unsigned int*               indices

) const {

    unsigned int  index  = indexStart;
    float*        arrayP = arrayBuf;
    unsigned int* indexP = indices;

    for(  auto& b : bounds ) {

        indexP[0] = index;
        indexP[1] = index + 1;
        indexP[2] = index + 3;
        indexP[3] = index + 2;
        indexP[4] = index + 3;
        indexP[5] = index + 1;

        arrayP[ 0]  = b.mFrame.mX;
        arrayP[ 1]  = b.mFrame.mY;
        arrayP[ 2]  = Z ;
        arrayP[ 3]  = 0.0;
        arrayP[ 4]  = 0.0;
        arrayP[ 5]  = 1.0;
        arrayP[ 6]  = b.mTexture.mX;
        arrayP[ 7]  = b.mTexture.mY;

        arrayP[ 8]  = b.mFrame.mX + b.mFrame.mW;
        arrayP[ 9]  = b.mFrame.mY;
        arrayP[10]  = Z ;
        arrayP[11]  = 0.0;
        arrayP[12]  = 0.0;
        arrayP[13]  = 1.0;
        arrayP[14]  = b.mTexture.mX + b.mTexture.mW;
        arrayP[15]  = b.mTexture.mY;

        arrayP[16]  = b.mFrame.mX + b.mFrame.mW;
        arrayP[17]  = b.mFrame.mY + b.mFrame.mH;
        arrayP[18]  = Z ;
        arrayP[19]  = 0.0;
        arrayP[20]  = 0.0;
        arrayP[21]  = 1.0;
        arrayP[22]  = b.mTexture.mX + b.mTexture.mW;
        arrayP[23]  = b.mTexture.mY + b.mTexture.mH;

        arrayP[24]  = b.mFrame.mX;
        arrayP[25]  = b.mFrame.mY + b.mFrame.mH;
        arrayP[26]  = Z ;
        arrayP[27]  = 0.0;
        arrayP[28]  = 0.0;
        arrayP[29]  = 1.0;
        arrayP[30]  = b.mTexture.mX;
        arrayP[31]  = b.mTexture.mY + b.mTexture.mH;

        index  += NUM_POINTS_PER_GLYPH;
        arrayP += NUM_FLOATS_PER_GLYPH;
        indexP += NUM_INDICES_PER_GLYPH;
    }
}


void RuntimeHelper::generateOpenGLDrawElementsForOneChar (

    const Glyph&       g,
    const float        leftX,
    const float        baselineY,
    const float        fontSize,
    const float        spreadRatio,
    const float        Z,
    float*             arrayBuf,
    const unsigned int indexStart,
    unsigned int*      indices

) const {

    indices[0] = indexStart;
    indices[1] = indexStart + 1;
    indices[2] = indexStart + 3;
    indices[3] = indexStart + 2;
    indices[4] = indexStart + 3;
    indices[5] = indexStart + 1;

    float spreadInFont    = mSpreadInFontMetrics * fontSize * spreadRatio;
    float spreadInTexture = mSpreadInTexture * spreadRatio;

    float belowBaseline = baselineY + ( g.mHorizontalBearingY - g.mHeight ) * fontSize - spreadInFont;
    float aboveBaseline = baselineY + g.mHorizontalBearingY * fontSize + spreadInFont;

    float leftPos       = leftX - spreadInFont;
    float rightPos      = leftX + g.mWidth * fontSize + spreadInFont;

    arrayBuf[ 0]  = leftPos;
    arrayBuf[ 1]  = belowBaseline;
    arrayBuf[ 2]  = Z ;
    arrayBuf[ 3]  = 0.0;
    arrayBuf[ 4]  = 0.0;
    arrayBuf[ 5]  = 1.0;
    arrayBuf[ 6]  = g.mTextureCoordX - spreadInTexture;
    arrayBuf[ 7]  = g.mTextureCoordY + g.mTextureHeight + spreadInTexture;

    arrayBuf[ 8]  = rightPos;
    arrayBuf[ 9]  = belowBaseline;
    arrayBuf[10]  = Z ;
    arrayBuf[11]  = 0.0;
    arrayBuf[12]  = 0.0;
    arrayBuf[13]  = 1.0;
    arrayBuf[14]  = g.mTextureCoordX + g.mTextureWidth  + spreadInTexture;
    arrayBuf[15]  = g.mTextureCoordY + g.mTextureHeight + spreadInTexture;

    arrayBuf[16]  = rightPos;
    arrayBuf[17]  = aboveBaseline;
    arrayBuf[18]  = Z ;
    arrayBuf[19]  = 0.0;
    arrayBuf[20]  = 0.0;
    arrayBuf[21]  = 1.0;
    arrayBuf[22]  = g.mTextureCoordX + g.mTextureWidth + spreadInTexture;
    arrayBuf[23]  = g.mTextureCoordY - spreadInTexture;

    arrayBuf[24]  = leftPos;
    arrayBuf[25]  = aboveBaseline;
    arrayBuf[26]  = Z ;
    arrayBuf[27]  = 0.0;
    arrayBuf[28]  = 0.0;
    arrayBuf[29]  = 1.0;
    arrayBuf[30]  = g.mTextureCoordX - spreadInTexture;
    arrayBuf[31]  = g.mTextureCoordY - spreadInTexture;
}


void RuntimeHelper::generateOpenGLDrawElements (

    const vector< Glyph* >& glyphs,
    const vector< float >&  posXs,
    const float             leftX,
    const float             baselineY,
    const float             fontSize,
    const float             spreadRatio,
    const float             distribution,
    const float             Z,
    float*                  arrayBuf,
    const unsigned int      indexStart,
    unsigned int*           indices

) const {

    for ( auto i = 0 ; i < glyphs.size() ; i++ ) {

        if ( glyphs[i] != nullptr ) {

            auto& g = *( glyphs[i] );

            generateOpenGLDrawElementsForOneChar (

                g ,

                leftX + posXs[i] * distribution ,

                baselineY ,

                fontSize,

                spreadRatio ,

                Z ,

                &( arrayBuf[ i * NUM_FLOATS_PER_GLYPH ] ) ,

                indexStart + i * NUM_POINTS_PER_GLYPH ,

                &( indices[ i * NUM_INDICES_PER_GLYPH ] )
            );
        }
    }
}


} // namespace SDFont
