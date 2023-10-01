#pragma once

#include "Entity.h"

#include "types/Color.h"

using namespace types;

namespace scene {

class Scene;

CLASS( Light, Entity )

	enum light_type_t {
		LT_AMBIENT_DIFFUSE
	};

	Light( const light_type_t light_type );

	void SetColor( const Color& color );
	const Color& GetColor() const;

private:

	Color m_color = {
		1.0f,
		1.0f,
		1.0f,
		0.5f
	};

};

}
