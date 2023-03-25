#pragma once

#include "UI.h"

#include "ui/object/Section.h"

namespace game {
namespace world {
namespace ui {

CLASS( Section, UI )

	Section( World* world, const std::string class_name, const std::string section_class_prefix );
	
	void Create();
	void Destroy();

	void AddChild( UIObject *object );
	void RemoveChild( UIObject *object );
	
protected:
	struct {
		bool no_outer_border = false;
		bool no_inner_border = false;
	} m_config = {};
	
private:
	const std::string m_section_class_prefix = "";
	
	object::Section* m_outer = nullptr;
	object::Section* m_inner = nullptr;
	
};
	
}
}
}
