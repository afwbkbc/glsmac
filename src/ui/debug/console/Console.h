#pragma once

#include "ui/object/UIContainer.h"
#include "ui/object/Surface.h"

#include "types/Texture.h"

namespace ui {
	
using namespace object;
	
namespace debug {

CHILD_CLASS(Console, UIContainer)

	void Create();
	void Destroy();
	void Iterate();
	
	void UpdateHeight();
	
protected:
	
	types::Texture* m_body_background = nullptr;
	
	Surface* m_body = nullptr;
	
};	
	
}
}
