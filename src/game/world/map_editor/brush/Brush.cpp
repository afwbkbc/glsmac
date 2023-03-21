#include "Brush.h"

namespace game {
namespace world {
namespace map_editor {
namespace brush {

Brush::Brush( World* world, const MapEditor::brush_type_t type )
	: m_world( world )
	, m_type( type )
{
	//
}

const MapEditor::brush_type_t Brush::GetType() const {
	return m_type;
}

}
}
}
}
