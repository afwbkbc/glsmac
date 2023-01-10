#pragma once

#include "ui/object/Button.h"

#include "MenuItemMesh.h"

//#include "scene/actor/MeshActor.h"

namespace task {
namespace mainmenu {

CHILD_CLASS( MenuItem, ui::object::Button )
	MenuItem( const types::Texture* texture );

	virtual void Create();
	virtual void Destroy();
	//virtual void Align()
	virtual void Draw();


protected:
	
	//const MenuItemMesh* m_mesh;
	//scene::actor::MeshActor *m_actor;
	
};

}
}
