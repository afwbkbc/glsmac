#pragma once

#include "Panel.h"

namespace ui {
namespace dom {

class Scrollbar;
class Container;

class Scrollview : public Panel {
public:
	Scrollview( DOM_ARGS_TD( "scrollview" ) );

protected:
	const bool ProcessEvent( GSE_CALLABLE, const input::Event& event ) override;

private:

	Container* m_inner = nullptr;
	Scrollbar* m_vscroll = nullptr;
	Scrollbar* m_hscroll = nullptr;

};

}
}
