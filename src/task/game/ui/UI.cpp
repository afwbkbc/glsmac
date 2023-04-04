#include "UI.h"

namespace task {
namespace game {
namespace ui {

UI::UI( Game* game, const std::string& class_name )
	: UIContainer( class_name )
	, m_game( game )
{
	
}

}
}
}
