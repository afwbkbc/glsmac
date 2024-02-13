#pragma once

#include "types/Buffer.h"

namespace game {
namespace unit {

class Render {
public:

	enum render_type_t {
		RT_SPRITE,
	};

	Render( const render_type_t type );

	const render_type_t m_type;

private:
	friend class StaticDef;

	static void Serialize( types::Buffer& buf, const Render* render );
	static Render* Unserialize( types::Buffer& buf );

};

}
}
