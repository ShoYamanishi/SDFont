#ifndef __SDFONT_INTERNAL_GLYPH_THREAD_DRIVER_HPP__
#define __SDFONT_INTERNAL_GLYPH_THREAD_DRIVER_HPP__

#include <cstdint>
#include <vector>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "sdfont/generator/thread_synchronizer.hpp"


using namespace std;

namespace SDFont {

class InternalGlyphForGen;

class InternalGlyphThreadDriver {
public:

    InternalGlyphThreadDriver( const int32_t num_threads );

    ~InternalGlyphThreadDriver();

    void run(
        InternalGlyphForGen* glyph,
        FT_Bitmap*           bm,
        float                scale,
        long                 spreadInBitmapPixels,
        long                 offset
    );


    InternalGlyphThreadDriver( InternalGlyphThreadDriver const& ) = delete;
    void operator = ( InternalGlyphThreadDriver const& ) = delete;

    WaitNotifyMultipleWaiters   m_fan_out;
    WaitNotifyMultipleNotifiers m_fan_in;
    const int32_t               m_num_threads;
    std::vector< std::thread >  m_threads;

    InternalGlyphForGen*        m_glyph;
    FT_Bitmap*                  m_bm;
    float                       m_scale;
    long                        m_spreadInBitmapPixels;
    long                        m_offset;
};

} // namespace SDFont

#endif /*__SDFONT_INTERNAL_GLYPH_THREAD_DRIVER_HPP__*/

