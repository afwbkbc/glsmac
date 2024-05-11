#include "Light.h"

namespace scene {

Light::Light( const light_type_t light_type ) {
	//
}

void Light::SetColor( const types::Color& color ) {
	m_color = color;
}

const types::Color& Light::GetColor() const {
	return m_color;
}

}
