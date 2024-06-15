#pragma once

#include <cstdlib>
#include <atomic>

namespace common {
typedef size_t mt_id_t;

// used to check if request processing should continue or stop
// and other similar cases
typedef std::atomic< bool > mt_flag_t;

// pass 'canceled' parameter
#define MT_C canceled

// add 'canceled' parameter
#define MT_CANCELABLE const common::mt_flag_t& MT_C

// 'return if canceled'
#define MT_RETIF() \
    if ( MT_C ) { \
        return; \
    }

// 'return something if canceled'
#define MT_RETIFV( _val ) \
    if ( MT_C ) { \
        return _val; \
    }

}
