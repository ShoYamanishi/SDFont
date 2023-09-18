#ifndef __SDFONT_GENERATOR_OPTION_PARSE_HPP__
#define __SDFONT_GENERATOR_OPTION_PARSE_HPP__

#include <string>
#include "sdfont_generator_config.hpp"

namespace SDFont {

class GeneratorOptionParser {

  public:

    GeneratorOptionParser( GeneratorConfig& config );
    virtual ~GeneratorOptionParser();

    bool        parse ( int argc, char* argv[] );

    inline bool hasHelp   () const { return mHelp;    }
    inline bool hasError  () const { return mError;   }
    inline bool hasVerbose() const { return mVerbose; }

    static const string Usage;

  private:

    inline void reset() { mHelp = false; mError = false; mVerbose = false; }

    void processLocale         ( const string s ) ;
    void processFontPath       ( const string s ) ;
    void processMaxCodePoint   ( const string s ) ;
    void processTextureSize    ( const string s ) ;
    void processResolution     ( const string s ) ;
    void processSpreadInPixels ( const string s ) ;
    void processOutputFileName ( const string s ) ;
    bool doesFileExist         ( const string s ) const ;
    bool isValidFileName       ( const string s ) const ;

    GeneratorConfig&      mConfig;
    bool                  mError;
    bool                  mHelp;
    bool                  mVerbose;

    static const string   Locale;
    static const string   FontPath;
    static const string   MaxCodePoint;
    static const string   TextureSize;
    static const string   Resolution;
    static const string   SpreadInPixels;
    static const string   Help;
    static const string   DashH;
    static const string   Verbose;

};

} // namespace SDFont

#endif /*__SDFONT_GENERATOR_OPTION_PARSE_HPP__*/




















