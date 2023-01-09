#pragma once

#include "TestTask.h"

#include "ui/object/Panel.h"
#include "ui/object/Button.h"

namespace task {
namespace test {

MINOR_CLASS( UI, TestTask )
	void Start();
	void Stop();
	void Iterate();
protected:
	ui::object::Panel *m_panel;
	ui::object::Panel *m_sub_panel;
	ui::object::Button *m_button;
};

} /* namespace test */
} /* namespace task */
