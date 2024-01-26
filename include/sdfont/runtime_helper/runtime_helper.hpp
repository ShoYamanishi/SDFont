#ifndef __SDFONT_RUNTIME_HELPER__
#define __SDFONT_RUNTIME_HELPER__

#include <vector>
#include <map>

#include "sdfont/glyph.hpp"
#include "sdfont/runtime_helper/metrics_parser.hpp"

using namespace std;

namespace SDFont {

class Rect {
  public:
    float mX; // lower (left) X
    float mY; // lower (bottom) Y
    float mW;
    float mH;

    Rect( const float x, const float y, const float w, const float h ) noexcept
        :mX{x} ,mY{y}, mW{w}, mH{h}{}
};

/** @brief represents one glyph in terms of two bounding boxes: mFrame and mTexture.
 *         mFrame specifies the position of the glyph in a rectangle in the xy rendering coordinate system.
 *         The other (mTexture) is to specify the corresponding location in the uv texture coordinates.
 */
class GlyphBound {
  public:
    Rect mFrame;
    Rect mTexture;

    GlyphBound( const Rect& frame, const Rect& texture ) noexcept : mFrame{frame}, mTexture{texture} {}
};

class RuntimeHelper {

  public:

    static const int NUM_POINTS_PER_GLYPH;
    static const int NUM_FLOATS_PER_POINT;
    static const int NUM_FLOATS_PER_GLYPH;
    static const int NUM_INDICES_PER_GLYPH;

    RuntimeHelper( string fileName );

    virtual ~RuntimeHelper();

    /** @brief
     *
     *  @param c (in): code point
     *
     *  @return pointer to Glyph that constains the metrics.
     *          nullptr if the given code point is not valid.
     */
    Glyph* getGlyph( long c );

    /** @brief spread in lengths in the uv-texture coordinates.
     */
    float& spreadInTexture()     { return mSpreadInTexture;     }

    /** @brief spread in pixels in the font metrics. */
    float& spreadInFontMetrics() { return mSpreadInFontMetrics; }

    map< long, Glyph>& glyphs() { return mGlyphs; }


    /** @brief it generates the follogin metrics in the normalized
     *         texture coordinate system for the given string.
     *
     *         - width
     *
     *         - posXs  list of X positions for each code point in the given
     *                  string. Each position is for the left side of the
     *                  glyph rectangle.
     *                  I.e. The X origin + horizontal bearing.
     *
     *         - height (bearingY + belowBaseLineY) where bearingY is
     *                  the height required above the base line.
     *                  The height is given as the sum of the two terms.
     *
     *         - vertical advancement
     *                  advanceY to place the base line for the next next line
     *                  relative to the current base line.
     *
     *         - glyphs list of Glyphs that contain the metrics for the code
     *                  points.
     *
     *  @param s              (in): string to be displayed.
     *  @param fontSize       (in): font size in pixels.
     *  @param width          (out): See above.
     *  @param posXs          (out): See above.
     *  @param firstBearingX  (out): See above.
     *  @param bearingY       (out): See above.
     *  @param belowBaseLineY (out): See above.
     *  @param advanceY       (out): See above.
     *  @param glyphs         (out): See above.
     */
    void getMetrics(

        string            s,
        float             fontSize,
        float&            width,
        vector< float >&  posXs,
        float&            firstBearingX,
        float&            bearingY,
        float&            belowBaseLineY,
        float&            advanceY,
        vector< Glyph* >& glyphs

    );


    void getMetricsNormalized(

        string            s,
        float&            width,
        vector< float >&  posXs,
        float&            firstBearingX,
        float&            bearingY,
        float&            belowBaseLineY,
        float&            advanceY,
        vector< Glyph* >& glyphs
    );


    /** @brief generates bounding boxes for rendering.
     *
     *  @param glyphs   (in): the list of glyphs. Usually obtained from getMetrics()
     *  @param posXs    (in): the list of the left positions of the glyphs in the
     *                        local coordinate system, i.e., the position for the first element is 0,
     *                        i.e., posXs[0] = 0.
     *                        Usually obtained from getMetrics()
     *  @param startX   (in): the starting X coordinate, that will be aligned with posXs[0].
     *  @param baselineY(in): the baseline horizontal cocordinate.
     *  @param fontSize (in): font size in pixels.
     *  @param spreadRatio
     *                  (in): how much spread to allocate around the glyph in proportion to the
     *                        spread specified in the font in the range of 0.0(no spread) to
     *                        1.0(max spread specified in the font).
     *  @param glyphSpacing
     *                  (in): spacing between glyphs. 1.0 means the original spacing kept in posXs,
     *                        similar to letter-spacing attribute in CSS.
     *
     *  @param bounds   (out):bounding boxes for rendering.
     *
     *  @return true if the generation is successful. false otherwise.
     */
    bool getBoundingBoxes(

        const vector< Glyph* >& glyphs,
        const vector< float >&  posXs,
        const float             startX,
        const float             baselineY,
        const float             fontSize,
        const float             spreadRatio,
        const float             glyphSpacing,
        vector< GlyphBound >&   bounds
    );



    /** @brief generates OpenGL VBOs for the given glyph, i.e.
     *         elements for  GL_ARRAY_BUFFER and
     *         indices for GL_ELEMENT_ARRAY_BUFFER.
     *         Each element for GL_ARRAY_BUFFER consists of 8 floats as follows
     *
     *         float  point   X
     *         float  point   Y
     *         float  point   Z (constant,   Z)
     *         float  normal  X (constant, 0.0)
     *         float  normal  Y (constant, 0.0)
     *         float  normal  Z (constant, 1.0)
     *         float  texture U
     *         float  texture V
     *
     *         It generates 4 elements for one glyph for the corner points
     *         of the rectangle for the glyph in the counter clock wise.
     *
     *         The indices consist of 6 elements for one glyph to represent
     *         two triangles for GL_TRIANGLES.
     *
     *         The glyph is drawn on the plane on the plane perpendicular to
     *         and intersects Z axis at (0, 0, GLZ).
     *         The positive Y-axis indicates the upward direction of the glyph.
     *
     *  @param bounds    (in): glyph bounds
     *
     *  @param Z         (in): Z coordinate of the plane on which the glyph
     *                         is drawn.
     *
     *  @param arrayBuf  (in): the start location in GL_ARRAY_BUFFER
     *                         where the series of values will be stored.
     *
     *  @param indexStart(in): the start index in the GL_ARRAY_BUFFER
     *                         that corresponds to arrayBuf above.
     *
     *  @param indices   (in): the start location in GL_ELEMENT_ARRAY_BUFFER
     *                         where the series of the indices will be stored.
     */
    void generateOpenGLDrawElements (

        const vector< GlyphBound >& bounds,
        const float                 Z,
        float*                      arrayBuf,
        const unsigned int          indexStart,
        unsigned int*               indices
    );

    /** @brief generates OpenGL VBOs for the given glyph, i.e.
     *         elements for  GL_ARRAY_BUFFER and
     *         indices for GL_ELEMENT_ARRAY_BUFFER.
     *         Each element for GL_ARRAY_BUFFER consists of 8 floats as follows
     *
     *         float  point   X
     *         float  point   Y
     *         float  point   Z (constant,   Z)
     *         float  normal  X (constant, 0.0)
     *         float  normal  Y (constant, 0.0)
     *         float  normal  Z (constant, 1.0)
     *         float  texture U
     *         float  texture V
     *
     *         It generates 4 elements for one glyph for the corner points
     *         of the rectangle for the glyph in the counter clock wise.
     *
     *         The indices consist of 6 elements for one glyph to represent
     *         two triangles for GL_TRIANGLES.
     *
     *         The glyph is drawn on the plane on the plane perpendicular to
     *         and intersects Z axis at (0, 0, GLZ).
     *         The positive Y-axis indicates the upward direction of the glyph.
     *
     *  @param g         (in): glyph to be displayed
     *
     *  @param leftX     (in): the left side of the glyph rectangle.
     *                         I.e. the origin of X + horizontal bearing.
     *  @param baseLineY (in): vertical base line.
     *
     *  @param fontSize  (in): font size in pixels.
     *
     *  @param spreadRatio (in): how much spread to allocate around the glyph.
     *
     *  @param Z         (in): Z coordinate of the plane on which the glyph
     *                         is drawn.
     *
     *  @param arrayBuf  (in): the start location in GL_ARRAY_BUFFER
     *                         where the series of values will be stored.
     *
     *  @param indexStart(in): the start index in the GL_ARRAY_BUFFER
     *                         that corresponds to arrayBuf above.
     *
     *  @param indices   (in): the start location in GL_ELEMENT_ARRAY_BUFFER
     *                         where the series of the indices will be stored.
     */
    void generateOpenGLDrawElementsForOneChar (

        Glyph&        g,
        float         leftX,
        float         baseLineY,
        float         fontSize,
        float         spreadRatio,
        float         Z,
        float*        arrayBuf,
        unsigned int  indexStart,
        unsigned int* indices
    );


    /** @brief generates OpenGL VBOs for the given glyphs, i.e.
     *         elements for  GL_ARRAY_BUFFER and
     *         indices for GL_ELEMENT_ARRAY_BUFFER.
     *         Each element for GL_ARRAY_BUFFER consists of 8 floats as follows
     *
     *         float  point   X
     *         float  point   Y
     *         float  point   Z (constant,   Z)
     *         float  normal  X (constant, 0.0)
     *         float  normal  Y (constant, 0.0)
     *         float  normal  Z (constant, 1.0)
     *         float  texture U
     *         float  texture V
     *
     *         It generates 4 elements for one glyph for the corner points
     *         of the rectangle for the glyph in the counter clock wise.
     *
     *         The indices consist of 6 elements for one glyph to represent
     *         two triangles for GL_TRIANGLES.
     *
     *         The glyph is drawn on the plane on the plane perpendicular to
     *         and intersects Z axis at (0, 0, GLZ).
     *         The positive Y-axis indicates the upward direction of the glyph.
     *
     *  @param g         (in): glyph to be displayed
     *
     *  @param leftX     (in): the left side of the glyph rectangle.
     *                         I.e. the origin of X + horizontal bearing.
     *  @param baseLineY (in): vertical base line.
     *
     *  @param fontSize  (in): font size in pixels.
     *
     *  @param spreadRatio (in): how much spread to allocate around the glyph.
     *
     *  @param distribution
     *                   (in): extra scale factor for the horizontal placement
     *                         of the glyphs. For example, if you want to
     *                         strecth the word a little bit because the line
     *                         has too much space, then you can set this to
     *                         1.1. On the other hand, if you want to compress
     *                         the word a little bit to make some space for
     *                         other words, then you can set this to a value
     *                         like 0.95.
     *
     *  @param Z         (in): Z coordinate of the plane on which the glyph
     *                         is drawn.
     *
     *  @param arrayBuf  (in): the start location in GL_ARRAY_BUFFER
     *                         where the series of values will be stored.
     *
     *  @param indexStart(in): the start index in the GL_ARRAY_BUFFER
     *                         that corresponds to arrayBuf above.
     *
     *  @param indices   (in): the start location in GL_ELEMENT_ARRAY_BUFFER
     *                         where the series of the indices will be stored.
     */
    void generateOpenGLDrawElements (

        vector< Glyph* >& glyphs,
        vector< float >&  posXs,
        float             leftX,
        float             baselineY,
        float             fontSize,
        float             spreadRatio,
        float             distribution,
        float             Z,
        float*            arrayBuf,
        unsigned int      indexStart,
        unsigned int*     indices
    );

  private:
    float             mSpreadInTexture;
    float             mSpreadInFontMetrics;
    map< long, Glyph> mGlyphs;
};


} // namespace SDFont

#endif/*__SDFONT_RUNTIME_HELPER__*/
