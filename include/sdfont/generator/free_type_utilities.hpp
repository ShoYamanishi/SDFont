#ifndef __SDFONT_FREE_TYPE_UTILITIES_HPP__
#define __SDFONT_FREE_TYPE_UTILITIES_HPP__

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <math.h>
#include <ft2build.h>

#include FT_FREETYPE_H

using namespace std;

/** @file free_type_utilities.hpp
 */
void FTUtilPrintFaceFlags ( FT_Long f, ostream& os );

void FTUtilPrintStyleFlags ( FT_Long f, ostream& os);

void FTUtilPrintPixelMode( FT_Pixel_Mode m, ostream& os );

void FTUtilPrintPixels ( FT_Bitmap& bm ,ostream& os ) ;

void FTUtilPrintGlyphMetrics( FT_Glyph_Metrics& m, ostream& os );

#endif /*__SDFONT_FREE_TYPE_UTILITIES_HPP__*/
