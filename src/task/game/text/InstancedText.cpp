#include "InstancedText.h"

namespace task {
namespace game {
namespace text {

InstancedText::InstancedText( const std::string& name, InstancedFont* font )
	: m_name( name )
	, m_font( font ) {
	//
}

void InstancedText::ShowAt( const types::Vec3& coords ) {
	
}

}
}
}