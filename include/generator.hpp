#ifndef __GENERATOR_HPP__
#define __GENERATOR_HPP__

#include <vector>

#include <ft2build.h>
#include FT_FREETYPE_H


#include "internal_glyph_for_gen.hpp"
#include "generator_config.hpp"

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


#endif /*__GENERATOR_HPP__*/

