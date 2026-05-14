#include "Ptr.h"

#include "gse/value/Float.h"
#include "gse/value/Int.h"

namespace gse {
namespace value {

AnyPtr::AnyPtr( gc::Space* const gc_space, const value_type_t type )
	: Value( gc_space, GetType() )
	, ptr_type( type ) {}

template< typename T > Ptr< T >::Ptr( gc::Space* const gc_space, std::atomic< T >& target )
	: AnyPtr( gc_space, GetPtrType() )
	, m_target( target ) {}

// cloning

#define CLONEIMPL( _type, _gse_type ) \
template<> Value* const Ptr< _type >::Clone() { \
    return VALUEEXT( _gse_type, m_gc_space, m_target ); \
}
CLONEIMPL( float, Float );
CLONEIMPL( int64_t, Int );
#undef CLONEIMPL

// operators

#define DEFAULT_COMPARE_OP( _type, _gse_vt, _gse_type, _op ) \
template<> const bool Ptr< _type >::operator _op ( const Value& other ) const { \
    switch ( other.type ) { \
        case _gse_vt: \
            return m_target _op ( (_gse_type*)&other )->value; \
        case VT_PTR: { \
            auto* v = ( (AnyPtr*)&other ); \
            switch ( v->ptr_type ) { \
                case _gse_vt: \
                    return m_target _op ( (Ptr< _type >*)v )->m_target; \
                default: \
                    return false; \
            } \
        } \
        default: \
            return false; \
    } \
}
#define DEFAULT_COMPARE( _type, _gse_vt, _gse_type ) \
DEFAULT_COMPARE_OP( _type, _gse_vt, _gse_type, == ) \
DEFAULT_COMPARE_OP( _type, _gse_vt, _gse_type, < ) \
DEFAULT_COMPARE_OP( _type, _gse_vt, _gse_type, > ) \
DEFAULT_COMPARE_OP( _type, _gse_vt, _gse_type, <= ) \
DEFAULT_COMPARE_OP( _type, _gse_vt, _gse_type, >= )
DEFAULT_COMPARE( float, VT_FLOAT, Float );
DEFAULT_COMPARE( int64_t, VT_INT, Int );
#undef DEFAULT_COMPARE
#undef DEFAULT_COMPARE_OP

// ptr type
#define PTRTYPEIMPL( _type, _gse_vt ) \
template<> const value_type_t Ptr< _type >::GetPtrType() const { \
    return _gse_vt; \
}
PTRTYPEIMPL( float, VT_FLOAT );
PTRTYPEIMPL( int64_t, VT_INT );
#undef PTRTYPEIMPL

template class Ptr< float >;
template class Ptr< int64_t >;

}
}
