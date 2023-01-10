#pragma once

#include "ui/object/UIContainer.h"
#include "ui/object/Surface.h"

#include <vector>

#include "MenuItemMesh.h"

using namespace std;
using namespace ui::object;

namespace task {
namespace mainmenu {

CHILD_CLASS( MenuItem, UIContainer )
	MenuItem( const vector<const types::Texture*>* textures );

	virtual void Create();
	virtual void Destroy();
	//virtual void Align()
	virtual void Draw();


protected:
	const vector<const types::Texture*>* m_textures;
	
	Surface* m_top;
	Surface* m_bottom;
	Surface* m_left;
	Surface* m_right;
	Surface* m_body;
	
};

}
}
