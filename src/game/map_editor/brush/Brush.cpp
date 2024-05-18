#include "Brush.h"

namespace game {
namespace map_editor {
namespace brush {

Brush::Brush( Game* game, const brush_type_t type )
	: m_game( game )
	, m_type( type ) {
	//
}

const brush_type_t Brush::GetType() const {
	return m_type;
}

}
}
}
