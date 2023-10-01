#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <limits>

#include "sdfont_generator_config.hpp"
#include "sdfont_internal_glyph_for_generator.hpp"

using namespace std;

namespace SDFont {


const long InternalGlyphForGen::FREE_TYPE_FIXED_POINT_SCALING = 64 ;


InternalGlyphForGen::InternalGlyphForGen (
    GeneratorConfig&  conf,
    long              codePoint,
    FT_Glyph_Metrics& m
):
    mConf               ( conf ),
    mCodePoint          ( codePoint ),

    mTextureCoordX      ( 0.0 ),
    mTextureCoordY      ( 0.0 ),
    mTextureWidth       ( 0.0 ),
    mTextureHeight      ( 0.0 ),

    mWidth              ( m.width        / FREE_TYPE_FIXED_POINT_SCALING ),
    mHeight             ( m.height       / FREE_TYPE_FIXED_POINT_SCALING ),
    mHorizontalBearingX ( m.horiBearingX / FREE_TYPE_FIXED_POINT_SCALING ),
    mHorizontalBearingY ( m.horiBearingY / FREE_TYPE_FIXED_POINT_SCALING ),
    mHorizontalAdvance  ( m.horiAdvance  / FREE_TYPE_FIXED_POINT_SCALING ),
    mVerticalBearingX   ( m.vertBearingX / FREE_TYPE_FIXED_POINT_SCALING ),
    mVerticalBearingY   ( m.vertBearingY / FREE_TYPE_FIXED_POINT_SCALING ),
    mVerticalAdvance    ( m.vertAdvance  / FREE_TYPE_FIXED_POINT_SCALING ),
    mSignedDist         ( nullptr ),
    mSignedDistWidth    ( 0 ),
    mSignedDistHeight   ( 0 ),
    mSignedDistBaseX    ( 0 ),
    mSignedDistBaseY    ( 0 )
{
    mSignedDistWidth  = ceil( (float)mWidth  * ( 1.0f + 2.0f * mConf.ratioSpreadToGlyph() ) );
    mSignedDistHeight = ceil( (float)mHeight * ( 1.0f + 2.0f * mConf.ratioSpreadToGlyph() ) );
}


InternalGlyphForGen::~InternalGlyphForGen () {

    if ( mSignedDist != nullptr ) {

        free ( mSignedDist );
    }
}


void InternalGlyphForGen::setBaseXY( long x, long y ) {

    mSignedDistBaseX = x;
    mSignedDistBaseY = y;

    auto fDim        = (float) mConf.outputTextureSize() ;

    mTextureCoordX   = (float) ( x + mConf.signedDistExtent() ) / fDim ;
    mTextureCoordY   = (float) ( y + mConf.signedDistExtent() ) / fDim ;

    const auto scale = mConf.glyphScalingFromSamplingToPackedSignedDist();

    mTextureWidth    = (float) mWidth  * scale / fDim ;
    mTextureHeight   = (float) mHeight * scale / fDim ;
}


void InternalGlyphForGen::addKerning( long followingCodePoint, FT_Pos v ) {

    mKernings[ followingCodePoint ] = v / FREE_TYPE_FIXED_POINT_SCALING ;
}


bool InternalGlyphForGen::testOrthogonalPoints(
    FT_Bitmap&  bm,
    bool        testBase,
    long        xBase,
    long        yBase,
    long        offset
) {

    bool test01 = isPixelSet( bm, xBase,          yBase + offset );
    bool test02 = isPixelSet( bm, xBase,          yBase - offset );
    bool test03 = isPixelSet( bm, xBase + offset, yBase          );
    bool test04 = isPixelSet( bm, xBase - offset, yBase          );

    if ( test01 && test02 && test03 && test04 ) {

        if ( !testBase ) {

            // All the four points are set and the center point is not set.
            return true;
        }
        else {
            return false;
	    }
    }
    else if ( !( test01 || test02 || test03 || test04 ) ) {

        if ( testBase ) {

	      // All the four points are not set and the center point is set.
            return true;
	    }
        else {
	      return false;

        }
    }
    else {
        // The four points are mixed.
        return true;
    }
}


bool InternalGlyphForGen::testSymmetricPoints(
    FT_Bitmap&  bm,
    bool        testBase,
    long        xBase,
    long        yBase,
    long        offset1,
    long        offset2
) {

    bool test01 = isPixelSet( bm, xBase + offset1, yBase + offset2 );
    bool test02 = isPixelSet( bm, xBase + offset1, yBase - offset2 );
    bool test03 = isPixelSet( bm, xBase - offset1, yBase + offset2 );
    bool test04 = isPixelSet( bm, xBase - offset1, yBase - offset2 );

    bool test05 = isPixelSet( bm, xBase + offset2, yBase + offset1 );
    bool test06 = isPixelSet( bm, xBase + offset2, yBase - offset1 );
    bool test07 = isPixelSet( bm, xBase - offset2, yBase + offset1 );
    bool test08 = isPixelSet( bm, xBase - offset2, yBase - offset1 );

    if (    test01 && test02 && test03 && test04
         && test05 && test06 && test07 && test08 ) {

        if ( !testBase ) {

            // All the four points are set and the center point is not set.
            return true;
        }
        else {
            return false;
        }
    }
    else if ( !(    test01 || test02 || test03 || test04
                 || test05 || test06 || test07 || test08 ) ) {

        if ( testBase ) {

            // All the four points are not set and the center point is set.
            return true;
        }
        else {
            return false;

        }
    }
    else {
        // The four points are mixed.
        return true;
    }
}


bool InternalGlyphForGen::testDiagonalPoints(
    FT_Bitmap&  bm,
    bool        testBase,
    long        xBase,
    long        yBase,
    long        offset
) {

    bool test01 = isPixelSet( bm, xBase + offset, yBase + offset );
    bool test02 = isPixelSet( bm, xBase + offset, yBase - offset );
    bool test03 = isPixelSet( bm, xBase - offset, yBase + offset );
    bool test04 = isPixelSet( bm, xBase - offset, yBase - offset );


    if ( test01 && test02 && test03 && test04 ) {

        if ( !testBase ) {

            // All the four points are set and the center point is not set.
            return true;
        }
        else {
            return false;
        }
    }
    else if ( !( test01 || test02 || test03 || test04 ) ) {

        if ( testBase ) {

            // All the four points are not set and the center point is set.
            return true;
        }
        else {
            return false;

        }
    }
    else {
        // The four points are mixed.
        return true;
    }
}


float InternalGlyphForGen::getSignedDistance(

    FT_Bitmap& bm,
    float      scaling,
    long       spreadInGlyphPixelsForSampling,
    long       xSD,
    long       ySD

) {
    auto pixelOffset = 0.5f / scaling;
    auto xPix        = (long) ( (float)xSD / scaling + pixelOffset );
    auto yPix        = (long) ( (float)ySD / scaling + pixelOffset );

    const long downSampleRate = 1;

    bool  curP       = isPixelSet( bm, xPix, yPix );
    float fSpread    = (float) spreadInGlyphPixelsForSampling ;
    float minSqDist  = fSpread * fSpread ;
    long  nextStartI = spreadInGlyphPixelsForSampling + downSampleRate;

    for (auto i = 1 ; i <= spreadInGlyphPixelsForSampling; i += downSampleRate ) {

        float fi = (float)i;

        if ( testOrthogonalPoints( bm, curP, xPix, yPix, i ) ) {

            minSqDist  = min( minSqDist, fi * fi );
            nextStartI = i + downSampleRate;
            break;
        }

        bool breaking = false;

        for ( auto j = 1 ; j < i; j++ ) {

            if ( testSymmetricPoints( bm, curP, xPix, yPix, i, j ) ) {

                float fj   = (float) j;
                minSqDist  = min( minSqDist, fi * fi + fj * fj );
                nextStartI = i + downSampleRate;
                breaking   = true;
                break;
            }
        }

        if (breaking) {

            break;
        }

        if ( testDiagonalPoints( bm, curP, xPix, yPix, i ) ) {

            minSqDist  = min( minSqDist, 2 * fi * fi );
            nextStartI = i + downSampleRate;
            break;
        }
    }

    long maxI = min( (long)(sqrt(minSqDist)) + 1, spreadInGlyphPixelsForSampling );

    for (auto i = nextStartI ; i <= maxI; i += downSampleRate ) {

        float fi = (float)i;

        if ( testOrthogonalPoints( bm, curP, xPix, yPix, i ) ) {

            minSqDist  = min( minSqDist, fi * fi );
            break;
        }

        bool breaking = false;

        for ( auto j = 1 ; j < nextStartI; j++ ) {

            if ( testSymmetricPoints( bm, curP, xPix, yPix, i, j ) ) {

                float fj   = (float) j;
                minSqDist  = min( minSqDist, fi * fi + fj * fj );
                breaking   = true;
                break;
            }
        }

        if (breaking) {

            break;
        }
    }

    auto normalizedMinDist = ( (float)sqrt(minSqDist) - 1.0 ) / fSpread ;

    if (curP) {

        return 0.5 + ( normalizedMinDist / 2.0 );
    }
    else {

        return 0.5 - ( normalizedMinDist / 2.0 );
    }

}


void InternalGlyphForGen::setSignedDist( FT_Bitmap& bm ) {

    const auto scale = mConf.glyphScalingFromSamplingToPackedSignedDist();

    const long spreadInBitmapPixels = (long)( mConf.ratioSpreadToGlyph() * (float)mConf.glyphBitmapSizeForSampling() );

    mSignedDistWidth  = mWidth  * scale + 2 * mConf.signedDistExtent();
    mSignedDistHeight = mHeight * scale + 2 * mConf.signedDistExtent();

    size_t arraySize = mSignedDistWidth * mSignedDistHeight;

    mSignedDist = new float[ arraySize ];

    const long offset = mConf.signedDistExtent();

    for ( long i = 0 ; i < mSignedDistHeight; i++ ) {

        for ( long j = 0 ; j < mSignedDistWidth; j++ ) {

            auto val = getSignedDistance( bm,
                                          scale,
                                          spreadInBitmapPixels,
                                          j - offset,
                                          i - offset
                                        );
            mSignedDist[i * mSignedDistWidth + j] = val;
        }
    }
}


void InternalGlyphForGen::releaseBitmap() {

    if ( mSignedDist != nullptr ) {

        delete[] mSignedDist;

        mSignedDist       = nullptr;
        mSignedDistWidth  = 0;
        mSignedDistHeight = 0;
    }
}


void InternalGlyphForGen::visualize( ostream& os ) const {

    if ( mSignedDist != nullptr ) {

        for ( long i = 0 ; i < mSignedDistHeight; i++ ) {

            for ( long j = 0 ; j < mSignedDistWidth; j++ ) {

                auto val = mSignedDist[ i * mSignedDistWidth + j ];

                if ( val >= 0.5 ) {
                    cerr << "*";
                }
                else {
                    cerr << ".";
                }
            }
            cerr << "\n";
        }
    }
}


void InternalGlyphForGen::emitMetrics( ostream& os ) const {

    float factor =  (float) ( mConf.glyphBitmapSizeForSampling() );

    os << mCodePoint ;

    os << "\t";
    os << ( (float)mWidth              / factor );
    os << "\t";
    os << ( (float)mHeight             / factor );
    os << "\t";
    os << ( (float)mHorizontalBearingX / factor );
    os << "\t";
    os << ( (float)mHorizontalBearingY / factor );
    os << "\t";
    os << ( (float)mHorizontalAdvance  / factor );
    os << "\t";
    os << ( (float)mVerticalBearingX   / factor );
    os << "\t";
    os << ( (float)mVerticalBearingY   / factor );
    os << "\t";
    os << ( (float)mVerticalAdvance    / factor );

    os << "\t";
    os << mTextureCoordX ;
    os << "\t";
    os << mTextureCoordY ;
    os << "\t";
    os << mTextureWidth ;
    os << "\t";
    os << mTextureHeight ;

}


void InternalGlyphForGen::emitKernings( ostream& os ) const {

    if ( mKernings.size() > 0 ) {

        float factor =  (float) ( mConf.glyphBitmapSizeForSampling() );

        os << mCodePoint ;

        for ( auto it = mKernings.begin(); it != mKernings.end(); it++ ) {

            os << "\t" << it->first ;
            os << "\t" << (float)( (it->second ) / factor ) ;
        }

        os << "\n";
    }
}


Glyph InternalGlyphForGen::generateSDGlyph() const {

    float factor =  (float) ( mConf.glyphBitmapSizeForSampling() );

    Glyph g;

    g.mCodePoint          = mCodePoint;
    g.mWidth              = (float)mWidth              / factor ;
    g.mHeight             = (float)mHeight             / factor ;
    g.mHorizontalBearingX = (float)mHorizontalBearingX / factor ;
    g.mHorizontalBearingY = (float)mHorizontalBearingY / factor ;
    g.mHorizontalAdvance  = (float)mHorizontalAdvance  / factor ;
    g.mVerticalBearingX   = (float)mVerticalBearingX   / factor ;
    g.mVerticalBearingY   = (float)mVerticalBearingY   / factor ;
    g.mVerticalAdvance    = (float)mVerticalAdvance    / factor ;
    g.mTextureCoordX      = mTextureCoordX ;
    g.mTextureCoordY      = mTextureCoordY ;
    g.mTextureWidth       = mTextureWidth  ;
    g.mTextureHeight      = mTextureHeight ;

    for ( auto it = mKernings.begin(); it != mKernings.end(); it++ ) {

        g.mKernings[ it->first ] = (float)( (it->second ) / factor ) ;

    }
    return g;

}


} // namespace SDFont

