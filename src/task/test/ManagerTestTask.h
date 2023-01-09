#pragma once

#include "TestTask.h"

#include "../../util/Timer.h"

namespace task {
namespace test {

MINOR_CLASS( Manager, TestTask )
	void Start();
	void Stop();
	void Iterate();
	
protected:
	uint8_t m_task_num = 0;
	util::Timer m_timer;
	task::Task* m_task = nullptr;
};

} /* namespace test */
} /* namespace task */
