#ifndef __SDFONT_INTERNAL_GLYPH_FOR_GENERATOR_HPP__
#define __SDFONT_INTERNAL_GLYPH_FOR_GENERATOR_HPP__

#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "sdfont/generator/generator_config.hpp"
#include "sdfont/glyph.hpp"

using namespace std;

namespace SDFont {

/** @file sdfont_internal_glyph_for_gen.hpp
 *
 *  @brief per glyph object to generate signed distant font from the 
 *         corresponding FreeType's bitmap font.
 *
 *  @dependency FreeType : https://www.freetype.org
 *
 */
class InternalGlyphForGen {

  public:

    InternalGlyphForGen(
        GeneratorConfig&  conf ,
        long              codePoint ,
        FT_Glyph_Metrics& m            );

    virtual ~InternalGlyphForGen ();

    inline long codePoint() const;

    inline long signedDistWidth()  const;

    inline long signedDistHeight() const;

    inline long baseX() const;

    inline long baseY() const;

    inline float signedDist( long x, long y ) const;


    void addKerning( long followingCodePoint, FT_Pos v );

    /** @brief generates an internal matrix that contains the 
     *         signed distance.
     *         It allocates the memory in mSignedDist and sets the dimension
     *         mSignedDistWidth and mSignedDistHeight.
     *
     *  @param bm     (in): FreeType bitmap info.
     */
    void setSignedDist( FT_Bitmap& bm );




    /** @brief set the coordinates of this glyph in the PNG coordinate system
     *         and the normalized texture coordinate system.
     *         It also sets the texture width and height in the normalized
     *         texture coordinate system.
     *
     *  @param  x (in): x position in the global PNG coordinate system.
     *  @param  y (in): y position in the global PNG coordinate system.
     */
    void setBaseXY( long x, long y );


    void releaseBitmap();

    void visualize   ( ostream& os ) const ;
    void emitMetrics ( ostream& os ) const ;
    void emitKernings( ostream& os ) const ;

    Glyph generateSDGlyph() const;

  private:

    /** @brief calculates the signed distance value from the current point
     *
     *  @param bm      (in): FreeType bitmap info.
     *  @param scaling (in): Scaling from the size of the bitmap for sampling to the size for signed distances to pack.
     *  @param spread  (in): Size of the extra region around the glyph
     *                      that are included in the signed distance 
     *                      calculation
     *  @param xSD    (in): x position in the downsampled local coordinate
     *                      system for the glyph
     *  @param ySD    (in): y position in the downsampled local coordinate
     *                      system for the glyph
     *
     *  @return normalized signed distance. 
     *          < 0.5 : the point is not on the glyph
     *                  the value 0.5 - rtn indicates the normalized
     *                  distance to the closest point 'in' the glyph.
     *            0.5 : the point is on the boundary.
     *          > 0.5 : the point is not on the glyph
     *                  the value rtn - 0.5 indicates the normalized
     *                  distance to the closest point 'out' of the glyph.
     *
     * @dependencies
     *               testOrthogonalPoints()
     *               testDiagonalPoints()
     *               testSymmetricPoints()
     */
    float getSignedDistance(

        FT_Bitmap& bm, 
        float      scaling,
        long       spread, 
        long       xSD, 
        long       ySD
    );


    /** @brief test the points along the X and Y axes distant by 'offset'
     *         from the terget point.
     *         The points are: 
     *             ( xBase,          yBase + offset )
     *             ( xBase,          yBase - offset )
     *             ( xBase + offset, yBase          )
     *             ( xBase - offset, yBase          )
     *
     *  @param bm       (in): FreeType bitmap info.
     *  @param testBase (in): True if the target point is 'in' the glyph.
     *
     *  @param xBase    (in): x position in the downsampled local coordinate
     *                      system for the glyph
     *  @param yBase    (in): y position in the downsampled local coordinate
     *                      system for the glyph
     *
     *  @param offset   (in): distance from the target point.
     *  @return true if at least one of the four points tested is different
     *               from the target point in the in/out categorization.
     * 
     *  @dependencies
     *                isPixelSet()
     */
    bool testOrthogonalPoints(

        FT_Bitmap&  bm, 
        bool        testBase,
        long        xBase,
        long        yBase,
        long        offset
    );


    /** @brief test the points along the diagonal lines distant by 'offset'
     *         from the terget point.
     *         The points are: 
     *             ( xBase + offset, yBase + offset )
     *             ( xBase + offset, yBase - offset )
     *             ( xBase - offset, yBase + offset )
     *             ( xBase - offset, yBase - offset )
     *
     *  @param bm       (in): FreeType bitmap info.
     *  @param testBase (in): True if the target point is 'in' the glyph.
     *
     *  @param xBase    (in): x position in the downsampled local coordinate
     *                      system for the glyph
     *  @param yBase    (in): y position in the downsampled local coordinate
     *                      system for the glyph
     *
     *  @param offset   (in): distance from the target point.
     *  @return true if at least one of the four points tested is different
     *               from the target point in the in/out categorization.
     *
     *  @dependencies
     *                isPixelSet()
     */
    bool testDiagonalPoints(

        FT_Bitmap&  bm, 
        bool        testBase,
        long        xBase,
        long        yBase,
        long        offset
    );


    /** @brief test the 8 symmetric points from the target point by offset1
     *         and offset2.)
     *         from the terget point.
     *         The points are: 
     *             ( xBase + offset1, yBase + offset2 )
     *             ( xBase + offset2, yBase + offset1 )
     *             ( xBase + offset1, yBase - offset2 )
     *             ( xBase + offset2, yBase - offset1 )
     *             ( xBase - offset1, yBase + offset2 )
     *             ( xBase - offset2, yBase + offset1 )
     *             ( xBase - offset1, yBase - offset2 )
     *             ( xBase - offset2, yBase - offset1 )
     *
     *  @param bm       (in): FreeType bitmap info.
     *  @param testBase (in): True if the target point is 'in' the glyph.
     *
     *  @param xBase    (in): x position in the downsampled local coordinate
     *                      system for the glyph
     *  @param yBase    (in): y position in the downsampled local coordinate
     *                      system for the glyph
     *
     *  @param offset1   (in): distance from the target point.
     *  @param offset2   (in): distance from the target point.
     *  @return true if at least one of the 8 points tested is different
     *               from the target point in the in/out categorization.
     *
     *  @dependencies
     *                isPixelSet()
     */
    bool testSymmetricPoints(

        FT_Bitmap&  bm, 
        bool        testBase,
        long        xBase,
        long        yBase,
        long        offset1,
        long        offset2
    );


    /** @brief test if the point is 'in' or 'out' of glyph.
     *
     *  @param bm (in): FreeType bitmap info.
     *
     *  @param x  (in): x position in the FreeType's bitmap coordinate space.
     *  @param y  (in): y position in the FreeType's bitmap coordinate space.
     *
     *  @return true if the point is in the glyph.
     */
    inline bool isPixelSet( FT_Bitmap& bm, long x, long y );

    void setSignedDistBySeparateVicinitySearch( FT_Bitmap& bm );

    void setSignedDistByDeadReckoning( FT_Bitmap& bm );

    class NearestCell {

        short mX;
        short mY;

      public:
        NearestCell():mX(0),mY(0){}

        void set( int x, int y ){ mX = x; mY = y; }

        float getDistFrom( const long i, const long j ) const {

            const auto spanX = static_cast<float>( i - mX );
            const auto spanY = static_cast<float>( j - mY );

            return sqrtf( spanX * spanX + spanY * spanY );
        }

    };

    void doDeadReckoning_initialize( NearestCell* nearestCells );
    void doDeadReckoning_processCellsOnEdges( FT_Bitmap& bm, NearestCell* nearestCells );
    void doDeadReckoning_scanForward( NearestCell* nearestCells );
    void doDeadReckoning_scanBackward( NearestCell* nearestCells );
    void doDeadReckoning_normalizeDistances( FT_Bitmap& bm );

    GeneratorConfig&    mConf;
    long                mCodePoint;

    float               mTextureCoordX;
    float               mTextureCoordY;
    float               mTextureWidth;
    float               mTextureHeight;

    short               mWidth;
    short               mHeight;

    short               mHorizontalBearingX;
    short               mHorizontalBearingY;
    short               mHorizontalAdvance;

    short               mVerticalBearingX;
    short               mVerticalBearingY;
    short               mVerticalAdvance;

    float*              mSignedDist;
    short               mSignedDistWidth;
    short               mSignedDistHeight;
    short               mSignedDistBaseX;
    short               mSignedDistBaseY;

    map< long, FT_Pos > mKernings;

    static const long   FREE_TYPE_FIXED_POINT_SCALING;
};


long InternalGlyphForGen::codePoint() const { return mCodePoint; }


long InternalGlyphForGen::signedDistWidth()  const { return mSignedDistWidth; }


long InternalGlyphForGen::signedDistHeight() const { return mSignedDistHeight;}


long InternalGlyphForGen::baseX() const { return mSignedDistBaseX; }


long InternalGlyphForGen::baseY() const { return mSignedDistBaseY; }


float InternalGlyphForGen::signedDist( long x, long y ) const {

    if ( mSignedDist != nullptr ) {

        return mSignedDist [ y * mSignedDistWidth + x ];
    }
    else {

        return 0.0;
    }
}


bool InternalGlyphForGen::isPixelSet( FT_Bitmap& bm, long x, long y ) {

    if ( x < 0 || y < 0 || x >= bm.width || y >= bm.rows ) {

        return false;
    }

    FT_Byte* p            = bm.buffer;
    FT_Byte* row          = p + ( bm.pitch * y );

    int      byteOffset   = x / 8;
    int      bitOffset    = 7 - ( x % 8 );

    FT_Byte  b            = *( row + byteOffset ) ;

    FT_Byte  bitPosTested = (FT_Byte)( 1L << bitOffset );

    return ( b & bitPosTested ) != 0;

}

} // namespace SDFont

#endif /*__SDFONT_INTERNAL_GLYPH_FOR_GENERATOR_HPP__*/
