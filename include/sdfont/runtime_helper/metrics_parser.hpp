#ifndef __SDFONT_METRICS_PARSER_HPP__
#define __SDFONT_METRICS_PARSER_HPP__

#include <map>
#include <string>
#include "sdfont/glyph.hpp"
#include "sdfont/char_map.hpp"

using namespace std;

namespace SDFont {

class MetricsParser {

  public:


    /** @brief constructor
     *
     *  @param  G (in/out) graph to which nodes and edges are to be added
     */
    MetricsParser( map< long, Glyph>& glyphs, float& spreadInTexture, float& spreadInFontMetrics, vector< CharMap >& charMaps ):
        mSpreadInTexture(spreadInTexture),
        mSpreadInFontMetrics(spreadInFontMetrics),
        mGlyphs(glyphs),
        mCharMaps( charMaps ) {;}


    virtual ~MetricsParser(){;}


    /** @brief
     *
     *  @param  filename (in): name of the file to be opened and parsed.
     *
     */
    bool parseSpec( string fileName );

    static const string SPREAD_IN_TEXTURE;
    static const string SPREAD_IN_FONT_METRICS;
    static const string GLYPHS;
    static const string KERNINGS;
    static const string CHAR_MAPS;
    static const string CHAR_MAP_DEFAULT;

  private:

    enum parseState {
        INIT,
        IN_SPREAD_IN_TEXTURE,
        IN_SPREAD_IN_FONT_METRICS,
        IN_GLYPHS,
        IN_KERNINGS,
        IN_CHAR_MAPS,
        END
    };


    void trim( string& line );


    bool isSectionHeader( string line, enum parseState& state );


    bool isCommentLine  ( string line );


    size_t splitLine    ( const string& txt, vector<string>& strs, char ch );


    void handleSpreadInTexture(
        string line,
        string filename,
        long   lineNumber,
        bool&  errorFlag
    );

    void handleSpreadInFontMetrics(
        string line,
        string filename,
        long   lineNumber,
        bool&  errorFlag
    );

    void handleGlyph(
        string line,
        string filename,
        long   lineNumber,
        bool&  errorFlag
    );


    void handleKerning(
        string line,
        string fileName,
        long   lineNumber,
        bool&  errorFlag
    );

    void handleCharMap(
        string line,
        string fileName,
        long   lineNumber,
        bool&  errorFlag
    );


    void emitError(
        string fileName,
        long   lineNumber,
        string mess,
        bool&  errorFlag
    );


    float& mSpreadInTexture;
    float& mSpreadInFontMetrics;

    /** @brief used during parsing to find a node from a node number.*/
    map< long, Glyph >& mGlyphs;
    vector< CharMap >&  mCharMaps;
};


} // namespace SDFont

#endif /*__SDFONT_METRICS_PARSER_HPP__*/
