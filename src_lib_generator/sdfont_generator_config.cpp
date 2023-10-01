#include "sdfont/generator/sdfont_generator_config.hpp"

namespace SDFont {

const string GeneratorConfig::DefaultLocale = "C" ;
const string GeneratorConfig::DefaultFontPath = "/usr/share/fonts/Arial.ttf" ;
const string GeneratorConfig::DefaultOutputFileName = "signed_dist_font" ;

const long   GeneratorConfig::DefaultMaxCodePoint           =  255 ;
const long   GeneratorConfig::DefaultOutputTextureSize      =  512 ;
const float  GeneratorConfig::DefaultRatioSpreadToGlyph     =  0.2f ;
const long   GeneratorConfig::DefaultGlyphBitmapSizeForSampling = 1024 ;

void GeneratorConfig::emitVerbose() const {

    cerr << "Signed Distance Font Generator [2019]\n";
    cerr << "Configured as follows.\n";
    cerr << "Locale: ["           << mLocale             << "]\n";
    cerr << "Font Path: ["        << mFontPath           << "]\n";
    cerr << "Output File Name: [" << mOutputFileName     << "]\n";
    cerr << "Max Code Point: ["   << mMaxCodePoint       << "]\n";
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


