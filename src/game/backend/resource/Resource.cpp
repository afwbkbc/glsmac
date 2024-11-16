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
	return (std::string)
		TS_OBJ_BEGIN( "Resource" ) +
		TS_OBJ_PROP_STR( "id", m_id ) +
		TS_OBJ_PROP_STR( "name", m_name ) +
		TS_OBJ_BEGIN( "renders" ) +
		TS_OBJ_PROP_STR( "file", m_render_info.file ) +
		TS_OBJ_BEGIN( "yields" ) +
		TS_OBJ_PROP_NUM( "grid_x", m_render_info.yields.grid_x ) +
		TS_OBJ_PROP_NUM( "grid_y", m_render_info.yields.grid_y ) +
		TS_OBJ_PROP_NUM( "grid_margin", m_render_info.yields.grid_margin ) +
		TS_OBJ_PROP_NUM( "cell_width", m_render_info.yields.cell_width ) +
		TS_OBJ_PROP_NUM( "cell_height", m_render_info.yields.cell_height ) +
		TS_OBJ_PROP_NUM( "cells_count", m_render_info.yields.cells_count ) +
		TS_OBJ_END() +
#define X( _r ) \
        TS_OBJ_BEGIN( #_r ) + \
        TS_OBJ_PROP_NUM( "x", m_render_info._r.x ) + \
        TS_OBJ_PROP_NUM( "y", m_render_info._r.y ) + \
        TS_OBJ_PROP_NUM( "width", m_render_info._r.width ) + \
        TS_OBJ_PROP_NUM( "height", m_render_info._r.height ) + \
        TS_OBJ_END() +
		X( plus )
		X( minus )
#undef X
		TS_OBJ_END();
}

const types::Buffer Resource::Serialize( const Resource* resource ) {
	types::Buffer buf;
	buf.WriteString( resource->m_id );
	buf.WriteString( resource->m_name );
	buf.WriteString( resource->m_render_info.file );
	{
		const auto& r = resource->m_render_info.yields;
		buf.WriteInt( r.grid_x );
		buf.WriteInt( r.grid_y );
		buf.WriteInt( r.grid_margin );
		buf.WriteInt( r.cell_width );
		buf.WriteInt( r.cell_height );
		buf.WriteInt( r.cells_count );
	};
#define X( _n ) \
    { \
        const auto& r = resource->m_render_info._n; \
        buf.WriteInt( r.x ); \
        buf.WriteInt( r.y ); \
        buf.WriteInt( r.width ); \
        buf.WriteInt( r.height ); \
    };
	X( plus )
	X( minus )
#undef X
	return buf;
}

Resource* Resource::Unserialize( types::Buffer& buf ) {
	const auto id = buf.ReadString();
	const auto name = buf.ReadString();
	render_info_t render_info = {};
	render_info.file = buf.ReadString();
	{
		auto& r = render_info.yields;
		r.grid_x = buf.ReadInt();
		r.grid_y = buf.ReadInt();
		r.grid_margin = buf.ReadInt();
		r.cell_width = buf.ReadInt();
		r.cell_height = buf.ReadInt();
		r.cells_count = buf.ReadInt();
	};
#define X( _n ) \
    { \
        auto& r = render_info._n; \
        r.x = buf.ReadInt(); \
        r.y = buf.ReadInt(); \
        r.width = buf.ReadInt(); \
        r.height = buf.ReadInt(); \
    };
	X( plus )
	X( minus )
#undef X
	return new Resource( id, name, render_info );
}

}
}
}
