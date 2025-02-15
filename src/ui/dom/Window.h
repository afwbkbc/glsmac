#pragma once

#include "Panel.h"

namespace ui {
namespace dom {

class Panel;
class Text;
class Surface;

class Window : public Panel {
public:
	Window( DOM_ARGS );

private:
	Panel* m_header;
	Text* m_title;
	Container* m_body;
	Surface* m_blocker;

	void SetHeaderHeight( const coord_t px );
	void SetTitle( const std::string& title );

};

}
}
