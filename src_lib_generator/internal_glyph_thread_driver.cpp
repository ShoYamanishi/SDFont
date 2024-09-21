#include "sdfont/generator/internal_glyph_thread_driver.hpp"
#include "sdfont/generator/internal_glyph_for_generator.hpp"

namespace SDFont {

InternalGlyphThreadDriver::InternalGlyphThreadDriver( const int32_t num_threads )
    :m_fan_out    ( num_threads )
    ,m_fan_in     ( num_threads )
    ,m_num_threads( num_threads )
{
    auto thread_lambda = [ this ]( const size_t thread_index ) {

        while ( true ) {

            m_fan_out.wait( thread_index );

            if( m_fan_out.isTerminating() ) {
                break;
            }

            for ( long i = thread_index ; i < m_glyph->mSignedDistHeight; i += this->m_num_threads ) {

                for ( long j = 0 ; j < m_glyph->mSignedDistWidth; j++ ) {

                    auto val = m_glyph->getSignedDistance(
                        *m_bm,
                        m_scale,
                        m_spreadInBitmapPixels,
                        j - m_offset,
                        i - m_offset
                                        );
                    m_glyph->mSignedDist[ i * m_glyph->mSignedDistWidth + j] = val;
                }
            }

            m_fan_in.notify();
            if( m_fan_in.isTerminating() ) {

                break;
            }
        }
    };

    for ( size_t i = 0; i < m_num_threads; i++ ) {

        m_threads.emplace_back( thread_lambda, i );
    }
}

InternalGlyphThreadDriver::~InternalGlyphThreadDriver()
{
    m_fan_out.terminate();
    m_fan_in.terminate();

    for ( auto& t : m_threads ) {

        t.join();
    }
}

void InternalGlyphThreadDriver::run(
    InternalGlyphForGen* glyph,
    FT_Bitmap*           bm,
    float                scale,
    long                 spreadInBitmapPixels,
    long                 offset
) {
    m_glyph                = glyph;
    m_bm                   = bm;
    m_scale                = scale;
    m_spreadInBitmapPixels = spreadInBitmapPixels;
    m_offset               = offset;

    m_fan_out.notify();

    m_fan_in.wait();

    m_glyph                = nullptr;
    m_bm                   = nullptr;
    m_scale                = 0.0f;
    m_spreadInBitmapPixels = 0;
    m_offset               = 0;
}

} // namespace SDFont
