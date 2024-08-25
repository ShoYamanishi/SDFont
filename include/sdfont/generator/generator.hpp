#ifndef __SDFONT_GENERATOR_HPP__
#define __SDFONT_GENERATOR_HPP__

#include <vector>

#include <ft2build.h>
#include FT_FREETYPE_H


#include "sdfont/generator/internal_glyph_for_generator.hpp"
#include "sdfont/generator/generator_config.hpp"

namespace SDFont {

class Generator {

  public:

    Generator(GeneratorConfig& conf, bool verbose);

    virtual ~Generator();

    bool generate();
    bool emitFilePNG();
    unsigned char** textureBitmap();
    void releaseTexture ();
    bool emitFileMetrics ();
    void generateMetrics(float& margin, vector<Glyph>& glyphs);

    static const string Encoding_unicode;
    static const string Encoding_ms_symbol;
    static const string Encoding_sjis;
    static const string Encoding_prc;
    static const string Encoding_big5;
    static const string Encoding_wansung;
    static const string Encoding_johab;
    static const string Encoding_adobe_latin_1;
    static const string Encoding_adobe_standard;
    static const string Encoding_adobe_expert;
    static const string Encoding_adobe_custom;
    static const string Encoding_apple_roman;
    static const string Encoding_old_latin_2;

  private:

    bool  initializeFreeType      ( ) ;
    bool  generateGlyphs          ( ) ;
    void  generateExtraGlyphs     ( ) ;
    std::pair<float, float>
          findMeanGlyphDimension  ( ) ;
    void  addExtraGlyph           ( const long code_point, const std::pair<float, float>& dim, const std::string& file_name );
    void  getKernings             ( ) ;
    long  fitGlyphsToTexture      ( ) ;
    long  findBestWidthForDefaultFontSize( long& bestHeight, long& maxNumGlyphsPerEdge );
    long  findHeightFromWidth     ( const long width, long& maxNumGlyphsPerEdge );
    bool  generateGlyphBitmaps    ( long bestWidthForDefaultFontSize ) ;
    bool  generateTexture         ( bool reverseY ) ;
    FT_Error setEncoding          ( const string& s );

    GeneratorConfig&               mConf;
    bool                           mVerbose;
    FT_Library                     mFtHandle;
    FT_Face                        mFtFace;
    vector< InternalGlyphForGen* > mGlyphs;
    unsigned char*                 mPtrMain;
    unsigned char**                mPtrArray;

};

} // namespace SDFont


#endif /*__SDFONT_GENERATOR_HPP__*/

