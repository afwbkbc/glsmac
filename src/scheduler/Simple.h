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
	void AddTask( base::Task* task ) override;
	void RemoveTask( base::Task* task ) override;

protected:

#ifdef DEBUG
	util::Timer m_timer;
#endif

	std::vector< base::Task* > m_tasks = {};
	std::vector< base::Task* > m_tasks_toadd = {};
	std::vector< base::Task* > m_tasks_toremove = {};
	bool m_active = false;
	bool m_iterating = false;
};

}
