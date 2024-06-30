#include "Tool.h"

namespace game {
namespace map_editor {
namespace tool {

Tool::Tool( Game* game, const tool_type_t type )
	: m_game( game )
	, m_type( type ) {
	//
}

const tool_type_t Tool::GetType() const {
	return m_type;
}

}
}
}
