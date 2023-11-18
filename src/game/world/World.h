#pragma once

#include "base/Module.h"

namespace game {
namespace world {

CLASS( World, base::Module )

	void Start() override;
	void Stop() override;
	void Iterate() override;

};

}
}
