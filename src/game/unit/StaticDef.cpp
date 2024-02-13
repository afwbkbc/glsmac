#include "StaticDef.h"

namespace game {
namespace unit {

StaticDef::StaticDef( const std::string& name, const Render* render )
	: Def( DT_STATIC, name )
	, m_render( render ) {}

StaticDef::~StaticDef() {
	delete m_render;
}

void StaticDef::Serialize( types::Buffer& buf, const StaticDef* def ) {
	Render::Serialize( buf, def->m_render );
}

StaticDef* StaticDef::Unserialize( types::Buffer& buf, const std::string& name ) {
	return new StaticDef( name, Render::Unserialize( buf ) );
}

}
}
