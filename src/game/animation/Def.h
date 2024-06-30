#pragma once

#include <string>
#include <cstdint>

#include "Types.h"
#include "types/Buffer.h"

namespace game {
namespace animation {

class Def {
public:

	Def(
		const std::string& id,
		const animation_type_t type,
		const uint16_t duration_ms,
		const float scale_x,
		const float scale_y,
		const std::string& sound_file
	);
	virtual ~Def() = default;

	const std::string m_id;
	const animation_type_t m_type;
	const uint16_t m_duration_ms;
	const float m_scale_x;
	const float m_scale_y;
	const std::string m_sound_file;

	virtual const std::string ToString( const std::string& prefix = "" ) const = 0;

	static const types::Buffer Serialize( const Def* def );
	static Def* Unserialize( types::Buffer& buf );

};

}
}
