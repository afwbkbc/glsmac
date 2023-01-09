#pragma once

#include "base/Module.h"
#include "base/Error.h"
#include "task/Task.h"

namespace scheduler {

MAJOR_CLASS( Scheduler, base::Module )
	virtual void AddTask( task::Task *task ) = 0;
	virtual void RemoveTask( task::Task *task ) = 0;
protected:
};

} /* namespace scheduler */
