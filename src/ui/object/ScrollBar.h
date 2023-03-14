#pragma once

#include "UIContainer.h"

#include "Surface.h"

namespace ui {
namespace object {

CLASS( ScrollBar, UIContainer )
	
	ScrollBar();
	
	void Create();
	void Destroy();

private:
	Surface* m_background = nullptr;
	Surface* m_up = nullptr;
	Surface* m_down = nullptr;
	Surface* m_slider = nullptr;
	
};

}
}
