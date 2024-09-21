#ifndef __SDFONT_UTIL_HPP__
#define __SDFONT_UTIL_HPP__

#include <string>

using namespace std;

namespace SDFont {

template< typename T >
static string toHexString( T val )
{
    static const char* digits = "0123456789ABCDEF";

    static const size_t num_digits = sizeof( T ) * 2; // num_bytes * 2

    string formatted_string( num_digits, '0' );

    int shift_amount = ( num_digits - 1 ) * 4 ;

    for (size_t i = 0; i < num_digits; i++ ) {

        formatted_string[ i ] = digits[ ( val >> shift_amount ) & 0x0f ];

        shift_amount -= 4;
    }

    return formatted_string;
}

} // namespace SDFont

#endif /*__SDFONT_UTIL_HPP__*/
