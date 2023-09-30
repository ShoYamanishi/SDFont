#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <png.h>

#include "sdfont_generator.hpp"

namespace SDFont {


Generator::Generator(GeneratorConfig& conf, bool verbose):
    mConf    ( conf    ),
    mVerbose ( verbose ),
    mPtrMain ( nullptr ),
    mPtrArray( nullptr )
    {;}


Generator::~Generator()
{
    releaseTexture();
}


bool Generator::generate()
{
    if ( mVerbose ) {

        mConf.emitVerbose();
    }

    setlocale ( LC_ALL, mConf.locale().c_str() );

    if ( !initializeFreeType() ) {

        return false;
    }


    if ( !generateGlyphs() ) {

        return false;
    }

    getKernings();

    auto numItemsPerRow = fitGlyphsToTexture();

    if ( !generateGlyphBitmaps( numItemsPerRow ) ) {

        return false;
    }

    return true;
}


bool Generator::initializeFreeType()
{
    auto ftError = FT_Init_FreeType( &mFtHandle );

    if ( ftError != FT_Err_Ok ) {

        cerr << "error: " << ftError << "\n";
        return false;
    }

    ftError = FT_New_Face(
                  mFtHandle,
                  mConf.fontPath().c_str(),
                  0,
                  &mFtFace 
              );

    if ( ftError != FT_Err_Ok ) {

        cerr << "error: " << ftError << "\n";
        return false;
    }

    ftError = FT_Set_Pixel_Sizes ( mFtFace, 0, mConf.resolution() );

    if ( ftError != FT_Err_Ok ) {

        cerr << "error: " << ftError << "\n";
        return false;
    }

    return true;
}


void Generator::getKernings()
{
    if ( FT_HAS_KERNING( mFtFace ) ) {;

        for ( auto& g1 : mGlyphs ) {

            auto ci1 = FT_Get_Char_Index( mFtFace, g1.codePoint() );

            for ( auto& g2 : mGlyphs ) {

                auto ci2 = FT_Get_Char_Index( mFtFace, g2.codePoint() );

                FT_Vector kerning;
                FT_Get_Kerning( mFtFace, ci1, ci2, FT_KERNING_DEFAULT, &kerning);

                if ( kerning.x != 0 ) {

                    g1.addKerning( g2.codePoint(), kerning.x );
                }
            }
        }
    }
}


long Generator::fitGlyphsToTexture()
{
    long X = 0, Y = 0 ;

    long numItemsPerRow = bestNumberOfItemsPerRow ( X, Y );

    if ( mVerbose ) {

        cerr << "Best Number of glyphs per row is " << numItemsPerRow << ".\n";
        cerr << "Area [X: " << X << " , Y: " << Y << "]\n";
    }

    auto factor = max( (float)X, (float)Y ) / (float)mConf.textureSize() ;

    mConf.setScale( factor );

    findDimension ( numItemsPerRow, X, Y );

    if ( mVerbose ) {

        cerr << "Scale is adjusted to " << mConf.scale() << ".\n";
        cerr << "Area [X: " << X/mConf.scale() << " , Y: " << Y/mConf.scale() << "]\n";
    }

    return numItemsPerRow;
}


long Generator::bestNumberOfItemsPerRow ( long& X, long& Y)
{

    long initialNumPerRow = (long) sqrt ( mGlyphs.size() );

    long curX, curY;

    float initialRatio = findDimension ( initialNumPerRow, curX, curY );

    long  bestNumPerRow = initialNumPerRow ;
    float bestRatio     = initialRatio;
    long  bestX         = curX;
    long  bestY         = curY;

    if (curX < curY) {

        // Intial area is Portrait. Try increasing the number.                  
        for ( auto curNumPerRow = initialNumPerRow + 1 ;
                   curNumPerRow < mGlyphs.size()       ;
                   curNumPerRow++                        ) {

            auto curRatio = findDimension( curNumPerRow, curX, curY );

            if ( curRatio >= bestRatio ) {
                break;
            }

            bestNumPerRow = curNumPerRow;
            bestRatio     = curRatio;
            bestX         = curX;
            bestY         = curY;
        }
    }
    else {
        // Intial area is Landscape. Try decreasing the number.                 

        for ( auto curNumPerRow = initialNumPerRow - 1 ;
                   curNumPerRow > 0                    ;
                   curNumPerRow--                        ) {

            auto curRatio = findDimension ( curNumPerRow, curX, curY );

            if ( curRatio >= bestRatio ) {
                break;
            }

            bestNumPerRow = curNumPerRow;
            bestRatio     = curRatio;
            bestX         = curX;
            bestY         = curY;
        }

    }

    X = bestX;
    Y = bestY;

    return bestNumPerRow;
}


float Generator::findDimension ( long itemsPerRow, long& X, long& Y )
{

    long totalX      = 0;
    long totalY      = 0;

    for ( auto i = 0; i < ( mGlyphs.size() + itemsPerRow - 1 ) / itemsPerRow ; i++ ) {

        long totalXrow = 0;
        long maxY      = 0;

        for ( auto j = 0; j < itemsPerRow; j++ ) {

            auto  k = i * itemsPerRow + j;

            if ( k < mGlyphs.size() ){

                auto& g = mGlyphs[k];

                totalXrow += g.signedDistWidth();

                maxY = max( maxY, g.signedDistHeight() );
            }
        }

        totalX = max( totalX, totalXrow );
        totalY += maxY;

    }

    X = totalX;
    Y = totalY;

    return ( totalX > totalY ) ? ( (float)totalX / (float)totalY ) :
                                 ( (float)totalY / (float)totalX ) ;
}


bool Generator::generateGlyphs()
{
    for (FT_ULong i = 0; i <= mConf.maxCodePoint(); i++) {

        if ( iswprint(i) ) {

            auto ind = FT_Get_Char_Index ( mFtFace, i );

            auto ftError = FT_Load_Glyph ( mFtFace, ind, FT_LOAD_DEFAULT );

            if ( ftError != FT_Err_Ok ) {
                cerr << "error: " << ftError << "\n";
                return false;
            }

            mGlyphs.emplace_back ( mConf, i, mFtFace->glyph->metrics );
        }
    }

    return true;
}


bool Generator::generateGlyphBitmaps( long numItemsPerRow )
{

    long baseX    = 0;
    long baseY    = 0;
    long glyphNum = 0;
    long maxY     = 0;

    long numGlyphsProcessed = 1;

    for ( auto& g : mGlyphs ) {

        auto ind = FT_Get_Char_Index( mFtFace, g.codePoint() );

        auto ftError = FT_Load_Glyph( mFtFace, ind, FT_LOAD_DEFAULT );

        if (ftError != FT_Err_Ok) {

            cerr << "error: " << ftError << "\n";
            return false;
        }

        ftError = FT_Render_Glyph( mFtFace->glyph, FT_RENDER_MODE_MONO );

        if (ftError != FT_Err_Ok) {

            cerr << "error: " << ftError << "\n";
            return false;
        }

        auto& bm = mFtFace->glyph->bitmap;
        g.setSignedDist( bm );
        g.setBaseXY(baseX, baseY);

        if ( mVerbose ) {

            g.visualize(cerr);
            cerr << "Num Glyphs Processed: " << numGlyphsProcessed << "/" << mGlyphs.size() << "\n";
            cerr << "Base:[" << baseX << " , " << baseY << "]\n";
            cerr << "\n";
        }

        maxY = max( maxY, g.signedDistHeight() );

        glyphNum++;

        if ( (glyphNum % numItemsPerRow) == 0 ) {

            baseX = 0;
            baseY += maxY;
            maxY  = 0;

        }
        else {
            baseX += g.signedDistWidth();
        }
        numGlyphsProcessed++;
    }

    return true;
}


bool Generator::generateTexture( bool reverseY )
{

    auto len = mConf.textureSize();

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

    for ( auto& g : mGlyphs ) {

        for ( auto srcY = 0; srcY < g.signedDistHeight(); srcY++ ) {

            auto dstY    = reverseY ? ( (len - 1) - ( srcY + g.baseY() ) ) :
                                      ( srcY + g.baseY()                 ) ;
            if ( dstY < 0 || len <= dstY ) {
                continue;
            }
            auto* curRow = mPtrArray [dstY];

            for ( auto srcX = 0; srcX < g.signedDistWidth(); srcX++ ) {

                auto dstX  = ( srcX + g.baseX() );
                if ( dstX < 0 || len <= dstX ) {
                    continue;
                }
                auto dist  = g.signedDist( srcX, srcY );

                auto alpha = min ( 255, max( 0, (int)( dist * 255.0 ) ) );

                curRow [ dstX ] = (unsigned char) alpha;
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
    if ( !generateTexture( true ) ) {

        std::cerr << "Error\n";

        return nullptr;
    }
    return mPtrArray;
}


bool Generator::emitFilePNG()
{
    if ( !generateTexture( true ) ) {

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
                  mConf.textureSize(),
                  mConf.textureSize(),
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
    osMetrics << (float)mConf.signedDistExtent() / (float) mConf.textureSize();
    osMetrics << "\n";
    osMetrics << "SPREAD IN FONT METRICS\n";
    osMetrics << (float)mConf.signedDistExtent() * mConf.fscale() / (float) mConf.resolution();
    osMetrics << "\n";
    osMetrics << "GLYPHS\n";

    for ( auto& g : mGlyphs ) {

        g.emitMetrics( osMetrics );
        osMetrics << "\n";

    }

    osMetrics << "#Kernings\tPred Code Point\tSucc Code Point 1"
                 "\tKerning1\tSucc Code Point 2\tKerning 2...\n";

    osMetrics << "KERNINGS\n";

    for ( auto& g : mGlyphs ) {

        g.emitKernings( osMetrics );

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

    margin =  (float)mConf.signedDistExtent() / (float) mConf.textureSize();

    for ( auto& g : mGlyphs ) {

        auto sdg = g.generateSDGlyph();
        glyphs.push_back( std::move(sdg) );
    }

}


} // namespace SDFont

