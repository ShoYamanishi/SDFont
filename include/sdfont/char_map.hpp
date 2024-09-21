#ifndef __SDFONT_CHAR_MAP_HPP__
#define __SDFONT_CHAR_MAP_HPP__

#include <cstdint>
#include <map>

#include "sdfont/util.hpp"

using namespace std;

namespace SDFont {

/** @file char_map.hpp
 *
 *  @brief mapping from a character code to a glyph code point
 */
class CharMap {

  public:

    CharMap(
        const bool     is_default,
        const string&  encoding,
        const uint32_t platform_id,
        const uint32_t encoding_id
    )
        :m_default     ( is_default )
        ,m_encoding    ( encoding )
        ,m_platform_id ( platform_id )
        ,m_encoding_id ( encoding_id )
    {
    }

    void emit( ostream& os )
    {
        os << m_encoding;
        os << "\t";
        os << m_platform_id;
        os << "\t";
        os << m_encoding_id;
        os << "\t";
        if ( m_default ) {
            os << "default";
        }
        else {
            os << "not default";
        }
        os << "\t";
        os << m_char_to_codepoint.size();

        for ( const auto& pe : m_char_to_codepoint ) {
            os << "\t";
            os << "0X" << toHexString( pe.first );
            os << "\t";
            os << "0X" << toHexString( pe.second );
        }
        os << "\n";
    }

    uint32_t getCodepoint( const uint32_t char_code ) const {

        const auto it = m_char_to_codepoint.find( char_code );
        return it->second;
    }

    const bool    m_default;
    const string  m_encoding;
    const int32_t m_platform_id;
    const int32_t m_encoding_id;

    map< uint32_t, uint32_t > m_char_to_codepoint;
};

} // namespace SDFont

#endif /*__SDFONT_CHAR_MAP_HPP__*/
