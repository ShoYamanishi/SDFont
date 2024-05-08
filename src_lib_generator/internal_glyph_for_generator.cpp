#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <limits>
#include <stdexcept>

#include "sdfont/generator/generator_config.hpp"
#include "sdfont/generator/internal_glyph_for_generator.hpp"

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
    mSignedDistBaseY    ( 0 ),
    mHasExternalBitmap  ( false ),
    mExternalBitmapWidth( 0 ),
    mExternalBitmapHeight( 0 ),
    mExternalBitmap     ( nullptr )
{
    mSignedDistWidth  = ceil( (float)mWidth  + 2.0f * mConf.signedDistExtent() );
    mSignedDistHeight = ceil( (float)mHeight + 2.0f * mConf.signedDistExtent() );
}


InternalGlyphForGen::InternalGlyphForGen (
    GeneratorConfig&  conf,
    const long        codePoint,
    const long        width,
    const long        height,
    unsigned char*    external_bitmap,
    const long        external_bitmap_width,
    const long        external_bitmap_height
):
    mConf               ( conf ),
    mCodePoint          ( codePoint ),

    mTextureCoordX      ( 0.0 ),
    mTextureCoordY      ( 0.0 ),
    mTextureWidth       ( 0.0 ),
    mTextureHeight      ( 0.0 ),

    mWidth              ( width ),
    mHeight             ( height ),
    mHorizontalBearingX ( 0.0f ),
    mHorizontalBearingY ( 0.0f ),
    mHorizontalAdvance  ( width ),
    mVerticalBearingX   ( 0.0f ),
    mVerticalBearingY   ( 0.0f ),
    mVerticalAdvance    ( height ),
    mSignedDist         ( nullptr ),
    mSignedDistWidth    ( 0 ),
    mSignedDistHeight   ( 0 ),
    mSignedDistBaseX    ( 0 ),
    mSignedDistBaseY    ( 0 ),
    mHasExternalBitmap  ( true ),
    mExternalBitmapWidth( external_bitmap_width ),
    mExternalBitmapHeight(external_bitmap_height ),
    mExternalBitmap     ( external_bitmap )
{
    mSignedDistWidth  = ceil( (float)mWidth  + 2.0f * mConf.signedDistExtent() );
    mSignedDistHeight = ceil( (float)mHeight + 2.0f * mConf.signedDistExtent() );
}


InternalGlyphForGen::~InternalGlyphForGen () {

    if ( mHasExternalBitmap ) {

        free( mExternalBitmap );
    }

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

        for ( auto j = 1 ; j < i; j++ ) {

            if ( testSymmetricPoints( bm, curP, xPix, yPix, i, j ) ) {

                float fj   = (float) j;
                minSqDist  = min( minSqDist, fi * fi + fj * fj );
                nextStartI = i + downSampleRate;
            }
        }

        if ( testDiagonalPoints( bm, curP, xPix, yPix, i ) ) {

            minSqDist  = min( minSqDist, 2 * fi * fi );
            nextStartI = i + downSampleRate;
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


float InternalGlyphForGen::getSignedDistance(
    const long i,
    const long j,
    const long width,
    const long height
) {
    const auto x_spread = static_cast<long>( mConf.ratioSpreadToGlyph() * static_cast<float>( mExternalBitmapWidth  ) );
    const auto y_spread = static_cast<long>( mConf.ratioSpreadToGlyph() * static_cast<float>( mExternalBitmapHeight ) );

    const auto x = static_cast<long>(   static_cast<float>( i )
                                      / static_cast<float>( width )
                                      * static_cast<float>( mExternalBitmapWidth + 2 * x_spread )
                                    );

    const auto y = static_cast<long>(   static_cast<float>( j )
                                      / static_cast<float>( height )
                                      * static_cast<float>( mExternalBitmapHeight + 2 * y_spread )
                                    );

    const auto set_xy = isPixelSetInExternalBitmap( x - x_spread, y - y_spread );

    long min_sq_dist{ x_spread * x_spread + y_spread * y_spread };

    for ( long ty = -1 * y_spread; ty <= y_spread; ty++ ) {

        for ( long tx = -1 * x_spread; tx <= x_spread; tx++ ) {

            if ( isPixelSetInExternalBitmap( tx + x - x_spread, ty + y -y_spread ) != set_xy ) {

                const auto sq_dist = tx * tx + ty * ty;

                if ( sq_dist < min_sq_dist ) {

                    min_sq_dist = sq_dist;
                }
            }
        }
    }

    const auto normalized_min_dist = sqrt( static_cast< float >( min_sq_dist ) )
                               / static_cast< float >( ( x_spread + y_spread ) / 2 );
    if ( set_xy ) {
        return 0.5f + ( normalized_min_dist / 2.0f );
    }
    else {
        return 0.5f - ( normalized_min_dist / 2.0f );
    }
}


bool InternalGlyphForGen::isPixelSetInExternalBitmap( const long x, const long y )
{
    if (   x < 0
        || x >= mExternalBitmapWidth
        || y < 0
        || y >= mExternalBitmapHeight
    ) {
        return false;
    }

    return mExternalBitmap[ ( mExternalBitmapHeight - 1 - y ) * mExternalBitmapWidth + x ] > 0;
}

void InternalGlyphForGen::setSignedDist( FT_Bitmap& bm ) {

    if ( mConf.isDeadReckoningSet() ) {

        setSignedDistByDeadReckoning( bm );
//        doGaussianBlur5x5( bm );
    }
    else {
        setSignedDistBySeparateVicinitySearch( bm );
    }
}


void InternalGlyphForGen::setSignedDist()
{
    if ( !hasExternalBitmap() ) {

        throw std::runtime_error( "no external bitmap specified." );
    }

    setSignedDistBySeparateVicinitySearch();
}


void InternalGlyphForGen::setSignedDistByDeadReckoning( FT_Bitmap& bm ) {

    const auto scale = mConf.glyphScalingFromSamplingToPackedSignedDist();

    const long spreadInBitmapPixels = static_cast<long> (
          mConf.ratioSpreadToGlyph()
        * (float)mConf.glyphBitmapSizeForSampling()
    );

    mSignedDistWidth  = ceil(mWidth  * scale + 2 * mConf.signedDistExtent());
    mSignedDistHeight = ceil(mHeight * scale + 2 * mConf.signedDistExtent());

    size_t arraySize = mSignedDistWidth * mSignedDistHeight;

    mSignedDist = new float[ arraySize ];

    auto* nearestCells = new NearestCell[ arraySize ];

    doDeadReckoning_initialize( nearestCells );

    doDeadReckoning_processCellsOnEdges( bm, nearestCells );

    doDeadReckoning_scanForward( nearestCells );

    doDeadReckoning_scanBackward( nearestCells );

    doDeadReckoning_normalizeDistances( bm );

    delete[] nearestCells;
}


void InternalGlyphForGen::doDeadReckoning_initialize( NearestCell* nearestCells )
{
    const auto longerEdge = static_cast<float>( std::max(mSignedDistWidth, mSignedDistHeight) );
    const auto longEnoughDist = longerEdge * longerEdge * 4.0f;

    for ( long j = 0; j < mSignedDistHeight; j++ ) {

        const auto rawStart = j * mSignedDistWidth;

        for ( long i = 0; i < mSignedDistWidth; i++ ) {

            const auto indexSD = rawStart + i;

            mSignedDist[ indexSD ] = longEnoughDist;
            nearestCells[ indexSD ].set(mSignedDistWidth * 2, mSignedDistHeight * 2);
        }
    }
}


void InternalGlyphForGen::doDeadReckoning_processCellsOnEdges( FT_Bitmap& bm, NearestCell* nearestCells ) {

    const auto offset = mConf.signedDistExtent();
    const auto scale = mConf.glyphScalingFromSamplingToPackedSignedDist();

    for ( long j = 0; j < mSignedDistHeight; j++ ) {

        const auto rawStart = j * mSignedDistWidth;

        const auto fj = static_cast<float>( j - offset ) + 0.5f;

        for ( long i = 0; i < mSignedDistWidth; i++ ) {

            const auto indexSD = rawStart + i;

            const auto fi = static_cast<float>(i - offset) + 0.5f;

            auto xCenter = static_cast<long>( fi / scale );
            auto xWest   = static_cast<long>( (fi - 1.0f) / scale );
            auto xEast   = static_cast<long>( (fi + 1.0f) / scale );

            auto yCenter = static_cast<long>( fj / scale );
            auto yNorth  = static_cast<long>( (fj - 1.0f) / scale );
            auto ySouth  = static_cast<long>( (fj + 1.0f) / scale );

            const auto center = isPixelSet( bm, xCenter, yCenter );
            const auto north  = isPixelSet( bm, xCenter, yNorth  );
            const auto south  = isPixelSet( bm, xCenter, ySouth  );
            const auto east   = isPixelSet( bm, xEast,   yCenter );
            const auto west   = isPixelSet( bm, xWest,   yCenter );

            const auto allOne  =    north && south && east && west && center;
            const auto allZero = !( north || south || east || west || center );

            if ( !(allOne || allZero) ) {

                mSignedDist[ indexSD ] = 0.0f;
                nearestCells[ indexSD ].set( i, j );
            }
        }
    }
}

void InternalGlyphForGen::doDeadReckoning_scanForward( NearestCell* nearestCells ) {

    const float distDiag  = sqrtf( 2.0f );
    const float distOrtho = 1.0f;

    for ( long j = 1; j < mSignedDistHeight; j++ ) {

        const auto rawStartCenter =  j    * mSignedDistWidth;
        const auto rawStartNorth  = (j-1) * mSignedDistWidth;

        for ( long i = 1; i < mSignedDistWidth - 1; i++ ) {

            const auto indexCenter    = rawStartCenter + i;

            auto& distCenter = mSignedDist[ indexCenter    ];

            if ( distCenter == 0.0f ) {
                continue;
            }

            auto& cellCenter = nearestCells[ indexCenter ];

            const auto indexWest      = rawStartCenter + i - 1;
            const auto indexNorth     = rawStartNorth  + i;
            const auto indexNorthWest = rawStartNorth  + i - 1;
            const auto indexNorthEast = rawStartNorth  + i + 1;

            const auto distWest      = mSignedDist[ indexWest      ];
            const auto distNorth     = mSignedDist[ indexNorth     ];
            const auto distNorthWest = mSignedDist[ indexNorthWest ];
            const auto distNorthEast = mSignedDist[ indexNorthEast ];

            if ( distNorthWest + distDiag < distCenter ) {

                cellCenter = nearestCells[ indexNorthWest ];

                distCenter = cellCenter.getDistFrom( i, j );
            }

            if ( distNorth + distOrtho < distCenter ) {

                cellCenter = nearestCells[ indexNorth ];

                distCenter = cellCenter.getDistFrom( i, j );
            }

            if ( distNorthEast + distDiag < distCenter ) {

                cellCenter = nearestCells[ indexNorthEast ];

                distCenter = cellCenter.getDistFrom( i, j );
            }

            if ( distWest + distOrtho < distCenter ) {

                cellCenter = nearestCells[ indexWest ];

                distCenter = cellCenter.getDistFrom( i, j );
            }
        }
    }
}

void InternalGlyphForGen::doDeadReckoning_scanBackward( NearestCell* nearestCells ) {

    const float distDiag  = sqrtf( 2.0f );
    const float distOrtho = 1.0f;

    for ( long j = mSignedDistHeight - 2; j > 0 ; j-- ) {

        const auto rawStartCenter =  j    * mSignedDistWidth;
        const auto rawStartSouth  = (j+1) * mSignedDistWidth;

        for ( long i = mSignedDistWidth - 2; i > 0; i-- ) {

            const auto indexCenter    = rawStartCenter + i;

            auto& distCenter = mSignedDist[ indexCenter    ];

            if ( distCenter == 0.0f ) {
                continue;
            }

            auto& cellCenter = nearestCells[ indexCenter ];

            const auto indexEast      = rawStartCenter + i + 1;
            const auto indexSouth     = rawStartSouth  + i;
            const auto indexSouthWest = rawStartSouth  + i - 1;
            const auto indexSouthEast = rawStartSouth  + i + 1;

            const auto distEast      = mSignedDist[ indexEast      ];
            const auto distSouth     = mSignedDist[ indexSouth     ];
            const auto distSouthWest = mSignedDist[ indexSouthWest ];
            const auto distSouthEast = mSignedDist[ indexSouthEast ];

            if ( distSouthEast + distDiag < distCenter ) {

                cellCenter = nearestCells[ indexSouthEast ];

                distCenter = cellCenter.getDistFrom( i, j );
            }

            if ( distSouth + distOrtho < distCenter ) {

                cellCenter = nearestCells[ indexSouth ];

                distCenter = cellCenter.getDistFrom( i, j );
            }

            if ( distSouthWest + distDiag < distCenter ) {

                cellCenter = nearestCells[ indexSouthWest ];

                distCenter = cellCenter.getDistFrom( i, j );
            }

            if ( distEast + distOrtho < distCenter ) {

                cellCenter = nearestCells[ indexEast ];

                distCenter = cellCenter.getDistFrom( i, j );
            }
        }
    }
}

void InternalGlyphForGen::doDeadReckoning_normalizeDistances( FT_Bitmap& bm ) {

    const auto offset = mConf.signedDistExtent();
    const auto scale = mConf.glyphScalingFromSamplingToPackedSignedDist();

    for ( long j = 0; j < mSignedDistHeight; j++ ) {

        const auto rawStart = j * mSignedDistWidth;

        const auto fj = static_cast<float>(j - offset) + 0.5f;

        for ( long i = 0; i < mSignedDistWidth; i++ ) {

            const auto indexSD = rawStart + i;

            const auto fi = static_cast<float>(i - offset) + 0.5f;

            auto xCenter = static_cast<long>( fi / scale );
            auto yCenter = static_cast<long>( fj / scale );

            if ( isPixelSet( bm, xCenter, yCenter ) ) {
                mSignedDist[ indexSD ] = std::min( 1.0f, 0.5f + mSignedDist[ indexSD ] / static_cast<float>(2.0*offset) );
            }
            else {
                mSignedDist[ indexSD ] = std::max( 0.0f, 0.5f - mSignedDist[ indexSD ] / static_cast<float>(2.0*offset) );
            }
        }
    }
}


void InternalGlyphForGen::doGaussianBlur5x5( FT_Bitmap& bm ) {

    const auto offset = mConf.signedDistExtent();
    const auto scale = mConf.glyphScalingFromSamplingToPackedSignedDist();

    const long spreadInBitmapPixels = static_cast<long> (
          mConf.ratioSpreadToGlyph()
        * (float)mConf.glyphBitmapSizeForSampling()
    );

    mSignedDistWidth  = ceil(mWidth  * scale + 2 * mConf.signedDistExtent());
    mSignedDistHeight = ceil(mHeight * scale + 2 * mConf.signedDistExtent());

    size_t arraySize = mSignedDistWidth * mSignedDistHeight;

    mSignedDist = new float[ arraySize ];

    for ( long j = 0; j < mSignedDistHeight; j++ ) {

        const auto rawStart = j * mSignedDistWidth;

        for ( long i = 0; i < mSignedDistWidth; i++ ) {

            const auto indexSD = rawStart + i;

            mSignedDist[ indexSD ] = std::max( 0.0f, std::min( 1.0f, doGaussianBlur5x5( bm, i - offset, j - offset) ) );
        }
    }
}


float InternalGlyphForGen::doGaussianBlur5x5( FT_Bitmap& bm, long i, long j ) {

    const auto offset = mConf.signedDistExtent();
    const auto scale = mConf.glyphScalingFromSamplingToPackedSignedDist();

    const auto fj = static_cast<float>( j ) + 0.5f;
    const auto fi = static_cast<float>( i ) + 0.5f;

    auto xCenter = static_cast<long>( fi / scale );
    auto yCenter = static_cast<long>( fj / scale );

    if ( isPixelSet( bm, xCenter, yCenter ) ) {

        return 1.0f;
    }
    else {
        return 0.0f;
    }

    static const float coeffs[] = {
        1.0f,  4.0f,   6.0f,  4.0f, 1.0f,
        4.0f, 16.0f,  24.0f, 16.0f, 4.0f,
        6.0f, 24.0f,  36.0f, 24.0f, 6.0f,
        4.0f, 16.0f,  24.0f, 16.0f, 4.0f,
        1.0f,  4.0f,   6.0f,  4.0f, 1.0f,
    };

    float val = 0.0f;

    for ( long j2 = -2; j2 < 3; j2++ ) {

        const auto fj = static_cast<float>(j + j2) + 0.5f;

        for ( long i2 = -2; i2 < 3; i2++ ) {

            const auto coeff = coeffs[j2*5 + i2];

            const auto fi = static_cast<float>(i + i2) + 0.5f;

            auto xCenter = static_cast<long>( fi / scale );
            auto yCenter = static_cast<long>( fj / scale );

            if ( isPixelSet( bm, xCenter, yCenter ) ) {
                val += coeff;
            }
        }
    }
    return val / 128.0f;
}



void InternalGlyphForGen::setSignedDistBySeparateVicinitySearch( FT_Bitmap& bm ) {

    const auto scale = mConf.glyphScalingFromSamplingToPackedSignedDist();

    const long spreadInBitmapPixels = (long)( mConf.ratioSpreadToGlyph() * (float)mConf.glyphBitmapSizeForSampling() );

    mSignedDistWidth  = ceil(mWidth  * scale + 2 * mConf.signedDistExtent());
    mSignedDistHeight = ceil(mHeight * scale + 2 * mConf.signedDistExtent());

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


void InternalGlyphForGen::setSignedDistBySeparateVicinitySearch()
{
    const auto scale = mConf.glyphScalingFromSamplingToPackedSignedDist();

    mSignedDistWidth  = ceil(mWidth  * scale + 2 * mConf.signedDistExtent());
    mSignedDistHeight = ceil(mHeight * scale + 2 * mConf.signedDistExtent());

    size_t arraySize = mSignedDistWidth * mSignedDistHeight;

    mSignedDist = new float[ arraySize ];

    const long offset = mConf.signedDistExtent();

    for ( long i = 0 ; i < mSignedDistHeight; i++ ) {

        for ( long j = 0 ; j < mSignedDistWidth; j++ ) {

            auto val = getSignedDistance(
                j,
                i,
                mSignedDistWidth,
                mSignedDistHeight
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

