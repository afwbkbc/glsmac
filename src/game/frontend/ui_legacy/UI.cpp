#include "UI.h"

namespace game {
namespace frontend {
namespace ui_legacy {

UI::UI( Game * game,
const std::string& class_name
)
:
UIContainer ( class_name )
, m_game( game ) {

}

}
}
}
