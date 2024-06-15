#pragma once

#include "common/Module.h"

namespace common {
class Task;
}

namespace scheduler {

CLASS( Scheduler, common::Module )
	virtual void AddTask( common::Task* task ) = 0;
	virtual void RemoveTask( common::Task* task ) = 0;
protected:
};

}
