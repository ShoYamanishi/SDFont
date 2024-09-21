#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "sdfont/runtime_helper/metrics_parser.hpp"

namespace SDFont {

const std::string MetricsParser::SPREAD_IN_TEXTURE      = "SPREAD IN TEXTURE";
const std::string MetricsParser::SPREAD_IN_FONT_METRICS = "SPREAD IN FONT METRICS";
const std::string MetricsParser::GLYPHS                 = "GLYPHS";
const std::string MetricsParser::KERNINGS               = "KERNINGS";
const std::string MetricsParser::CHAR_MAPS              = "CHAR MAPS";
const std::string MetricsParser::CHAR_MAP_DEFAULT       = "default";

void MetricsParser::trim( string& line ) {

    if ( !line.empty() && line[line.size() - 1] == '\n' ) {

        line.erase(line.size() - 1);
    }

    if ( !line.empty() && line[line.size() - 1] == '\r' ) {

        line.erase(line.size() - 1);
    }
}

bool MetricsParser::parseSpec( string fileName )
{

    ifstream        is( fileName.c_str() );

    if (!is) {
        return false;
    }

    long            lineNumber = 0;
    bool            error      = false;
    enum parseState state      = INIT;

    while ( !is.eof() && !error ) {

        string line;

        getline( is, line );

        trim( line );

        lineNumber++;

        if ( line.empty() ) {
            continue;
        }

        if ( isCommentLine(line) ) {
            continue;
        }

        if( isSectionHeader( line, state ) ) {
            continue;
        }

        switch( state ) {

          case INIT:

            emitError     ( fileName, lineNumber, "", error );
            break;

          case IN_SPREAD_IN_TEXTURE:

            handleSpreadInTexture( line, fileName, lineNumber, error );
            break;

          case IN_SPREAD_IN_FONT_METRICS:

            handleSpreadInFontMetrics( line, fileName, lineNumber, error );
            break;

          case IN_GLYPHS:

            handleGlyph   ( line, fileName, lineNumber, error );
            break;

          case IN_KERNINGS:

            handleKerning ( line, fileName, lineNumber, error );
            break;

          case IN_CHAR_MAPS:

            handleCharMap ( line, fileName, lineNumber, error );
            break;

          case END:
          default:

            emitError     ( fileName, lineNumber, "", error );
            break;
        }
    }

    if (error) {
        return false;
    }

    return true;
}


bool MetricsParser::isSectionHeader (

    string           line,
    enum parseState& state

) {
    if ( line.compare( 0, SPREAD_IN_TEXTURE.size(), SPREAD_IN_TEXTURE ) == 0 ) {

        state = IN_SPREAD_IN_TEXTURE;
        return true;
    }
    else if ( line.compare( 0, SPREAD_IN_FONT_METRICS.size(), SPREAD_IN_FONT_METRICS ) == 0 ) {

        state = IN_SPREAD_IN_FONT_METRICS;
        return true;
    }
    else if ( line.compare( 0, GLYPHS.size(), GLYPHS ) == 0 ) {

        state = IN_GLYPHS;
        return true;
    }
    else if ( line.compare( 0, KERNINGS.size(), KERNINGS ) == 0 ) {

        state = IN_KERNINGS;
        return true;
    }
    else if ( line.compare( 0, CHAR_MAPS.size(), CHAR_MAPS ) == 0 ) {

        state = IN_CHAR_MAPS;
        return true;
    }

    return false;
}


void MetricsParser::emitError(

    string fileName,
    long   lineNumber,
    string message ,
    bool&  errorFlag

) {

    cerr << "Syntax Error: "
         << fileName
         << " at line: "
         << lineNumber
         << " "
         << message
         << "\n";

    errorFlag = true;
}


bool MetricsParser::isCommentLine( std::string line )
{
    return line.at(0) == '#';
}


size_t MetricsParser::splitLine(

    const string&   txt,
    vector<string>& strs,
    char            ch

) {

    auto   pos        = txt.find( ch );
    size_t initialPos = 0;

    strs.clear();

    while( pos != std::string::npos && initialPos < txt.size() ) {

        if ( pos > initialPos ) {

            strs.push_back( txt.substr( initialPos, pos - initialPos ) );
        }

        initialPos = pos + 1;

        if ( initialPos < txt.size() ) {
            pos = txt.find( ch, initialPos );
        }

    }

    if ( initialPos < txt.size() ) {

        strs.push_back( txt.substr( initialPos, txt.size() - initialPos ) );
    }

    return strs.size();
}


void MetricsParser::handleSpreadInTexture (

    string line,
    string filename,
    long   lineNumber,
    bool&  errorFlag

) {

    vector<std::string> fields;

    if ( splitLine( line, fields, '\t' ) != 1 ) {

        emitError( filename, lineNumber, "Invalid Spread in Texture", errorFlag );
        return;
    }

    Glyph g;

    mSpreadInTexture = stof( fields[ 0] );
}


void MetricsParser::handleSpreadInFontMetrics (

    string line,
    string filename,
    long   lineNumber,
    bool&  errorFlag

) {

    vector<std::string> fields;

    if ( splitLine( line, fields, '\t' ) != 1 ) {

        emitError( filename, lineNumber, "Invalid Spread in Font Metrics", errorFlag );
        return;
    }

    Glyph g;

    mSpreadInFontMetrics = stof( fields[ 0] );
}

static long convertToLong( const std::string& s )
{
    const auto trimmed_s = s.substr( 2, s.size() - 2 ); // remove "0X"

    return stol( trimmed_s, 0, 16);
}

static unsigned long convertToUnsignedLong( const std::string& s )
{
    const auto trimmed_s = s.substr( 2, s.size() - 2 ); // remove "0X"

    return stoul( trimmed_s, 0, 16);
}


void MetricsParser::handleGlyph (

    string line,
    string filename,
    long   lineNumber,
    bool&  errorFlag

) {

    vector<std::string> fields;

    if ( splitLine( line, fields, '\t' ) != 14 ) {

        emitError( filename, lineNumber, "Invalid Node", errorFlag );
        return;
    }

    Glyph g;

    g.mCodePoint          = convertToLong( fields[ 0 ] );
    g.mGlyphName          = fields[ 1];
    g.mWidth              = stof( fields[ 2] );
    g.mHeight             = stof( fields[ 3] );
    g.mHorizontalBearingX = stof( fields[ 4] );
    g.mHorizontalBearingY = stof( fields[ 5] );
    g.mHorizontalAdvance  = stof( fields[ 6] );
    g.mVerticalBearingX   = stof( fields[ 7] );
    g.mVerticalBearingY   = stof( fields[ 8] );
    g.mVerticalAdvance    = stof( fields[ 9] );
    g.mTextureCoordX      = stof( fields[10] );
    g.mTextureCoordY      = stof( fields[11] );
    g.mTextureWidth       = stof( fields[12] );
    g.mTextureHeight      = stof( fields[13] );

    mGlyphs[ g.mCodePoint ] = g;
}


void MetricsParser::handleKerning (

    string      line,
    string      filename,
    long        lineNumber,
    bool&       errorFlag

) {

    vector<std::string> fields;

    auto numFields = splitLine( line, fields, '\t' );

    if ( numFields < 3 || (numFields - 1) % 2 != 0 ) {

        emitError( filename, lineNumber, "Invalid Kerning Line", errorFlag );
    }

    auto& g = mGlyphs[ convertToLong( fields[ 0] ) ];

    for ( int i = 1; i < numFields; i +=2 ) {

        g.mKernings[ convertToLong( fields[ i ] ) ] = stof( fields[ i+1 ] );
    }
}


void MetricsParser::handleCharMap (

    string      line,
    string      filename,
    long        lineNumber,
    bool&       errorFlag
) {

    vector<std::string> fields;

    auto numFields = splitLine( line, fields, '\t' );

    if ( numFields < 5 || (numFields - 5) % 2 != 0 ) {

        emitError( filename, lineNumber, "Invalid Char Map Line", errorFlag );
    }

    CharMap mp(
        ( fields[ 3 ].compare( 0, CHAR_MAP_DEFAULT.size(), CHAR_MAP_DEFAULT ) == 0 ),
        fields[ 0 ],
        stol( fields[ 1 ] ),
        stol( fields[ 2 ] )
    );

    const auto num_elems = stol( fields[ 4 ] );

    for ( int i = 5; i < numFields; i +=2 ) {

        const uint32_t charCode       = convertToUnsignedLong( fields[ i     ] );
        const uint32_t glyphCodePoint = convertToUnsignedLong( fields[ i + 1 ] );

        mp.m_char_to_codepoint.insert( pair( charCode, glyphCodePoint ) );
    }

    mCharMaps.push_back( mp );
}

} // namespace SDFont
