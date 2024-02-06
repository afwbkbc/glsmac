#pragma once

#include "Def.h"

#include "Render.h"

namespace game {
namespace unit {

class StaticDef : public Def {
public:

	StaticDef( const std::string& name, const Render* render )
		: Def( name )
		, m_render( render ) {}

	~StaticDef() {
		delete m_render;
	}

private:
	const Render* m_render;

};

}
}
