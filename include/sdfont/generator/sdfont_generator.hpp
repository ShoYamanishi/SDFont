#ifndef __SDFONT_GENERATOR_HPP__
#define __SDFONT_GENERATOR_HPP__

#include <vector>

#include <ft2build.h>
#include FT_FREETYPE_H


#include "sdfont/generator/sdfont_internal_glyph_for_generator.hpp"
#include "sdfont/generator/sdfont_generator_config.hpp"

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

  private:

    bool  initializeFreeType      ( ) ;
    bool  generateGlyphs          ( ) ;
    void  getKernings             ( ) ;
    long  fitGlyphsToTexture      ( ) ;
    long  bestNumberOfItemsPerRow ( long& X, long& Y ) ;
    float findDimension           ( long itemsPerRow, long& X, long& Y ) ;
    bool  generateGlyphBitmaps    ( long numItemsPerRow ) ;
    bool  generateTexture         ( bool reverseY ) ;

    GeneratorConfig&               mConf;
    bool                           mVerbose;
    FT_Library                     mFtHandle;
    FT_Face                        mFtFace;
    vector< InternalGlyphForGen >  mGlyphs;
    unsigned char*                 mPtrMain;
    unsigned char**                mPtrArray;

};

} // namespace SDFont


#endif /*__SDFONT_GENERATOR_HPP__*/

