#include "Resource.h"

namespace game {
namespace backend {
namespace resource {

Resource::Resource( const std::string& id, const std::string& name, const render_info_t& render_info )
	: m_id( id )
	, m_name( name )
	, m_render_info( render_info ) {
	//
}

const std::string Resource::ToString( const std::string& prefix ) const {
	auto result = (std::string)
		TS_OBJ_BEGIN( "Resource" ) +
		TS_OBJ_PROP_STR( "id", m_id ) +
		TS_OBJ_PROP_STR( "name", m_name ) +
		TS_OBJ_BEGIN( "renders" ) +
		TS_OBJ_PROP_STR( "file", m_render_info.file ) +
		TS_ARR_BEGIN( "coords" );
	for ( size_t i = 0 ; i < m_render_info.coords.size() ; i++ ) {
		const auto& c = m_render_info.coords.at( i );
		result += TS_OBJ_BEGIN( std::to_string( i + 1 ) ) +
			TS_OBJ_PROP_NUM( "x1", c.first.x ) +
			TS_OBJ_PROP_NUM( "y1", c.first.y ) +
			TS_OBJ_PROP_NUM( "x2", c.second.x ) +
			TS_OBJ_PROP_NUM( "y2", c.second.y ) +
			TS_OBJ_END();
	}
	result +=
		TS_ARR_END() +
#define X( _r ) \
        TS_OBJ_BEGIN( #_r ) + \
        TS_OBJ_PROP_NUM( "x", m_render_info._r.x ) + \
        TS_OBJ_PROP_NUM( "y", m_render_info._r.y ) + \
        TS_OBJ_PROP_NUM( "width", m_render_info._r.width ) + \
        TS_OBJ_PROP_NUM( "height", m_render_info._r.height ) + \
        TS_OBJ_END() +
#undef X
			TS_OBJ_END();
	return result;
}

const types::Buffer Resource::Serialize( const Resource* resource ) {
	types::Buffer buf;
	buf.WriteString( resource->m_id );
	buf.WriteString( resource->m_name );
	buf.WriteString( resource->m_render_info.file );
	{
		const auto& coords = resource->m_render_info.coords;
		buf.WriteInt( coords.size() );
		for ( const auto& c : coords ) {
			buf.WriteInt( c.first.x );
			buf.WriteInt( c.first.y );
			buf.WriteInt( c.second.x );
			buf.WriteInt( c.second.y );
		}
	};
#define X( _n ) \
    { \
        const auto& r = resource->m_render_info._n; \
        buf.WriteInt( r.x ); \
        buf.WriteInt( r.y ); \
        buf.WriteInt( r.width ); \
        buf.WriteInt( r.height ); \
    };
#undef X
	return buf;
}

Resource* Resource::Deserialize( types::Buffer& buf ) {
	const auto id = buf.ReadString();
	const auto name = buf.ReadString();
	render_info_t render_info = {};
	render_info.file = buf.ReadString();
	{
		auto& r = render_info.coords;
		const auto count = buf.ReadInt();
		r.reserve( count );
		for ( size_t i = 0 ; i < count ; i++ ) {
			r.push_back(
				{
					{ buf.ReadInt(), buf.ReadInt() },
					{ buf.ReadInt(), buf.ReadInt() }
				}
			);
		}
	};
	return new Resource( id, name, render_info );
}

}
}
}
