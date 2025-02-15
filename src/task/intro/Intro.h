#pragma once

#include "common/Task.h"

#include "util/Timer.h"

namespace ui_legacy::object {
class Surface;
}

namespace task {
namespace intro {

class Theme;

CLASS( Intro, common::Task )
	void Start() override;
	void Stop() override;
	void Iterate() override;

private:
	Theme* m_theme = nullptr;
	::ui_legacy::object::Surface* m_logo = nullptr;
	util::Timer m_timer;

	void Finish();

};

}
}
