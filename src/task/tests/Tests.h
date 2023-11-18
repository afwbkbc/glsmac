#pragma once

#include "base/Task.h"

namespace task {
namespace tests {

CLASS( Tests, base::Task )
	void Start() override;
	void Stop() override;
	void Iterate() override;

};

}
}
