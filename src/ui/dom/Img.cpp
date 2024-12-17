#include "Img.h"

namespace ui {
namespace dom {

Img::Img( Object* const parent, const properties_t& properties )
	: Object( "img", parent, properties ) {
	Property( "src", gse::type::Type::T_STRING );
}

}
}
