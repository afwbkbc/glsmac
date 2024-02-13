#include "Unit.h"

namespace game {
namespace unit {

static size_t next_id = 1;
const size_t Unit::GetNextId() {
	return next_id;
}

Unit::Unit( const size_t id, const Def* def, const size_t pos_x, const size_t pos_y )
	: m_id( id )
	, m_def( def )
	, m_pos_x( pos_x )
	, m_pos_y( pos_y ) {
	if ( next_id <= id ) {
		next_id = id + 1;
	}
}

const types::Buffer Unit::Serialize( const Unit* unit ) {
	types::Buffer buf;
	buf.WriteInt( unit->m_id );
	buf.WriteString( Def::Serialize( unit->m_def ).ToString() );
	buf.WriteInt( unit->m_pos_x );
	buf.WriteInt( unit->m_pos_y );
	return buf;
}

Unit* Unit::Unserialize( types::Buffer& buf ) {
	const auto id = buf.ReadInt();
	auto defbuf = types::Buffer( buf.ReadString() );
	const auto* def = Def::Unserialize( defbuf );
	const auto pos_x = buf.ReadInt();
	const auto pos_y = buf.ReadInt();
	return new Unit( id, def, pos_x, pos_y );
}

}
}
