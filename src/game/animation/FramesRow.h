#pragma once

#include <cstdint>

#include "Def.h"

namespace game {
namespace animation {

class FramesRow : public Def {
public:

	FramesRow(
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
	);

	const std::string m_file;
	const uint16_t m_row_x;
	const uint16_t m_row_y;
	const uint16_t m_frame_width;
	const uint16_t m_frame_height;
	const uint16_t m_frame_padding;
	const uint16_t m_frame_center_x;
	const uint16_t m_frame_center_y;
	const uint8_t m_frames_count;
	const uint8_t m_frames_per_row;

	const std::string ToString( const std::string& prefix ) const override;

private:
	friend class Def;

	static void Serialize( types::Buffer& buf, const FramesRow* def );
	static FramesRow* Unserialize(
		types::Buffer& buf,
		const std::string& id,
		const float scale_x,
		const float scale_y,
		const uint16_t duration_ms,
		const std::string& sound_file
	);
};

}
}
