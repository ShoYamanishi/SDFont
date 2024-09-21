#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

#include "sdfont/generator/generator_option_parser.hpp"


namespace SDFont {


const string GeneratorOptionParser::Usage = "Usage: "
                                            "sdfont_generator "
                                            "-verbose "
                                            "-font_path [FontPath] "
                                            "-extra_glyph_path [DirPath] "
                                            "-max_code_point [num] "
                                            "-texture_size [num] "
                                            "-glyph_size_for_sampling [num] "
                                            "-ratio_spread_to_glyph [float] "
                                            "-num_threads [num 1-32] "
                                            "-enable_dead_reckoning "
                                            "-reverse_y_direction_for_glyphs "
                                            "[output file name w/o ext]"
                                            "\n";

const string GeneratorOptionParser::FontPath             = "-font_path" ;
const string GeneratorOptionParser::ExtraGlyphPath       = "-extra_glyph_path" ;
const string GeneratorOptionParser::MaxCodePoint         = "-max_code_point" ;
const string GeneratorOptionParser::TextureSize          = "-texture_size" ;
const string GeneratorOptionParser::GlyphSizeForSampling = "-glyph_size_for_sampling" ;
const string GeneratorOptionParser::RatioSpreadToGlyph   = "-ratio_spread_to_glyph" ;
const string GeneratorOptionParser::NumThreads           = "-num_threads" ;
const string GeneratorOptionParser::Encoding             = "-encoding" ;
const string GeneratorOptionParser::EnableDeadReckoning  = "-enable_dead_reckoning" ;
const string GeneratorOptionParser::ReverseYDirectionForGlyphs
                                                         = "-reverse_y_direction_for_glyphs";
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
        else if ( arg.compare ( ExtraGlyphPath ) == 0 ) {

            if ( i < argc - 1 ) {

                string arg2( argv[++i] );
                processExtraGlyphPath( arg2 );
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
        else if ( arg.compare ( NumThreads ) == 0 ) {

            if ( i < argc - 1 ) {

                string arg2( argv[++i] );
                processNumThreads( arg2 );
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
        else if ( arg.compare ( ReverseYDirectionForGlyphs ) == 0 ) {

            processReverseYDirectionForGlyphs( true );
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

void GeneratorOptionParser::processExtraGlyphPath ( const string s ) {

    if ( doesDirectoryExist( s ) ) {

         mConfig.setExtraGlyphPath( s );
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

void GeneratorOptionParser::processNumThreads( const string s ) {

    long numThreads = atoi( s.c_str() ) ;

    if ( numThreads < 1 || numThreads > 32 ) {

        mError = true;
    }
    else {

        mConfig.setNumThreads( numThreads );
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

void GeneratorOptionParser::processReverseYDirectionForGlyphs ( const bool b ) {

    mConfig.setReverseYDirectionForGlyphs ( b );
}


bool GeneratorOptionParser::doesFileExist ( const string s ) const {

    return std::filesystem::is_regular_file( s );
}


bool GeneratorOptionParser::doesDirectoryExist ( const string s ) const {

    return std::filesystem::is_directory( s );
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

















