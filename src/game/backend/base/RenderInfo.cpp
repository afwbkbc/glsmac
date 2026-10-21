#include "RenderInfo.h"

#include "common/Common.h"
#include "types/Buffer.h"

namespace game {
namespace backend {
namespace base {

const RenderInfo RenderInfo::FromValue( GSE_CALLABLE, const gse::value::object_properties_t& render ) {
	N_ARGS;

	RenderInfo r = {};
	N_GETPROP( type, render, "type", String );
	if ( type == "sprite" ) {
		N_GETPROP( file, render, "file", String );
		N_GETPROP( x, render, "x", Int );
		N_GETPROP( y, render, "y", Int );
		N_GETPROP( w, render, "w", Int );
		N_GETPROP( h, render, "h", Int );
		r = {
			file,
			(uint16_t)x,
			(uint16_t)y,
			(uint16_t)w,
			(uint16_t)h
		};
	}
	else {
		GSE_ERROR( gse::EC.INVALID_CALL, "Only sprite facilities are supported for now" );
	}

	return r;
}

const RenderInfo RenderInfo::FromBuffer( types::Buffer& buf ) {
	RenderInfo r = {};
	r.file = buf.ReadString();
	r.x = buf.ReadInt();
	r.y = buf.ReadInt();
	r.width = buf.ReadInt();
	r.height = buf.ReadInt();
	return r;
}

const std::string RenderInfo::ToString( const std::string& prefix ) const {
	return TS_OBJ_PROP_STR( "file", file ) +
		TS_OBJ_PROP_NUM( "x", x ) +
		TS_OBJ_PROP_NUM( "y", y ) +
		TS_OBJ_PROP_NUM( "width", width ) +
		TS_OBJ_PROP_NUM( "height", height );
}

const std::string RenderInfo::ToTextureString() const {
	return file + ":crop(" + std::to_string( x ) + "," + std::to_string( y ) + "," + std::to_string( x + width ) + "," + std::to_string( y + height ) + ")";
}

void RenderInfo::Write( types::Buffer& buf ) const {
	buf.WriteString( file );
	buf.WriteInt( x );
	buf.WriteInt( y );
	buf.WriteInt( width );
	buf.WriteInt( height );
}

}
}
}
