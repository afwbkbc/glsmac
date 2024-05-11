#pragma once

#include <vector>
#include <functional>
#include <unordered_map>

#include "Base.h"

namespace base {

class Module : public Base {
public:
	virtual ~Module() = default;

	virtual void Start() {}
	virtual void Stop() {}
	virtual void Iterate() {}
};

}
