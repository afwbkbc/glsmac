#pragma once

#include <stdexcept>

#define THROW( _text ) throw std::runtime_error( _text )

#if defined( DEBUG ) || defined( FASTDEBUG )

#define ASSERT( _condition, _text ) \
    if ( !( _condition ) ) { \
        THROW( _text ); \
    }

#else
#define ASSERT( _condition, _text )
#endif
