#pragma once

#include <vector>
#include <functional>
#include <unordered_map>

#include "Base.h"

namespace base {

typedef size_t mt_id_t;

class Module : public Base {
public:
	virtual void Start() {}
	virtual void Stop() {}
	virtual void Iterate() {}
};

typedef std::vector<base::Module *> modules_t;

} /* namespace base */
