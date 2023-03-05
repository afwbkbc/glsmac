#include "UI.h"

namespace game {
namespace world {
namespace ui {

UI::UI( World* world, const std::string& class_name )
	: UIContainer( class_name )
	, m_world( world )
{
	
}

}
}
}
