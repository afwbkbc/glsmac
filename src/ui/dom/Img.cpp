#include "Img.h"

namespace ui {
namespace dom {

Img::Img( GSE_CALLABLE, UI* const ui, Object* const parent, const properties_t& properties )
	: Surface( ctx, call_si, ui, "img", parent, properties ) {
	Property( ctx, call_si, "src", gse::type::Type::T_STRING );
}

}
}
