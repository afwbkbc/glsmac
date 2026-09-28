#include "RenderInfo.h"

#include "common/Common.h"
#include "types/Buffer.h"

namespace game {
namespace backend {
namespace base {

const std::string RenderInfo::ToString( const std::string& prefix ) const {
	return TS_OBJ_PROP_STR( "file", file ) +
		TS_OBJ_PROP_NUM( "x", x ) +
		TS_OBJ_PROP_NUM( "y", y ) +
		TS_OBJ_PROP_NUM( "width", width ) +
		TS_OBJ_PROP_NUM( "height", height );
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
