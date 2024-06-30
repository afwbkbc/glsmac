#pragma once

#include <vector>
#include <functional>
#include <unordered_map>

#include "Common.h"

namespace common {

class Module : public Class {
public:
	virtual ~Module() = default;

	virtual void Start() {}
	virtual void Stop() {}
	virtual void Iterate() {}
};

}
