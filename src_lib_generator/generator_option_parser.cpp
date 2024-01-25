#include <iostream>
#include <fstream>
#include <sstream>

#include "sdfont/generator/generator_option_parser.hpp"


namespace SDFont {


const string GeneratorOptionParser::Usage = "Usage: "
                                            "sdfont_generator "
                                            "-verbose "
                                            "-font_path [FontPath] "
                                            "-max_code_point [num] "
                                            "-texture_size [num] "
                                            "-glyph_size_for_sampling [num] "
                                            "-ratio_spread_to_glyph [float] "
                                            "-codepoint_range_file_path [FilePath] "
                                            "-encoding [unicode(default) / ms_symbol / sjis / prc / big5 / wansung / johab / adobe_latin_1 / adobe_standard / adobe_expert / adobe_custom / apple_roman / old_latin_2] "
                                            " -enable_dead_reckoning "
                                            "[output file name w/o ext]"
                                            "\n";

const string GeneratorOptionParser::FontPath             = "-font_path" ;
const string GeneratorOptionParser::MaxCodePoint         = "-max_code_point" ;
const string GeneratorOptionParser::TextureSize          = "-texture_size" ;
const string GeneratorOptionParser::GlyphSizeForSampling = "-glyph_size_for_sampling" ;
const string GeneratorOptionParser::RatioSpreadToGlyph   = "-ratio_spread_to_glyph" ;
const string GeneratorOptionParser::Encoding             = "-encoding" ;
const string GeneratorOptionParser::EnableDeadReckoning  = "-enable_dead_reckoning" ;
const string GeneratorOptionParser::Help                 = "-help" ;
const string GeneratorOptionParser::DashH                = "-h" ;
const string GeneratorOptionParser::Verbose              = "-verbose" ;
const string GeneratorOptionParser::CodepointRangeFilePath = "-codepoint_range_file_path" ;

GeneratorOptionParser::GeneratorOptionParser( GeneratorConfig& config ):
    mConfig ( config )
    {;}


GeneratorOptionParser::~GeneratorOptionParser() {;}


bool GeneratorOptionParser::parse( int argc, char* argv[] )
{
    reset();

    for ( auto i = 1; i < argc ; i++ ) {

        string arg( argv[i] );

        if ( arg.compare ( Help ) == 0 || arg.compare ( DashH ) == 0 ) {

            mHelp = true;
        }
        else if ( arg.compare ( Verbose ) == 0 ) {

            mVerbose = true;
        }
        else if ( arg.compare ( FontPath ) == 0 ) {

            if ( i < argc - 1 ) {

                string arg2( argv[++i] );
                processFontPath( arg2 );
            }
            else {
                mError = true;
                break;
            }
        }
        else if ( arg.compare ( MaxCodePoint ) == 0 ) {

            if ( i < argc - 1 ) {

                string arg2( argv[++i] );
                processMaxCodePoint( arg2 );
            }
            else {
                mError = true;
                break;
            }
        }
        else if ( arg.compare ( TextureSize ) == 0 ) {

            if ( i < argc - 1 ) {

                string arg2( argv[++i] );
                processTextureSize( arg2 );
            }
            else {
                mError = true;
                break;
            }
        }
        else if ( arg.compare ( GlyphSizeForSampling ) == 0 ) {

            if ( i < argc - 1 ) {

                string arg2( argv[++i] );
                processGlyphSizeForSampling( arg2 );
            }
            else {
                mError = true;
                break;
            }
        }
        else if ( arg.compare ( RatioSpreadToGlyph ) == 0 ) {

            if ( i < argc - 1 ) {

                string arg2( argv[++i] );
                processRatioSpreadToGlyph( arg2 );
            }
            else {
                mError = true;
                break;
            }
        }
        else if ( arg.compare ( CodepointRangeFilePath ) == 0 ) {

            if ( i < argc - 1 ) {

                string arg2( argv[++i] );
                processCodepointRangeFilePath( arg2 );
            }
            else {
                mError = true;
                break;
            }
        }
        else if ( arg.compare ( Encoding ) == 0 ) {

            if ( i < argc - 1 ) {

                string arg2( argv[++i] );
                processEncoding( arg2 );
            }
            else {
                mError = true;
                break;
            }
        }
        else if ( arg.compare ( EnableDeadReckoning ) == 0 ) {

            processDeadReckoning( true );
        }
	else {

            processOutputFileName( arg );
        }
    }

    return !mError;
}


void GeneratorOptionParser::processFontPath ( const string s ) {

    if ( doesFileExist( s ) ) {

         mConfig.setFontPath( s );
    }
        else {

        mError = true;
    }
}


void GeneratorOptionParser::processMaxCodePoint( const string s ) {

    long maxCodePoint = atoi( s.c_str() ) ;

    if ( maxCodePoint < 0 ) {

        mError = true;
    }
    else {
         mConfig.setMaxCodePoint( maxCodePoint );
    }
}


void GeneratorOptionParser::processTextureSize( const string s ) {

    long textureSize = atoi( s.c_str() ) ;

    if ( textureSize < 0 ) {

        mError = true;
    }
    else {

        mConfig.setOutputTextureSize( textureSize );
    }
}


void GeneratorOptionParser::processGlyphSizeForSampling( const string s ) {

    long glyphSizeForSampling = atoi( s.c_str() ) ;

    if ( glyphSizeForSampling < 0 || glyphSizeForSampling > 8192) {

        mError = true;
    }
    else {

        mConfig.setGlyphBitmapSizeForSampling( glyphSizeForSampling );
    }
}


void GeneratorOptionParser::processRatioSpreadToGlyph( const string s ) {

    float ratio = atof( s.c_str() ) ;

    if ( ratio <= 0.0f || ratio > 1.0f) {

        mError = true;
    }
    else {

        mConfig.setRatioSpreadToGlyph( ratio );
    }
}


void GeneratorOptionParser::processOutputFileName( const string s ) {

    if ( isValidFileName ( s ) ) {

        mConfig.setOutputFileName( s );
    }
    else {

        mError = true;
    }
}


void GeneratorOptionParser::processCodepointRangeFilePath ( const string s ) {

    if ( doesFileExist( s ) ) {

         mConfig.setCodepointRangeFilePath( s );
    }
        else {

        mError = true;
    }
}


void GeneratorOptionParser::processEncoding ( const string s ) {

    mConfig.setEncoding(s);
}

void GeneratorOptionParser::processDeadReckoning ( const bool b ) {

    mConfig.setDeadReckoning( b );
}


bool GeneratorOptionParser::doesFileExist ( const string s ) const {

    ifstream fp( s );

    if ( fp ) {

        return true;
    }
    else {

        return false;
    }
}


bool GeneratorOptionParser::isValidFileName ( const string s ) const {

    ofstream fp( s );

    if ( fp ) {

        fp.close();

        remove( s.c_str() );

        return true;
    }
    else {

        return false;
    }
}

} // namespace SDFont

















