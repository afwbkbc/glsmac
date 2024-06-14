#pragma once

#include <string>

#include "types/Vec3.h"

namespace task {
namespace game {

namespace sprite {
class InstancedSprite;
}

namespace text {

class InstancedFont;

class InstancedText {
public:
	InstancedText( const std::string& name, InstancedFont* font );

	void ShowAt( const types::Vec3& coords );

private:
	const std::string m_name = "";
	InstancedFont* m_font = nullptr;

};

}
}
}
