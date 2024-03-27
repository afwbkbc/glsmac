#pragma once

#include <cstdint>
#include <string>

namespace game {
namespace unit {

class Morale {
public:
	typedef uint8_t morale_t;
	static const morale_t MORALE_MIN;
	static const morale_t MORALE_MAX;

	Morale( const std::string& name );

	const std::string m_name;

};

}
}
