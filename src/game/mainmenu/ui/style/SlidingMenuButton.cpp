#include "SlidingMenuButton.h"

#include "ui/object/UIObject.h"

namespace game {
namespace mainmenu {
namespace style {

void SlidingMenuButton::SetStyle() {
	Set( A_LEFT, 0 );
	Set( A_WIDTH, 300 );
	Set( A_HEIGHT, 46 );
	Set( A_ALIGN, ui::object::UIObject::ALIGN_BOTTOM | ui::object::UIObject::ALIGN_LEFT );
};


}
}
}
