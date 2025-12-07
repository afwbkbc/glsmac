#include "Def.h"

#include "FramesRow.h"

namespace game {
namespace backend {
namespace animation {

Def::Def(
	const std::string& id,
	const animation_type_t type,
	const uint16_t duration_ms,
	const float scale_x,
	const float scale_y,
	const std::string& sound_file
)
	: m_id( id )
	, m_type( type )
	, m_duration_ms( duration_ms )
	, m_scale_x( scale_x )
	, m_scale_y( scale_y )
	, m_sound_file( sound_file ) {
	//
}

const types::Buffer Def::Serialize( const Def* def ) {
	types::Buffer buf;
	buf.WriteString( def->m_id );
	buf.WriteInt( def->m_type );
	buf.WriteFloat( def->m_scale_x );
	buf.WriteFloat( def->m_scale_y );
	buf.WriteInt( def->m_duration_ms );
	buf.WriteString( def->m_sound_file );
	switch ( def->m_type ) {
		case AT_FRAMES_ROW: {
			FramesRow::Serialize( buf, (FramesRow*)def );
			break;
		}
		default:
			THROW( "unknown def type on write: " + std::to_string( def->m_type ) );
	}
	return buf;
}

Def* Def::Deserialize( types::Buffer& buf ) {
	const auto id = buf.ReadString();
	const auto type = (animation_type_t)buf.ReadInt();
	const auto scale_x = buf.ReadFloat();
	const auto scale_y = buf.ReadFloat();
	const auto duration_ms = buf.ReadInt();
	const auto sound_file = buf.ReadString();
	switch ( type ) {
		case AT_FRAMES_ROW:
			return FramesRow::Deserialize( buf, id, scale_x, scale_y, duration_ms, sound_file );
		default:
			THROW( "unknown def type on read: " + std::to_string( type ) );
	}
}

}
}
}
