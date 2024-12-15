#pragma once

#include <vector>

#include "ui_legacy/object/UIContainer.h"

#include "ui_legacy/event/Types.h"

namespace ui_legacy::object {
class Label;
}

namespace task {
namespace mainmenu {

class MenuBlock;

CLASS( MenuItem, ui_legacy::object::UIContainer )
	MenuItem( MenuBlock* menu, const std::string& text, const bool is_title = false );

	virtual void Create() override;
	virtual void Destroy() override;

	const std::string& GetText() const;

protected:

	bool OnMouseDown( const ui_legacy::event::event_data_t* data ) override;

	MenuBlock* m_menu = nullptr;
	const std::string m_text = "";
	const bool m_is_title = false;

	ui_legacy::object::Label* m_label = nullptr;

	std::vector< UIObject* > m_parts = {};
};

}
}
