#include "sdfont/generator/generator_config.hpp"

namespace SDFont {

const string GeneratorConfig::DefaultFontPath = "/usr/share/fonts/Arial.ttf" ;
const string GeneratorConfig::DefaultOutputFileName = "signed_dist_font" ;
const string GeneratorConfig::DefaultCodepointRangeFilePath = "" ;
const string GeneratorConfig::DefaultEncoding = "unicode" ;

const long   GeneratorConfig::DefaultMaxCodePoint           =  255 ;
const long   GeneratorConfig::DefaultOutputTextureSize      =  512 ;
const float  GeneratorConfig::DefaultRatioSpreadToGlyph     =  0.2f ;
const long   GeneratorConfig::DefaultGlyphBitmapSizeForSampling = 1024 ;

void GeneratorConfig::setCodepointRangeFilePath( string s )
{
    mCodepointRangeFilePath = s ;

    ifstream is( s.c_str() );

    if (!is) {
        return;
    }

    long            lineNumber = 0;
    bool            error      = false;

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

        vector<std::string> fields;

        if ( splitLine( line, fields, ',' ) == 2 ) {
            trim( fields[0] );
            trim( fields[1] );
            mCodepointRangePairs.emplace_back( atoi( fields[0].c_str() ), atoi( fields[1].c_str() ) );
        }
    }
}

void GeneratorConfig::trim( string& line ) const
{
    if ( !line.empty() && line[line.size() - 1] == '\n' ) {

        line.erase(line.size() - 1);
    }

    if ( !line.empty() && line[line.size() - 1] == '\r' ) {

        line.erase(line.size() - 1);
    }
}

bool GeneratorConfig::isCommentLine( const std::string& line ) const
{
    return line.at(0) == '#';
}

size_t GeneratorConfig::splitLine(

    const string&   txt,
    vector<string>& strs,
    const char      ch

) const {

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


void GeneratorConfig::emitVerbose() const {

    cerr << "Signed Distance Font Generator [2019]\n";
    cerr << "Configured as follows.\n";
    cerr << "Font Path: ["        << mFontPath           << "]\n";
    cerr << "Encoding: ["         << mEncoding           << "]\n";
    cerr << "Codepoint Range File Path: ["
                                  << mCodepointRangeFilePath << "]\n";
    cerr << "Output File Name: [" << mOutputFileName     << "]\n";
    cerr << "Max Code Point: ["   << mMaxCodePoint       << "]\n";
    cerr << "Codepoint Range Pairs(low, high+1): [";
    for ( const auto& pair : mCodepointRangePairs ) {
        cerr << " (" << pair.first << "," << pair.second << ")";
    }
    cerr << "]\n";
    cerr << "Output Texture Size: ["     << mOutputTextureSize        << "]\n";
    cerr << "Initial Glyph Scaling from Sampling oo Packed Signed Dist: ["    << mGlyphScalingFromSamplingToPackedSignedDist << "]\n";
    cerr << "Glyph Bitmap Size for Sampling: ["  << glyphBitmapSizeForSampling()        << "]\n";
    cerr << "Ratio Spread to Glyph: [" << ratioSpreadToGlyph()   << "]\n";
}


void GeneratorConfig::outputMetricsHeader( ostream& os ) const  {

    os << "# Source Font Path: ";
    os << mFontPath;
    os << "\n";
    os << "# Output Texture Size: ";
    os << mOutputTextureSize;
    os << "\n";
    os << "# Encoding: ";
    os << mEncoding;
    os << "\n";
    os << "# Highest Code Point: ";
    os << mMaxCodePoint;
    os << "\n";
    os << "# Codepoint Range Pairs:(low, high+1) [";
    for ( const auto& pair : mCodepointRangePairs ) {
        os << " (" << pair.first << "," << pair.second << ")";
    }
    os << "]\n";
    os << "# Glyph Bitmap Size for Sampling: ";
    os << glyphBitmapSizeForSampling();
    os << "\n";
    os << "# Ratio Spread to Glyph: ";
    os << ratioSpreadToGlyph();
    os << "\n";
    os << "# Glyph Scaling from Sampling to Packed Signed Dist: ";
    os << mGlyphScalingFromSamplingToPackedSignedDist;
    os << "\n";
    os << "# Associated Texture File: ";
    os << mOutputFileName << ".png\n";
    os << "#\t";
    os << "Code Point";
    os << "\t";
    os << "Width";
    os << "\t";
    os << "Height";
    os << "\t";
    os << "Horizontal Bearing X";
    os << "\t";
    os << "Horizontal Bearing Y";
    os << "\t";
    os << "Horizontal Advance";
    os << "\t";
    os << "Vertical Bearing X";
    os << "\t";
    os << "Vertical Bearing Y";
    os << "\t";
    os << "Vertical Advance";
    os << "\t";
    os << "Texture Coord X";
    os << "\t";
    os << "Texture Coord Y";
    os << "\t";
    os << "Texture Width";
    os << "\t";
    os << "Texture Height";
    os << "\n";
}

} // namespace SDFont


