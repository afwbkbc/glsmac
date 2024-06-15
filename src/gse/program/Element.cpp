#include "Element.h"

#include "common/Assert.h"

namespace gse {
namespace program {

Element::Element( const si_t& si, const element_type_t element_type )
	: m_si( si )
	, m_element_type( element_type ) {}

const std::string Element::ToString() const {
	THROW( "ToString() not implemented for element: " + Dump() );
}

}
}
