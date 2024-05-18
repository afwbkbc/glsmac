#pragma once

#include "base/Module.h"

namespace base {
class Task;
}

namespace scheduler {

CLASS( Scheduler, base::Module )
	virtual void AddTask( base::Task* task ) = 0;
	virtual void RemoveTask( base::Task* task ) = 0;
protected:
};

}
