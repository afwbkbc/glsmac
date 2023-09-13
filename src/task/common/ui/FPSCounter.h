#pragma once

#include "ui/object/Label.h"

#include "util/Timer.h"

namespace task {
namespace ui {
			
CLASS( FPSCounter, ::ui::object::Label )
	
	FPSCounter();
	
	void Create() override;
	void Iterate() override;
	void Destroy() override;
	
private:
	util::Timer m_timer;
	
};

}
}
