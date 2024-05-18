#include "FramesRow.h"

namespace game {
namespace animation {

FramesRow::FramesRow(
	const std::string& id,
	const std::string& file,
	const uint16_t row_x,
	const uint16_t row_y,
	const uint16_t frame_width,
	const uint16_t frame_height,
	const uint16_t frame_center_x,
	const uint16_t frame_center_y,
	const uint16_t frame_padding,
	const uint8_t frames_count,
	const uint8_t frames_per_row,
	const float scale_x,
	const float scale_y,
	const uint16_t duration_ms,
	const std::string& sound_file
)
	: Def( id, AT_FRAMES_ROW, duration_ms, scale_x, scale_y, sound_file )
	, m_file( file )
	, m_row_x( row_x )
	, m_row_y( row_y )
	, m_frame_width( frame_width )
	, m_frame_height( frame_height )
	, m_frame_center_x( frame_center_x )
	, m_frame_center_y( frame_center_y )
	, m_frame_padding( frame_padding )
	, m_frames_count( frames_count )
	, m_frames_per_row( frames_per_row ) {
	//
}

const std::string FramesRow::ToString( const std::string& prefix ) const {
	return (std::string)
		TS_OBJ_BEGIN( "FramesRow" ) +
		TS_OBJ_PROP_STR( "id", m_id ) +
		TS_OBJ_PROP_STR( "file", m_file ) +
		TS_OBJ_PROP_NUM( "row_x", m_row_x ) +
		TS_OBJ_PROP_NUM( "row_y", m_row_y ) +
		TS_OBJ_PROP_NUM( "frame_width", m_frame_width ) +
		TS_OBJ_PROP_NUM( "frame_height", m_frame_height ) +
		TS_OBJ_PROP_NUM( "frame_center_x", m_frame_center_x ) +
		TS_OBJ_PROP_NUM( "frame_center_y", m_frame_center_y ) +
		TS_OBJ_PROP_NUM( "frame_padding", m_frame_padding ) +
		TS_OBJ_PROP_NUM( "frames_count", m_frames_count ) +
		TS_OBJ_PROP_NUM( "frames_per_row", m_frames_per_row ) +
		TS_OBJ_PROP_NUM( "scale_x", m_scale_x ) +
		TS_OBJ_PROP_NUM( "scale_y", m_scale_y ) +
		TS_OBJ_PROP_NUM( "duration_ms", m_duration_ms ) +
		TS_OBJ_PROP_STR( "sound_file", m_sound_file ) +
		TS_OBJ_END();
}

void FramesRow::Serialize( types::Buffer& buf, const FramesRow* def ) {
	buf.WriteString( def->m_file );
	buf.WriteInt( def->m_row_x );
	buf.WriteInt( def->m_row_y );
	buf.WriteInt( def->m_frame_width );
	buf.WriteInt( def->m_frame_height );
	buf.WriteInt( def->m_frame_center_x );
	buf.WriteInt( def->m_frame_center_y );
	buf.WriteInt( def->m_frame_padding );
	buf.WriteInt( def->m_frames_count );
	buf.WriteInt( def->m_frames_per_row );
}

FramesRow* FramesRow::Unserialize(
	types::Buffer& buf,
	const std::string& id,
	const float scale_x,
	const float scale_y,
	const uint16_t duration_ms,
	const std::string& sound_file
) {
	const auto file = buf.ReadString();
	const auto row_x = buf.ReadInt();
	const auto row_y = buf.ReadInt();
	const auto frame_width = buf.ReadInt();
	const auto frame_height = buf.ReadInt();
	const auto frame_center_x = buf.ReadInt();
	const auto frame_center_y = buf.ReadInt();
	const auto frame_padding = buf.ReadInt();
	const auto frames_count = buf.ReadInt();
	const auto frames_per_row = buf.ReadInt();
	return new FramesRow(
		id,
		file,
		row_x,
		row_y,
		frame_width,
		frame_height,
		frame_center_x,
		frame_center_y,
		frame_padding,
		frames_count,
		frames_per_row,
		scale_x,
		scale_y,
		duration_ms,
		sound_file
	);
}

}
}
