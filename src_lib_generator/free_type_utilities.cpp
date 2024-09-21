#include "sdfont/free_type_utilities.hpp"

using namespace std;

/** @file free_type_utilities.cpp
 */


void FTUtilPrintFaceFlags ( FT_Long f, ostream& os ) {

    if ( ( f & FT_FACE_FLAG_SCALABLE )         != 0 ) {
        os << "FT_FACE_FLAG_SCALABLE\n";

    }
    if ( ( f & FT_FACE_FLAG_FIXED_SIZES )      != 0 ) {
        os << "FT_FACE_FLAG_FIXED_SIZES\n";

    }
    if ( ( f & FT_FACE_FLAG_FIXED_WIDTH )      != 0 ) {
        os << "FT_FACE_FLAG_FIXED_WIDTH\n";

    }
    if ( ( f & FT_FACE_FLAG_SFNT )             != 0 ) {
        os << "FT_FACE_FLAG_SFNT\n";

    }
    if ( ( f & FT_FACE_FLAG_HORIZONTAL )       != 0 ) {
        os << "FT_FACE_FLAG_HORIZONTAL\n";

    }
    if ( ( f & FT_FACE_FLAG_VERTICAL )         != 0 ) {
        os << "FT_FACE_FLAG_VERTICAL\n";

    }
    if ( ( f & FT_FACE_FLAG_KERNING )          != 0 ) {
        os << "FT_FACE_FLAG_KERNING\n";

    }
    if ( ( f & FT_FACE_FLAG_FAST_GLYPHS )      != 0 ) {
        os << "FT_FACE_FLAG_FAST_GLYPHS\n";

    }
    if ( ( f & FT_FACE_FLAG_MULTIPLE_MASTERS ) != 0 ) {
        os << "FT_FACE_FLAG_MULTIPLE_MASTERS\n";

    }
    if ( ( f & FT_FACE_FLAG_GLYPH_NAMES )      != 0 ) {
        os << "FT_FACE_FLAG_GLYPH_NAMES\n";

    }
    if ( ( f & FT_FACE_FLAG_EXTERNAL_STREAM )  != 0 ) {
        os << "FT_FACE_FLAG_EXTERNAL_STREAM\n";

    }
    if ( ( f & FT_FACE_FLAG_HINTER )           != 0 ) {
        os << "FT_FACE_FLAG_HINTER\n";

    }
    if ( ( f & FT_FACE_FLAG_CID_KEYED )        != 0 ) {
        os << "FT_FACE_FLAG_CID_KEYED\n";

    }
    if ( ( f & FT_FACE_FLAG_TRICKY )           != 0 ) {
        os << "FT_FACE_FLAG_TRICKY\n";

    }
    if ( ( f & FT_FACE_FLAG_COLOR )            != 0 ) {
        os << "FT_FACE_FLAG_COLOR\n";

    }
    if ( ( f & FT_FACE_FLAG_VARIATION )        != 0 ) {
        os << "FT_FACE_FLAG_VARIATION\n";

    }
}


void FTUtilPrintStyleFlags ( FT_Long f, ostream& os) {

    if ( ( f & FT_STYLE_FLAG_ITALIC ) != 0 ) {

        os << "FT_STYLE_FLAG_ITALIC\n";
    }
    if ( ( f & FT_STYLE_FLAG_BOLD )   != 0 ) {

        os << "FT_STYLE_FLAG_BOLD\n";
    }
}


void FTUtilPrintPixelMode( FT_Pixel_Mode m, ostream& os ) {

    switch (m) {

      case FT_PIXEL_MODE_NONE:

        os << "FT_PIXEL_MODE_NONE\n";
        break;

      case FT_PIXEL_MODE_MONO:

        os << "FT_PIXEL_MODE_MONO\n";
        break;

      case FT_PIXEL_MODE_GRAY:

        os << "FT_PIXEL_MODE_GRAY\n";
        break;

      case FT_PIXEL_MODE_GRAY2:

        os << "FT_PIXEL_MODE_GRAY2\n";
        break;

      case FT_PIXEL_MODE_GRAY4:

        os << "FT_PIXEL_MODE_GRAY4\n";
        break;

      case FT_PIXEL_MODE_LCD:

        os << "FT_PIXEL_MODE_LCD\n";
        break;

      case FT_PIXEL_MODE_LCD_V:

        os << "FT_PIXEL_MODE_LCD_V\n";
        break;

      case FT_PIXEL_MODE_BGRA:

        os << "FT_PIXEL_MODE_BGRA\n";
        break;

      default:
        break;

    }
}


static bool isPixelSet( FT_Bitmap& bm, long x, long y ) {

    if ( x < 0 || y < 0 || x >= bm.width || y >= bm.rows ) {

        return false;
    }

    FT_Byte* p            = bm.buffer;
    FT_Byte* row          = p + ( bm.pitch * y );

    int      byteOffset   = x / 8;
    int      bitOffset    = 7 - ( x % 8 );

    FT_Byte  b            = *( row + byteOffset ) ;

    FT_Byte  bitPosTested = (FT_Byte)( 1L << bitOffset );

    return ( b & bitPosTested ) != 0;

}


void FTUtilPrintPixels ( FT_Bitmap& bm ,ostream& os ) {

    for ( auto i = 0 ; i < bm.rows ; i++ ) {

        for ( auto j = 0 ; j < bm.width ; j++ ) {

            if ( isPixelSet( bm, j, i ) ) {

                os << "*";
            }
            else {

                os << ".";
            }
        }

        os << "\n";
    }
}

string FTUtilStringEncoding( FT_Encoding enc )
{
    switch( enc ) {

    case FT_ENCODING_NONE:
        return "FT_ENCODING_NONE";

    case FT_ENCODING_UNICODE:
        return "FT_ENCODING_UNICODE";

    case FT_ENCODING_MS_SYMBOL:
        return "FT_ENCODING_MS_SYMBOL";

    case FT_ENCODING_ADOBE_LATIN_1:
        return "FT_ENCODING_ADOBE_LATIN_1";

    case FT_ENCODING_OLD_LATIN_2:
        return "FT_ENCODING_OLD_LATIN_2";

    case FT_ENCODING_SJIS:
        return "FT_ENCODING_SJIS";

    case FT_ENCODING_PRC:
        return "FT_ENCODING_PRC";

    case FT_ENCODING_BIG5:
        return "FT_ENCODING_BIG5";

    case FT_ENCODING_WANSUNG:
        return "FT_ENCODING_WANSUNG";

    case FT_ENCODING_JOHAB:
        return "FT_ENCODING_JOHAB";

    case FT_ENCODING_ADOBE_STANDARD:
        return "FT_ENCODING_ADOBE_STANDARD";

    case FT_ENCODING_ADOBE_EXPERT:
        return "FT_ENCODING_ADOBE_EXPERT";

    case FT_ENCODING_ADOBE_CUSTOM:
        return "FT_ENCODING_ADOBE_CUSTOM";

    case FT_ENCODING_APPLE_ROMAN:
        return "FT_ENCODING_APPLE_ROMAN";

    default:
        return "<unknown>";
    }
}


void FTUtilPrintGlyphMetrics( FT_Glyph_Metrics& m, ostream& os ) {

    os << "width:        " << (float)(m.width)        / 64.0 << "\n";
    os << "height:       " << (float)(m.height)       / 64.0 << "\n";
    os << "horiBearingX: " << (float)(m.horiBearingX) / 64.0 << "\n";
    os << "horiBearingY: " << (float)(m.horiBearingY) / 64.0 << "\n";
    os << "horiAdvance:  " << (float)(m.horiAdvance)  / 64.0 << "\n";
    os << "vertBearingX: " << (float)(m.vertBearingX) / 64.0 << "\n";
    os << "vertBearingY: " << (float)(m.vertBearingY) / 64.0 << "\n";
    os << "vertAdvance:  " << (float)(m.vertAdvance)  / 64.0 << "\n";   

}
