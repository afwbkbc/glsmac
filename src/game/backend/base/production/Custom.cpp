#include "Custom.h"

namespace game {
namespace backend {
namespace base {
namespace production {

Custom::Custom( const RenderInfo& render_info )
	: Production( PT_CUSTOM )
	, m_render_info( render_info ) {}

void Custom::SerializeImpl( types::Buffer& buf ) const {
	m_render_info.Write( buf );
}

const Production* const Custom::DeserializeImpl( types::Buffer& buf ) {
	return new Custom( RenderInfo::FromBuffer( buf ) );
}

}
}
}
}