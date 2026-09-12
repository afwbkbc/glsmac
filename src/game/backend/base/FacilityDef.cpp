#include "FacilityDef.h"

namespace game {
namespace backend {
namespace base {

FacilityDef::FacilityDef(
	const std::string& id,
	const std::string& name,
	const std::string& description,
	const size_t cost,
	const render_info_t& render_info
)
	: m_id( id )
	, m_name( name )
	, m_description( description )
	, m_cost( cost )
	, m_render_info( render_info ) {
	//
}

const std::string FacilityDef::ToString( const std::string& prefix ) const {
	return (std::string)
		TS_OBJ_BEGIN( "PopDef" ) +
		TS_OBJ_PROP_STR( "id", m_id ) +
		TS_OBJ_PROP_STR( "name", m_name ) +
		TS_OBJ_PROP_STR( "description", m_description ) +
		TS_OBJ_PROP_NUM( "cost", m_cost ) +
		m_render_info.ToString( "render", prefix ) +
		TS_OBJ_END();
}

const types::Buffer FacilityDef::Serialize( const FacilityDef* def ) {
	types::Buffer buf;
	buf.WriteString( def->m_id );
	buf.WriteString( def->m_name );
	buf.WriteString( def->m_description );
	buf.WriteInt( def->m_cost );
	buf.WriteString( def->m_render_info.file );
	buf.WriteInt( def->m_render_info.x );
	buf.WriteInt( def->m_render_info.y );
	buf.WriteInt( def->m_render_info.width );
	buf.WriteInt( def->m_render_info.height );
	return buf;
}

FacilityDef* FacilityDef::Deserialize( types::Buffer& buf ) {
	const auto id = buf.ReadString();
	const auto name = buf.ReadString();
	const auto description = buf.ReadString();
	const auto cost = buf.ReadInt();
	render_info_t r = {};
	r.file = buf.ReadString();
	r.x = buf.ReadInt();
	r.y = buf.ReadInt();
	r.width = buf.ReadInt();
	r.height = buf.ReadInt();
	return new FacilityDef( id, name, description, cost, r );
}

}
}
}
