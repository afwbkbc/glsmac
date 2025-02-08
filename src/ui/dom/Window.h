#pragma once

#include "Container.h"

namespace ui {
namespace dom {

class Panel;
class Text;
class Surface;

class Window : public Container {
public:
	Window( DOM_ARGS );

private:
	Panel* m_header;
	Text* m_title;
	Panel* m_body;
	Surface* m_blocker;

	void SetHeaderHeight( const coord_t px );
	void SetTitle( const std::string& title );

};

}
}
