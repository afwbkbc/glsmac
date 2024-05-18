#pragma once

#include <stdexcept>

#define THROW( _text ) throw std::runtime_error( _text )

#ifdef DEBUG

#define ASSERT( _condition, _text ) \
    if ( !( _condition ) ) { \
        Log( (std::string) "FATAL: " + _text ); \
        THROW( _text ); \
    }

// for rare cases when Log() is not available
#define ASSERT_NOLOG( _condition, _text ) \
    if ( !( _condition ) ) { \
        THROW( _text ); \
    }

#else
#define ASSERT( _condition, _text )
#define ASSERT_NOLOG( _condition, _text )
#endif
