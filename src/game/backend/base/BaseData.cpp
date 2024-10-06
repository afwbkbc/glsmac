#include "BaseData.h"

namespace game {
namespace backend {
namespace base {

BaseData::BaseData( const std::string& name, const size_t population )
	: name( name )
	, population( population ) {
	//
}

BaseData::BaseData( types::Buffer& buf ) {
	Unserialize( buf );
}

void BaseData::Serialize( types::Buffer& buf ) const {
	buf.WriteString( name );
	buf.WriteInt( population );
}

void BaseData::Unserialize( types::Buffer& buf ) {
	name = buf.ReadString();
	population = buf.ReadInt();
}

TS_BEGIN( BaseData )
		TS_FUNC_BEGIN( "BaseData" ) +
			TS_OBJ_PROP_STR( "name", name ) +
			TS_OBJ_PROP_NUM( "population", population ) +
		TS_FUNC_END()
TS_END()

}
}
}
