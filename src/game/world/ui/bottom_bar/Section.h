#pragma once

#include "../UI.h"

#include "ui/object/Section.h"

namespace game {
namespace world {
namespace ui {

CLASS( Section, UI )

	Section( const World* world, const std::string subclass ) : UI( world, "MapBottomBar" + subclass ) {}
	
	void Create();
	void Destroy();

	void AddChild( UIObject *object );
	void RemoveChild( UIObject *object );
	
private:
	object::Section* m_outer = nullptr;
	object::Section* m_inner = nullptr;
	
};
	
}
}
}
