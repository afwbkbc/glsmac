#pragma once

#include <string>
#include <cstdint>

#include "types/Buffer.h"

namespace game {
namespace animation {

class Def {
public:
	enum animation_type_t {
		AT_NONE,
		AT_FRAMES_ROW,
	};

	Def(
		const std::string& id,
		const animation_type_t type,
		const uint16_t duration_ms,
		const std::string& sound_file
	);
	virtual ~Def() = default;

	const std::string m_id;
	const animation_type_t m_type;
	const uint16_t m_duration_ms;
	const std::string m_sound_file;

	virtual const std::string ToString( const std::string& prefix = "" ) const = 0;

	static const types::Buffer Serialize( const Def* def );
	static Def* Unserialize( types::Buffer& buf );

};

}
}
