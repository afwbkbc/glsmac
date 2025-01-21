#pragma once

#include "Panel.h"

namespace ui {
namespace dom {

class Text;

class Button : public Panel {
public:
	Button( DOM_ARGS );

private:
	Text* m_label;

};

}
}
