#include "Custom.h"

namespace game {
namespace backend {
namespace base {
namespace production {

Custom::Custom( const RenderInfo& render_info, const std::string& name, const std::string& hint )
	: Production( PT_CUSTOM, hint )
	, m_name( name )
	, m_render_info( render_info ) {}

gse::value::Object* const Custom::ToValue( GSE_CALLABLE, const std::string& id ) const {
	return VALUEEXT( gse::value::Object, GSE_CALL, {
		{ "id", VALUE( gse::value::String, , id ) },
		{ "name", VALUE( gse::value::String, , m_name ) },
		{ "texture", VALUE( gse::value::String, , m_render_info.ToTextureString() ) },
		{ "hint", VALUE( gse::value::String, , m_hint ) },
	} );
}

const uint8_t Custom::GetGroupOrder() const {
	return 10; // TODO: get from scripts
}

void Custom::SerializeImpl( types::Buffer& buf ) const {
	m_render_info.Write( buf );
	buf.WriteString( m_name );
}

const Production* const Custom::DeserializeImpl( types::Buffer& buf, const std::string& hint ) {
	const auto render_info = RenderInfo::FromBuffer( buf );
	const std::string name = buf.ReadString();
	return new Custom( render_info, name, hint );
}

}
}
}
}