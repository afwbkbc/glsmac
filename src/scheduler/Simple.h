#pragma once

#include <vector>

#include "Scheduler.h"

#include "base/Task.h"
#include "util/Timer.h"

using namespace base;

namespace scheduler {

CLASS( Simple, Scheduler )

	Simple();
	virtual ~Simple();
	void Start() override;
	void Stop() override;
	void Iterate() override;
	void AddTask( Task* task ) override;
	void RemoveTask( Task* task ) override;
	
protected:

#ifdef DEBUG
	util::Timer m_timer;
#endif

	std::vector< Task* > m_tasks = {};
	std::vector< Task* > m_tasks_toadd = {};
	std::vector< Task* > m_tasks_toremove = {};
	bool m_active = false;
	bool m_iterating = false;
};

} /* namespace scheduler */
