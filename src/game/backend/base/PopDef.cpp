#include "PopDef.h"

namespace game {
namespace backend {
namespace base {

PopDef::PopDef(
	const std::string& id,
	const std::string& name,
	const render_infos_t& renders_human,
	const render_infos_t& renders_progenitor,
	const pop_flags_t flags
)
	: m_id( id )
	, m_name( name )
	, m_renders_human( renders_human )
	, m_renders_progenitor( renders_progenitor )
	, m_flags( flags ) {
	//
}

const std::string PopDef::ToString( const std::string& prefix ) const {
	return (std::string)
		TS_OBJ_BEGIN( "PopDef" ) +
		TS_OBJ_PROP_STR( "id", m_id ) +
		TS_OBJ_PROP_STR( "name", m_name ) +
		InfosToString( prefix, "renders_human", m_renders_human ) +
		InfosToString( prefix, "renders_progenitor", m_renders_human ) +
		TS_OBJ_PROP_STR( "flags", (
			( m_flags & PF_TILE_WORKER )
				? "tile_worker"
				: ""
		) ) +
		TS_OBJ_END();
}

const types::Buffer PopDef::Serialize( const PopDef* def ) {
	types::Buffer buf;
	buf.WriteString( def->m_id );
	buf.WriteString( def->m_name );
#define X( _r ) \
    buf.WriteInt( def->_r.size() ); \
    for ( const auto& r : def->_r ) { \
        r.Write( buf ); \
    }
	X( m_renders_human )
	X( m_renders_progenitor )
#undef X
	buf.WriteInt( def->m_flags );
	return buf;
}

PopDef* PopDef::Deserialize( types::Buffer& buf ) {
	const auto id = buf.ReadString();
	const auto name = buf.ReadString();
#define X( _r ) \
    render_infos_t _r = {}; \
    _r.resize( buf.ReadInt() ); \
    for ( auto& r : _r ) { \
        r.file = buf.ReadString(); \
        r.x = buf.ReadInt(); \
        r.y = buf.ReadInt(); \
        r.width = buf.ReadInt(); \
        r.height = buf.ReadInt(); \
    }
	X( renders_human )
	X( renders_progenitor )
#undef X
	const auto flags = buf.ReadInt();
	return new PopDef( id, name, renders_human, renders_progenitor, flags );
}

const std::string PopDef::InfosToString( const std::string& prefix, const std::string& name, const render_infos_t& infos ) const {
	std::string result = TS_ARR_BEGIN( name );
	for ( size_t i = 0 ; i < infos.size() ; i++ ) {
		const auto& info = infos.at( i );
		result += TS_OBJ_BEGIN( std::to_string( i ) ) +
			info.ToString( prefix ) +
			TS_OBJ_END();
	}
	result += TS_ARR_END();
	return result;
}

}
}
}
