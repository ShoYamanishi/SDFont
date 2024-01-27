#include <time.h>
#include "sdfont/generator/generator_option_parser.hpp"
#include "sdfont/generator/generator_config.hpp"
#include "sdfont/generator/generator.hpp"

using namespace std;


/** @file sdfont_generator.cpp
 *
 *  @brief command line tool to invoke SDFont::Generator
 */

int main ( int argc, char* argv[] )
{
    auto tStart = clock();
    SDFont::GeneratorConfig conf;

    SDFont::GeneratorOptionParser parser( conf );

    bool res = parser.parse( argc, argv );

    if ( !res || parser.hasError() || parser.hasHelp() ) {

        cerr << parser.Usage;
        exit(1);
    }

    SDFont::Generator generator( conf, parser.hasVerbose() );


    res = generator.generate();


    if ( !res ) {

        cerr << parser.Usage;
        exit(1);
    }

    res = generator.emitFilePNG();

    if ( !res ) {

        cerr << parser.Usage;
        exit(1);
    }

    res = generator.emitFileMetrics();

    if ( !res ) {

        cerr << parser.Usage;
        exit(1);
    }

    auto tEnd = clock();
    if ( parser.hasVerbose() ) {

        auto tDiffInMillisec =  static_cast<float>( tEnd - tStart ) 
                              / static_cast<float>( CLOCKS_PER_SEC ) * 1000.0f;

        cerr << "Finishied in " << int(tDiffInMillisec) << " milli seconds\n";
    }

    return 0;
}
