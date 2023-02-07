#include "UI.h"

namespace game {
namespace world {
namespace ui {

UI::UI( const World* world, const string& class_name )
	: UIContainer( class_name )
	, m_world( world )
{
	
}

}
}
}
