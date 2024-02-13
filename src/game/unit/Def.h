#pragma once

#include "types/Buffer.h"

#include <string>

namespace game {
namespace unit {

class Def {
public:

	enum def_type_t {
		DT_STATIC,
	};

	Def( const def_type_t type, const std::string& name );

	const def_type_t m_type;
	const std::string m_name;

	static const types::Buffer Serialize( const Def* def );
	static Def* Unserialize( types::Buffer& buf );

private:
};

}
}
