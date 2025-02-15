#pragma once

#include "ui_legacy/object/Label.h"

#include "util/Timer.h"

namespace task {
namespace ui_legacy {

CLASS( FPSCounter, ::ui_legacy::object::Label )

	FPSCounter();

	void Create() override;
	void Iterate() override;
	void Destroy() override;

private:
	util::Timer m_timer;

};

}
}
