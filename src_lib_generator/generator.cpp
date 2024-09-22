#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <png.h>
#include <filesystem>

#include "sdfont/generator/generator.hpp"
#include "sdfont/generator/png_loader.hpp"
#include "sdfont/free_type_utilities.hpp"

namespace SDFont {

const string Generator::Encoding_unicode        = "unicode";
const string Generator::Encoding_ms_symbol      = "ms_symbol";
const string Generator::Encoding_sjis           = "sjis";
const string Generator::Encoding_prc            = "prc";
const string Generator::Encoding_big5           = "big5";
const string Generator::Encoding_wansung        = "wansung";
const string Generator::Encoding_johab          = "johab";
const string Generator::Encoding_adobe_latin_1  = "adobe_latin_1";
const string Generator::Encoding_adobe_standard = "adobe_standard";
const string Generator::Encoding_adobe_expert   = "adobe_expert";
const string Generator::Encoding_adobe_custom   = "adobe_custom";
const string Generator::Encoding_apple_roman    = "apple_roman";
const string Generator::Encoding_old_latin_2    = "old_latin_2";

Generator::Generator(GeneratorConfig& conf, bool verbose):
    mConf    ( conf    ),
    mVerbose ( verbose ),
    mPtrMain ( nullptr ),
    mPtrArray( nullptr ),
    mThreadDriver( nullptr )
{
    if ( mConf.numThreads() != 0 ) {

        mThreadDriver = new InternalGlyphThreadDriver( mConf.numThreads() );
    }
}


Generator::~Generator()
{
    releaseTexture();

    for ( auto* g : mGlyphs ) {

        delete g;
    }

    if ( mThreadDriver != nullptr ) {

        delete mThreadDriver;
    }
}


bool Generator::generate()
{
    if ( mVerbose ) {

        mConf.emitVerbose();
    }

    if ( !initializeFreeType() ) {

        return false;
    }

    if ( !generateGlyphs() ) {

        return false;
    }

    if ( mConf.extraGlyphPath() != "" ) {

       generateExtraGlyphs();
    }

    getKernings();

    const auto bestWidthForDefaultFontSize = fitGlyphsToTexture();

    if ( !generateGlyphBitmaps( bestWidthForDefaultFontSize ) ) {

        return false;
    }

    return true;
}


FT_Error Generator::setEncoding ( const string& s )
{
    if ( s.compare(Encoding_unicode) == 0 ) {

        return FT_Select_Charmap( mFtFace, FT_ENCODING_UNICODE );
    }
    else if (s.compare(Encoding_ms_symbol) == 0 ) {

        return FT_Select_Charmap( mFtFace, FT_ENCODING_MS_SYMBOL );
    }
    else if (s.compare(Encoding_sjis) == 0 ) {

        return FT_Select_Charmap( mFtFace, FT_ENCODING_SJIS );
    }
    else if (s.compare(Encoding_prc) == 0 ) {

        return FT_Select_Charmap( mFtFace, FT_ENCODING_PRC );
    }
    else if (s.compare(Encoding_big5) == 0 ) {

        return FT_Select_Charmap( mFtFace, FT_ENCODING_BIG5 );
    }
    else if (s.compare(Encoding_wansung) == 0 ) {

        return FT_Select_Charmap( mFtFace, FT_ENCODING_WANSUNG );
    }
    else if (s.compare(Encoding_johab) == 0 ) {

        return FT_Select_Charmap( mFtFace, FT_ENCODING_JOHAB );
    }
    else if (s.compare(Encoding_adobe_latin_1) == 0 ) {

        return FT_Select_Charmap( mFtFace, FT_ENCODING_ADOBE_LATIN_1 );
    }
    else if (s.compare(Encoding_adobe_standard) == 0 ) {

        return FT_Select_Charmap( mFtFace, FT_ENCODING_ADOBE_STANDARD );
    }
    else if (s.compare(Encoding_adobe_expert) == 0 ) {

        return FT_Select_Charmap( mFtFace, FT_ENCODING_ADOBE_EXPERT );
    }
    else if (s.compare(Encoding_adobe_custom) == 0 ) {

        return FT_Select_Charmap( mFtFace, FT_ENCODING_ADOBE_CUSTOM );
    }
    else if (s.compare(Encoding_apple_roman) == 0 ) {

        return FT_Select_Charmap( mFtFace, FT_ENCODING_ADOBE_CUSTOM );
    }
    else if (s.compare(Encoding_old_latin_2) == 0 ) {

        return FT_Select_Charmap( mFtFace, FT_ENCODING_OLD_LATIN_2 );
    }
    else {
        return FT_Err_Bad_Argument;
    }
}

bool Generator::initializeFreeType()
{
    auto ftError = FT_Init_FreeType( &mFtHandle );

    if ( ftError != FT_Err_Ok ) {

        cerr << "FreeType error: " << ftError << "\n";
        return false;
    }

    ftError = FT_New_Face(
                  mFtHandle,
                  mConf.fontPath().c_str(),
                  0,
                  &mFtFace 
              );

    if ( ftError != FT_Err_Ok ) {

        cerr << "Free Type error: " << ftError << "\n";
        return false;
    }

    ftError = setEncoding ( mConf.encoding() );

    if ( ftError != FT_Err_Ok ) {

        cerr << "FreeType error: " << ftError << "\n";
        return false;
    }

    ftError = FT_Set_Pixel_Sizes ( mFtFace, 0, mConf.glyphBitmapSizeForSampling() );

    if ( ftError != FT_Err_Ok ) {

        cerr << "FreeType error: " << ftError << "\n";
        return false;
    }

    if ( FT_HAS_GLYPH_NAMES( mFtFace ) ) {
        mConf.setFaceHasGlyphNames();
        cerr << "The face has glyph names.\n";
    }
    else {
        cerr << "The face does not have glyph names.\n";
    }

    cerr << "Num charmaps: " << mFtFace->num_charmaps << "\n";

    int active_charmap_index = -1;
    if ( mFtFace->charmap != nullptr ) {

        active_charmap_index = FT_Get_Charmap_Index( mFtFace->charmap );
    }

    for ( int i = 0; i < mFtFace->num_charmaps; i++ ) {

        cerr << "index: " << i;
        if ( i == active_charmap_index ) {

            cerr << " [ACTIVE] ";
        }
        else {
            cerr << "          ";
        }

        cerr << "encoding: " << FTUtilStringEncoding( mFtFace->charmaps[i]->encoding ) << "\t";
        cerr << "platform_id: " << mFtFace->charmaps[i]->platform_id << "\t";
        cerr << "encoding_id: " << mFtFace->charmaps[i]->encoding_id << "\t";
        cerr << "\n";
    }

    for ( int i = 0; i < mFtFace->num_charmaps; i++ ) {

        const auto charMap = generateCharMap( mFtFace, mFtFace->charmaps[i], i == active_charmap_index );
        mCharMaps.push_back( charMap );
    }

    if ( ! mConf.processHiddenGlyphs() ) {

        for ( const auto& charMap : mCharMaps ) {

            for ( const auto& pe : charMap.m_char_to_codepoint ) {

                mCodepointsToProcess.insert( pe.second );
            }
        }
    }

    return true;
}


void Generator::getKernings()
{
    if ( FT_HAS_KERNING( mFtFace ) ) {;

        for ( auto* g1 : mGlyphs ) {

            if ( g1->hasExternalBitmap() ) {
                continue;
            }

            for ( auto* g2 : mGlyphs ) {

                if ( g2->hasExternalBitmap() ) {
                    continue;
                }

                FT_Vector kerning;
                FT_Get_Kerning( mFtFace, g1->codePoint(), g2->codePoint(), FT_KERNING_DEFAULT, &kerning);

                if ( kerning.x != 0 ) {

                    g1->addKerning( g2->codePoint(), kerning.x );
                }
            }
        }
    }
}


long Generator::fitGlyphsToTexture()
{
    long maxNumGlyphsPerEdge = 0;
    long bestHeight = 0;
    const auto bestWidth = findBestWidthForDefaultFontSize( bestHeight, maxNumGlyphsPerEdge );

    if ( mVerbose ) {

        cerr << "Best width/height for the default font size is " << bestWidth << " / " << bestHeight << ".\n";
        cerr << "Max number of glyphs per edge is " << maxNumGlyphsPerEdge << ".\n";
    }

    // Consider a 1 pixel margin per glyph, to account for rounding errors
    auto factorX = (float)(mConf.outputTextureSize() - maxNumGlyphsPerEdge) / (float)bestWidth;
    auto factorY = (float)(mConf.outputTextureSize() - maxNumGlyphsPerEdge) / (float)bestHeight;
    auto factor = min(factorX, factorY);
    mConf.setGlyphScalingFromSamplingToPackedSignedDist( factor );

    if ( mVerbose ) {
        const auto s = mConf.glyphScalingFromSamplingToPackedSignedDist();
        cerr << "Scale is adjusted to " << s << ".\n";
        cerr << "Area [W: " << (long)((float)bestWidth * s) << " , H: " << (long)((float)bestHeight * s) << "]\n";
    }

    return bestWidth;
}

long Generator::findHeightFromWidth( const long width, long& maxNumGlyphsPerEdge )
{
    long leftX  = 0;
    long height = 0;
    long heightOfCurrentRow = 0;

    long maxNumGlyphsPerRow = 0;
    long numGlyphsPerRow = 0;
    long numGlyphsPerColumn = 1;

    for ( auto i = 0; i < mGlyphs.size(); i++ ) {

        auto* g = mGlyphs[ i ];

        if ( leftX + g->signedDistWidth() > width ) {

            height += heightOfCurrentRow;

            leftX  = g->signedDistWidth();
            heightOfCurrentRow = g->signedDistHeight();

            maxNumGlyphsPerRow = std::max( maxNumGlyphsPerRow, numGlyphsPerRow );
            numGlyphsPerRow = 1;

            numGlyphsPerColumn++;
        }
        else {
            leftX += g->signedDistWidth();
            heightOfCurrentRow = std::max( heightOfCurrentRow, g->signedDistHeight() );

            numGlyphsPerRow++;
        }
    }

    height += heightOfCurrentRow;

    maxNumGlyphsPerRow = std::max( maxNumGlyphsPerRow, numGlyphsPerRow );

    maxNumGlyphsPerEdge = std::max( maxNumGlyphsPerRow, numGlyphsPerColumn );

    return height;
}


long Generator::findBestWidthForDefaultFontSize( long& bestHeight, long& maxNumGlyphsPerEdge )
{
    const auto initialWidth = (long) sqrt ( mGlyphs.size() ) * mConf.glyphBitmapSizeForSampling();

    const auto initialHeight = findHeightFromWidth( initialWidth, maxNumGlyphsPerEdge );

    auto previousWidth { initialWidth  };
    auto previousHeight{ initialHeight };
    long previousMaxNumGlyphsPerEdge{ 0 };

    if ( initialWidth > initialHeight ) {

        for ( auto width = initialWidth - 1; width >= initialHeight; width-- ) {

            const auto height = findHeightFromWidth( width, maxNumGlyphsPerEdge );

            if ( width == height ) {

                bestHeight = height;
                return width;
            }
            else if ( width < height ) {

                if ( height - width < previousWidth - previousHeight ) {

                    bestHeight = height;
                    return width;
                }
                else {
                    maxNumGlyphsPerEdge = previousMaxNumGlyphsPerEdge;
                    bestHeight = previousHeight;
                    return previousWidth;
                }
            }
            else {
                previousWidth  = width;
                previousHeight = height;
                previousMaxNumGlyphsPerEdge = maxNumGlyphsPerEdge;
            }
        }
    }
    else {
        for ( auto width = initialWidth + 1; width <= initialHeight; width++ ) {

            const auto height = findHeightFromWidth( width, maxNumGlyphsPerEdge );

            if ( width == height ) {

                bestHeight = height;
                return width;
            }
            else if ( width > height ) {

                if ( width - height < previousHeight - previousWidth ) {

                    bestHeight = height;
                    return width;
                }
                else {
                    maxNumGlyphsPerEdge = previousMaxNumGlyphsPerEdge;
                    bestHeight = previousHeight;
                    return previousWidth;
                }
            }
            else {
                previousWidth  = width;
                previousHeight = height;
                previousMaxNumGlyphsPerEdge = maxNumGlyphsPerEdge;
            }
        }
    }

    return initialWidth;
}

bool Generator::generateGlyphs()
{
    char glyph_name_buffer[256];
    string glyph_name( "" );

    if ( mConf.processHiddenGlyphs() ) {

        for (FT_ULong i = 0; i <= mConf.maxCodePoint(); i++) {

            auto ftError = FT_Load_Glyph ( mFtFace, i, FT_LOAD_DEFAULT );

            if ( ftError != FT_Err_Ok ) {

                // no glyph present for the codepoint
                continue;
            }

            if ( mConf.faceHasGlyphNames() ) {

                ftError = FT_Get_Glyph_Name( mFtFace, i, glyph_name_buffer, 256 );

                if ( ftError != FT_Err_Ok ) {

                    cerr << "FreeType error: " << ftError << "\n";
                    return false;
                }

                glyph_name_buffer[255] = 0;
                glyph_name = glyph_name_buffer;
            }

            auto* g = new InternalGlyphForGen( mConf, mThreadDriver, i, mFtFace->glyph->metrics, glyph_name );
            mGlyphs.push_back ( g );
        }
    }
    else {
        for ( const FT_ULong i : mCodepointsToProcess ) {

            auto ftError = FT_Load_Glyph ( mFtFace, i, FT_LOAD_DEFAULT );

            if ( ftError != FT_Err_Ok ) {

                // no glyph present for the codepoint
                continue;
            }

            if ( mConf.faceHasGlyphNames() ) {

                ftError = FT_Get_Glyph_Name( mFtFace, i, glyph_name_buffer, 256 );

                if ( ftError != FT_Err_Ok ) {

                    cerr << "FreeType error: " << ftError << "\n";
                    return false;
                }

                glyph_name_buffer[255] = 0;
                glyph_name = glyph_name_buffer;
            }

            auto* g = new InternalGlyphForGen( mConf, mThreadDriver, i, mFtFace->glyph->metrics, glyph_name );
            mGlyphs.push_back ( g );
        }
    }

    return true;
}

CharMap Generator::generateCharMap(
    FT_Face        ftFace,
    FT_CharMapRec* ftCharMap,
    const bool     isDefault
) {
    CharMap charMap(
        isDefault,
        FTUtilStringEncoding( ftCharMap->encoding ),
        ftCharMap->platform_id,
        ftCharMap->encoding_id
    );

    auto ftError = FT_Set_Charmap( ftFace, ftCharMap );

    if ( ftError != FT_Err_Ok ) {

        cerr << "FreeType error: FT_Set_Charmap" << ftError << "\n";

        return charMap;
    }

    FT_UInt gindex = 0;

    FT_ULong charcode = FT_Get_First_Char( ftFace, &gindex );

    while ( gindex != 0 ) {

        if ( mConf.isInACharCodeRange( charcode ) ) {

            charMap.m_char_to_codepoint.insert( pair( charcode, gindex ) );
        }

        charcode = FT_Get_Next_Char( ftFace, charcode, &gindex );
    }
    return charMap;
}


std::pair<float, float> Generator::findMeanGlyphDimension()
{
    float width { 0.0f };
    float height{ 0.0f };
    float count { 0.0f };
    for ( auto* g : mGlyphs ) {
        if ( g->width() > 0.0f &&  g->height() > 0.0f ) {
            width  += g->width();
            height += g->height();
            count  += 1.0f;
        }
    }
    return std::pair( width / count, height / count );
}

void Generator::generateExtraGlyphs()
{
    const auto dim  = findMeanGlyphDimension();

    addExtraGlyph( 0x0A, "extra line feed", dim, GeneratorConfig::FileNameExtraGlyphLineFeed );

    addExtraGlyph( 0x00, "extra blank", dim, GeneratorConfig::FileNameExtraGlyphBlank          );
}

void Generator::addExtraGlyph( const long code_point, const string& glyph_name, const std::pair<float, float>& dim, const std::string& file_name )
{
    const auto base_path = std::filesystem::path{ mConf.extraGlyphPath() };
    const auto file_path = std::filesystem::path{ file_name };

    unsigned long width, height;
    unsigned char* data;

    const auto result = loadPngImage( base_path / file_path, width, height, &data );

    if ( mVerbose ) {
        cerr << "External bitmap: [" << file_name << "] ( " << width << ", " << height << ")\n";
    }

    auto* g = new InternalGlyphForGen (
        mConf,
        mThreadDriver,
        code_point,
        glyph_name,
        static_cast< long >( dim.first  ),
        static_cast< long >( dim.second ),
        data,
        width,
        height
    );

    mGlyphs.push_back( g );
}

bool Generator::generateGlyphBitmaps( long bestWidthForDefaultFontSize )
{
    long baseX    = 0;
    long baseY    = 0;
    long maxY     = 0;

    long numGlyphsProcessed = 1;

    for ( auto* g : mGlyphs ) {

        if ( g->hasExternalBitmap() ) {

            g->setSignedDist();
        }
        else {
            auto ftError = FT_Load_Glyph( mFtFace, g->codePoint(), FT_LOAD_DEFAULT );

            if (ftError != FT_Err_Ok) {

                cerr << "FreeType error: " << ftError << "\n";
                return false;
            }

            ftError = FT_Render_Glyph( mFtFace->glyph, FT_RENDER_MODE_MONO );

            if (ftError != FT_Err_Ok) {

                cerr << "FreeType error: " << ftError << "\n";
                return false;
            }

            auto& bm = mFtFace->glyph->bitmap;
            g->setSignedDist( bm );
        }

        if ( baseX + g->signedDistWidth() > mConf.outputTextureSize() ) {

            baseX = 0;
            baseY += maxY;
            maxY = 0;
        }

        g->setBaseXY(baseX, baseY);
        baseX += g->signedDistWidth();
        maxY = std::max( maxY, g->signedDistHeight() );

        if ( mVerbose ) {

            g->visualize(cerr);
            cerr << "Num Glyphs Processed: " << numGlyphsProcessed << "/" << mGlyphs.size() << "\n";
            cerr << "Base:[" << baseX << " , " << baseY << "]\n";
            cerr << "\n";
        }

        numGlyphsProcessed++;
    }

    return true;
}


bool Generator::generateTexture( bool reverseY )
{

    auto len = mConf.outputTextureSize();

    mPtrMain = (unsigned char*) malloc (sizeof(unsigned char) * 4 * len * len);

    if ( mPtrMain == nullptr ) {

        std::cerr << "Error\n";
        return false;
    }

    memset ( mPtrMain, (int)0, sizeof(unsigned char) * 4 * len * len );

    mPtrArray = (unsigned char**) malloc ( sizeof(unsigned char*) * len ) ;
               

    if ( mPtrArray == nullptr ) {

        free(mPtrMain);
        std::cerr << "Error\n";
        return false;
    }

    for (auto i = 0; i < len; i++ ) {

        mPtrArray[i] = &( mPtrMain[ sizeof(unsigned char) * len * i ] );
    }

    for ( auto* g : mGlyphs ) {

        for ( auto srcY = 0; srcY < g->signedDistHeight(); srcY++ ) {

            auto dstY    = len - 1 - ( srcY + g->baseY() );

            if ( dstY < 0 || len <= dstY ) {
                continue;
            }
            auto* curRow = mPtrArray [dstY];

            for ( auto srcX = 0; srcX < g->signedDistWidth(); srcX++ ) {

                auto dstX  = ( srcX + g->baseX() );
                if ( dstX < 0 || len <= dstX ) {
                    continue;
                }

                auto dist  = g->signedDist(
                    srcX, 
                    reverseY ? srcY : (g->signedDistHeight() - 1 - srcY)
                );

                auto alpha = min ( 255, max( 0, (int)( dist * 255.0 ) ) );
                curRow [ dstX ] = (unsigned char)alpha;
            }
        }
    }

    return true;
}


void Generator::releaseTexture()
{
    if ( mPtrMain != nullptr ) {

        free ( mPtrMain  );
        free ( mPtrArray );
        
        mPtrMain  = nullptr;
        mPtrArray = nullptr;
    }
}


unsigned char** Generator::textureBitmap()
{
    if ( !generateTexture( mConf.isReverseYDirectionForGlyphsSet() ) ) {

        std::cerr << "Error\n";

        return nullptr;
    }
    return mPtrArray;
}


bool Generator::emitFilePNG()
{
    if ( !generateTexture( mConf.isReverseYDirectionForGlyphsSet() ) ) {

        std::cerr << "Error\n";
        return false;
    }

    string outputFileNamePNG = mConf.outputFileName() + ".png";

    FILE*  pngOut = fopen( outputFileNamePNG.c_str(), "wb" );

    if ( pngOut == nullptr ) {

        releaseTexture();
        std::cerr << "Error\n";
        return false;
    }

    png_structp pngWritePtr = png_create_write_struct( PNG_LIBPNG_VER_STRING,
                                                       NULL,
                                                       NULL,
                                                       NULL
                                                     );
    if ( pngWritePtr == nullptr ) {

        fclose ( pngOut   );
        releaseTexture();
        std::cerr << "Error\n";
        return false;
    }

    png_infop pngInfoPtr = png_create_info_struct( pngWritePtr );

    if ( pngInfoPtr == nullptr ) {

        png_destroy_write_struct( &pngWritePtr, (png_infopp)NULL ) ;
        fclose ( pngOut   );
        releaseTexture();
        std::cerr << "Error\n";
        return false;
    }

    if ( setjmp( png_jmpbuf( pngWritePtr ) ) != 0 ) {

        png_destroy_write_struct( &pngWritePtr, &pngInfoPtr ) ;
        fclose ( pngOut   );
        releaseTexture();
        std::cerr << "Error\n";
        return false;
    }

    png_init_io( pngWritePtr, pngOut );

    if ( setjmp( png_jmpbuf( pngWritePtr ) ) != 0 ) {

        png_destroy_write_struct( &pngWritePtr, &pngInfoPtr ) ;
        fclose ( pngOut   );
        releaseTexture();
        std::cerr << "Error\n";
        return false;
    }

    png_set_IHDR( pngWritePtr,
                  pngInfoPtr,
                  mConf.outputTextureSize(),
                  mConf.outputTextureSize(),
                  8,
                  PNG_COLOR_TYPE_GRAY,
                  PNG_INTERLACE_NONE,
                  PNG_COMPRESSION_TYPE_BASE,
                  PNG_FILTER_TYPE_BASE
                );

    png_write_info( pngWritePtr, pngInfoPtr );

    if ( setjmp( png_jmpbuf( pngWritePtr ) ) != 0 ) {

        png_destroy_write_struct( &pngWritePtr, &pngInfoPtr ) ;
        fclose ( pngOut   );
        releaseTexture();
        std::cerr << "Error\n";
        return false;
    }

    png_write_image( pngWritePtr, mPtrArray );

    if ( setjmp( png_jmpbuf( pngWritePtr ) ) != 0 ) {

        png_destroy_write_struct( &pngWritePtr, &pngInfoPtr ) ;
        fclose ( pngOut   );
        releaseTexture();
        std::cerr << "Error\n";
        return false;
    }

    png_write_end( pngWritePtr, NULL );

    png_destroy_write_struct( &pngWritePtr, &pngInfoPtr ) ;

    fclose ( pngOut   );
    releaseTexture();

    return true;
}


bool Generator::emitFileMetrics()
{
    ofstream osMetrics(mConf.outputFileName() + ".txt");

    if ( !osMetrics ) {

        std::cerr << "Error\n";
        return false;
    }

    mConf.outputMetricsHeader( osMetrics );

    osMetrics << "SPREAD IN TEXTURE\n";
    osMetrics << (float)mConf.signedDistExtent() / (float) mConf.outputTextureSize();
    osMetrics << "\n";
    osMetrics << "SPREAD IN FONT METRICS\n";
    osMetrics << (float)mConf.signedDistExtent() / mConf.glyphScalingFromSamplingToPackedSignedDist() / (float) mConf.glyphBitmapSizeForSampling();
    osMetrics << "\n";
    osMetrics << "GLYPHS\n";

    for ( auto* g : mGlyphs ) {

        g->emitMetrics( osMetrics );
        osMetrics << "\n";

    }

    osMetrics << "#Kernings\tPred Code Point\tSucc Code Point 1"
                 "\tKerning1\tSucc Code Point 2\tKerning 2...\n";

    osMetrics << "KERNINGS\n";

    for ( auto* g : mGlyphs ) {

        g->emitKernings( osMetrics );

    }

    osMetrics << "#Char Maps\tEncoding\tPlatform ID\tEncoding ID\tDefault?\tNum Chars\t";
    osMetrics << "#Char Code 1\tGlyph Code Point 1\tChar Code 2\tGlyph Code Point 2...\n";
    osMetrics << "CHAR MAPS\n";

    for ( auto& char_map : mCharMaps ) {

        char_map.emit( osMetrics );
    }

    osMetrics.close();

    if ( mVerbose ) {

        cerr << "Output Metrics written to ["
             << mConf.outputFileName() << ".txt]\n";
    }

    return true;
}


void Generator::generateMetrics(float& margin, vector<Glyph>& glyphs)
{

    glyphs.clear();

    margin =  (float)mConf.signedDistExtent() / (float) mConf.outputTextureSize();

    for ( auto* g : mGlyphs ) {

        auto sdg = g->generateSDGlyph();
        glyphs.push_back( std::move(sdg) );
    }

}

} // namespace SDFont

