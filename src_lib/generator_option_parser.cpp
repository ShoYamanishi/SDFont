#include <iostream>
#include <fstream>
#include <sstream>

#include "generator_option_parser.hpp"


namespace SDFont {


const string GeneratorOptionParser::Usage = "Usage: "
                                            "sdfont_generator "
                                            "-locale [Locale] "
                                            "-font_path [FontPath] "
                                            "-max_code_point [num] "
                                            "-texture_size [num] "
                                            "[output file name w/o ext]"
                                            "\n";

const string GeneratorOptionParser::Locale       = "-locale" ;
const string GeneratorOptionParser::FontPath     = "-font_path" ;
const string GeneratorOptionParser::MaxCodePoint = "-max_code_point" ;
const string GeneratorOptionParser::TextureSize  = "-texture_size" ;
const string GeneratorOptionParser::Help         = "-help" ;
const string GeneratorOptionParser::DashH        = "-h" ;
const string GeneratorOptionParser::Verbose      = "-verbose" ;


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
        else if ( arg.compare ( Locale ) == 0 ) {

            if ( i < argc - 1 ) {

                string arg2( argv[++i] );
                processLocale( arg2 );
            }
            else {
                mError = true;
                break;
            }
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
        else {

            processOutputFileName( arg );
        }
    }

    return !mError;
}


void GeneratorOptionParser::processLocale ( const string s ) {

    if ( nullptr != setlocale( LC_ALL, s.c_str() ) ) {

         mConfig.setLocale( s );
    }
    else {

        mError = true;
    }
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

        mConfig.setTextureSize( textureSize );
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

















