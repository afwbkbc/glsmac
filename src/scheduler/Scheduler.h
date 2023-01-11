#pragma once

#include "base/Module.h"
#include "base/Error.h"
#include "base/Task.h"

using namespace base;

namespace scheduler {

MAJOR_CLASS( Scheduler, Module )
	virtual void AddTask( Task *task ) = 0;
	virtual void RemoveTask( Task *task ) = 0;
protected:
};

} /* namespace scheduler */
