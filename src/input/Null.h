#pragma once

#include "Input.h"

namespace input {

CLASS( Null, Input )
	const std::string GetClipboardText() const override { return ""; }
};

}
