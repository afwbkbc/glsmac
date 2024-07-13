#pragma once

#include "UI.h"

namespace ui::object {
class Section;
}

namespace game {
namespace frontend {
namespace ui {

CLASS( Section, UI )

	Section( Game* game, const std::string class_name, const std::string section_class_prefix );

	void Create() override;
	void Destroy() override;

	void AddChild( UIObject* object );
	void RemoveChild( UIObject* object );

	void SetTitleText( const std::string& title_text );

protected:
	struct {
		bool no_outer_border = false;
		bool no_inner_border = false;
	} m_config = {};

private:
	const std::string m_section_class_prefix = "";
	std::string m_title_text = "";

	::ui::object::Section* m_outer = nullptr;
	::ui::object::Section* m_inner = nullptr;

};

}
}
}
