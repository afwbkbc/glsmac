#pragma once

#include "Entity.h"

#include "types/Color.h"

namespace scene {

class Scene;

CLASS( Light, Entity )

	enum light_type_t {
		LT_AMBIENT_DIFFUSE
	};

	Light( const light_type_t light_type );

	void SetColor( const types::Color& color );
	const types::Color& GetColor() const;

private:

	types::Color m_color = {
		1.0f,
		1.0f,
		1.0f,
		0.5f
	};

};

}
