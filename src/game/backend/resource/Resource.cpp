#include "Resource.h"

#include <limits>

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
		const auto count = buf.ReadCollectionSize( "resource render coordinate" );
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
	if ( buf.GetRemaining() != 0 ) {
		THROW( "unexpected data after serialized resource" );
	}
	if ( id.empty() ) {
		THROW( "serialized resource id is empty" );
	}
	ValidateRenderInfo( render_info );
	return new Resource( id, name, render_info );
}

void Resource::ValidateRenderInfo( const render_info_t& render_info, const bool require_single_coordinate ) {
	if (
		render_info.file.empty() ||
		render_info.coords.empty() ||
		( require_single_coordinate && render_info.coords.size() != 1 )
	) {
		THROW( "invalid serialized resource render definition" );
	}
	const auto max_coordinate = static_cast< int64_t >( ( std::numeric_limits< uint32_t >::max )() ) - 1;
	for ( const auto& coordinate : render_info.coords ) {
		if (
			coordinate.first.x < 0 || coordinate.first.y < 0 ||
			coordinate.second.x < coordinate.first.x || coordinate.second.y < coordinate.first.y ||
			coordinate.second.x > max_coordinate || coordinate.second.y > max_coordinate
		) {
			THROW( "invalid serialized resource render coordinates" );
		}
	}
}

}
}
}
