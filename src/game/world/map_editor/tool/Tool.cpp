#include "Tool.h"

namespace game {
namespace world {
namespace map_editor {
namespace tool {

Tool::Tool( World* world, const MapEditor::tool_type_t type )
	: m_world( world )
	, m_type( type )
{
	//
}

const MapEditor::tool_type_t Tool::GetType() const {
	return m_type;
}

}
}
}
}
