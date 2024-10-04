#ifndef __SDFONT_GENERATOR_OPTION_PARSE_HPP__
#define __SDFONT_GENERATOR_OPTION_PARSE_HPP__

#include <string>
#include "sdfont/generator/generator_config.hpp"

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

    void processFontPath             ( const string& s ) ;
    void processExtraGlyphPath       ( const string& s ) ;
    void processTextureSize          ( const string& s ) ;
    void processGlyphSizeForSampling ( const string& s ) ;
    void processRatioSpreadToGlyph   ( const string& s ) ;
    void processCharCodeRange        ( const string& s ) ;
    void processProcessHiddenGlyphs  ( const bool    b ) ;
    void processNumThreads           ( const string& s ) ;
    void processOutputFileName       ( const string& s ) ;
    void processEncoding             ( const string& s ) ;
    void processDeadReckoning        ( const bool    b );
    void processReverseYDirectionForGlyphs
                                     ( const bool    b );
    bool doesFileExist               ( const string& s ) const ;
    bool doesDirectoryExist          ( const string& s ) const ;
    bool isValidFileName             ( const string& s ) const ;


    GeneratorConfig&      mConfig;
    bool                  mError;
    bool                  mHelp;
    bool                  mVerbose;

    static const string   FontPath;
    static const string   ExtraGlyphPath;
    static const string   TextureSize;
    static const string   GlyphSizeForSampling;
    static const string   RatioSpreadToGlyph;
    static const string   ProcessHiddenGlyphs;
    static const string   CharCodeRange;
    static const string   NumThreads;
    static const string   EnableDeadReckoning;
    static const string   ReverseYDirectionForGlyphs;
    static const string   Help;
    static const string   DashH;
    static const string   Verbose;
    static const string   Encoding;
};

} // namespace SDFont

#endif /*__SDFONT_GENERATOR_OPTION_PARSE_HPP__*/




















