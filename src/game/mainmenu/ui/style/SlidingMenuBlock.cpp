#include "SlidingMenuBlock.h"

#include "ui/object/UIObject.h"

namespace game {
namespace mainmenu {
namespace style {

void SlidingMenuBlock::SetStyle() {
	Set( A_ALIGN, ui::object::UIObject::ALIGN_RIGHT | ui::object::UIObject::ALIGN_BOTTOM );
	Set( A_WIDTH, 354 );
	Set( A_BOTTOM, 16 );
};


}
}
}
