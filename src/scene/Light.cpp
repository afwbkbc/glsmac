#include "Light.h"

namespace scene {

Light::Light( const light_type_t light_type )
	: m_light_type( light_type ) {
	//
}

void Light::SetColor( const Color& color ) {
	m_color = color;
}

const Color& Light::GetColor() const {
	return m_color;
}

}
