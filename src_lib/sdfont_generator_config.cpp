#include "sdfont_generator_config.hpp"

namespace SDFont {

const string GeneratorConfig::DefaultLocale = "en_US" ;
const string GeneratorConfig::DefaultFontPath = "/usr/share/fonts/Arial.ttf" ;
const string GeneratorConfig::DefaultOutputFileName = "signed_dist_font" ;

const long   GeneratorConfig::DefaultMaxCodePoint           =  255 ;
const long   GeneratorConfig::DefaultTextureSize            =  512 ;
const long   GeneratorConfig::DefaultScale                  =  128 ;
const long   GeneratorConfig::DefaultSpreadInPixels         =  128 ;
const long   GeneratorConfig::DefaultBaseResolutionInPixels = 1024 ;


void GeneratorConfig::emitVerbose() const {

    cerr << "Signed Distance Font Generator [2019]\n";
    cerr << "Configured as follows.\n";
    cerr << "Locale: ["           << mLocale             << "]\n";
    cerr << "Font Path: ["        << mFontPath           << "]\n";
    cerr << "Output File Name: [" << mOutputFileName     << "]\n";
    cerr << "Max Code Point: ["   << mMaxCodePoint       << "]\n";
    cerr << "Texture Size: ["     << mTextureSize        << "]\n";
    cerr << "Initial Scale: ["    << mScale              << "]\n";
    cerr << "Font Resolution: ["  << resolution()        << "]\n";
    cerr << "Spread In Pixels: [" << spreadInPixels()   << "]\n";
}


void GeneratorConfig::outputMetricsHeader( ostream& os ) const  {

    os << "# Source Font Path: ";
    os << mFontPath;
    os << "\n";
    os << "# Texture Size: ";
    os << mTextureSize;
    os << "\n";
    os << "# Original Font Resolution: ";
    os << resolution();
    os << "\n";
    os << "# Spread in pixels: ";
    os << spreadInPixels();
    os << "\n";
    os << "# Down Sampling Scale: ";
    os << mScale;
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


