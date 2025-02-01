#pragma once

#include "common/Task.h"

#include "util/Timer.h"

class GLSMAC;

namespace task {
namespace principal {

CLASS( Main, common::Task )
	void Start() override;
	void Stop() override;
	void Iterate() override;

private:
	GLSMAC* m_glsmac = nullptr;

};

}
}
