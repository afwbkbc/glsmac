#pragma once

#include <vector>

#include "Scheduler.h"
#include "task/Task.h"

namespace scheduler {

class SimpleScheduler : public Scheduler {
public:
	SimpleScheduler();
	void Start();
	void Stop();
	void Iterate();
	void AddTask( task::Task *task );
	void RemoveTask( task::Task *task );
protected:
	std::vector<task::Task *> m_tasks;
	std::vector<task::Task *> m_tasks_toadd;
	std::vector<task::Task *> m_tasks_toremove;
	bool m_active = false;
	bool m_iterating = false;
};

} /* namespace scheduler */
