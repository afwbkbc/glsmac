#include "Img.h"

namespace ui {
namespace dom {

Img::Img( DOM_ARGS )
	: Surface( DOM_ARGS_PASS, "img" ) {
	Property( ctx, call_si, "src", gse::type::Type::T_STRING );
}

}
}
