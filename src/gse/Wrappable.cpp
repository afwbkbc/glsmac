#include "Wrappable.h"

#include "gse/type/Object.h"

namespace gse {

Wrappable::~Wrappable() {
	for ( const auto& wrapobj : m_wrapobjs ) {
		wrapobj->Unlink();
	}
}

void Wrappable::Link( type::Object* wrapobj ) {
	ASSERT_NOLOG( m_wrapobjs.find( wrapobj ) == m_wrapobjs.end(), "wrapobj already linked" );
	m_wrapobjs.insert( wrapobj );
}

void Wrappable::Unlink( type::Object* wrapobj ) {
	ASSERT_NOLOG( m_wrapobjs.find( wrapobj ) != m_wrapobjs.end(), "wrapobj not linked" );
	m_wrapobjs.erase( wrapobj );
}

}
