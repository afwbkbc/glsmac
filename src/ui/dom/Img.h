#pragma once

#include "Object.h"
#include "Surface.h"

namespace ui {
namespace dom {

class Img : public Surface {
public:
	Img( GSE_CALLABLE, UI* const ui, Object* const parent, const properties_t& properties );
};

}
}
