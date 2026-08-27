#include "PopDef.h"

#include <limits>

namespace game {
namespace backend {
namespace base {

PopDef::PopDef(
	const std::string& id,
	const std::string& name,
	const pop_render_infos_t& renders_human,
	const pop_render_infos_t& renders_progenitor,
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
        buf.WriteString( r.file ); \
        buf.WriteInt( r.x ); \
        buf.WriteInt( r.y ); \
        buf.WriteInt( r.width ); \
        buf.WriteInt( r.height ); \
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
	if ( id.empty() ) {
		THROW( "serialized base population definition id is empty" );
	}
#define X( _r ) \
    pop_render_infos_t _r = {}; \
    { \
        const auto count = buf.ReadCollectionSize( #_r " render" ); \
        if ( count == 0 || count > static_cast< size_t >( std::numeric_limits< uint8_t >::max() ) + 1 ) { \
            THROW( "invalid serialized " #_r " render count" ); \
        } \
        _r.resize( count ); \
    } \
    for ( auto& r : _r ) { \
        r.file = buf.ReadString(); \
        r.x = buf.ReadInt< uint16_t >( #_r " render x" ); \
        r.y = buf.ReadInt< uint16_t >( #_r " render y" ); \
        r.width = buf.ReadInt< uint16_t >( #_r " render width" ); \
        r.height = buf.ReadInt< uint16_t >( #_r " render height" ); \
        if ( r.file.empty() || r.width == 0 || r.height == 0 ) { \
            THROW( "invalid serialized " #_r " render" ); \
        } \
    }
	X( renders_human )
	X( renders_progenitor )
#undef X
	const auto flags = buf.ReadInt< pop_flags_t >( "base population definition flags" );
	if ( flags & ~PF_TILE_WORKER ) {
		THROW( "invalid serialized base population definition flags" );
	}
	return new PopDef( id, name, renders_human, renders_progenitor, flags );
}

const std::string PopDef::InfosToString( const std::string& prefix, const std::string& name, const pop_render_infos_t& infos ) const {
	std::string result = TS_ARR_BEGIN( name );
	for ( size_t i = 0 ; i < infos.size() ; i++ ) {
		const auto& info = infos.at( i );
		result += TS_OBJ_BEGIN( std::to_string( i ) ) +
			TS_OBJ_PROP_STR( "file", info.file ) +
			TS_OBJ_PROP_NUM( "x", info.x ) +
			TS_OBJ_PROP_NUM( "y", info.y ) +
			TS_OBJ_PROP_NUM( "width", info.width ) +
			TS_OBJ_PROP_NUM( "height", info.height ) +
			TS_OBJ_END();
	}
	result += TS_ARR_END();
	return result;
}

}
}
}
