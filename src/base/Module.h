#pragma once

#include <vector>

#include "Base.h"

namespace base {

class Module : public Base {
public:
	virtual ~Module() {};

	virtual void Start() {};
	virtual void Stop() {};
	virtual void Iterate() {};
};

typedef std::vector<base::Module *> modules_t;

} /* namespace base */
