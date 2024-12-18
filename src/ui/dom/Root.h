#pragma once

#include "Container.h"

namespace ui {

class UI;

namespace dom {

class Root : public Container {
public:
	Root( GSE_CALLABLE, UI* const ui );

private:
	friend class ui::UI;
	void Resize( const uint16_t window_width, const uint16_t window_height );
};

}
}
