#include <chrono>
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
    auto time_begin = std::chrono::high_resolution_clock::now();

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

    auto time_end = std::chrono::high_resolution_clock::now();

    if ( parser.hasVerbose() ) {

        std::chrono::duration<double> time_diff = time_end - time_begin;
        std::cerr << "\n\nFinished in " << time_diff.count() << "[s].\n\n";
    }

    return 0;
}
