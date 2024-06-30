#pragma once

#include <vector>

#include "Scheduler.h"

#ifdef DEBUG
#include "util/Timer.h"
#endif

namespace scheduler {

CLASS( Simple, Scheduler )

	Simple();
	virtual ~Simple();
	void Start() override;
	void Stop() override;
	void Iterate() override;
	void AddTask( common::Task* task ) override;
	void RemoveTask( common::Task* task ) override;

protected:

#ifdef DEBUG
	util::Timer m_timer;
#endif

	std::vector< common::Task* > m_tasks = {};
	std::vector< common::Task* > m_tasks_toadd = {};
	std::vector< common::Task* > m_tasks_toremove = {};
	bool m_active = false;
	bool m_iterating = false;
};

}
