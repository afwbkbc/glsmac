#pragma once

#include "Def.h"

#include "Render.h"

namespace game {
namespace unit {

class StaticDef : public Def {
public:
	StaticDef( const std::string& name, const Render* render );
	~StaticDef();

	const Render* m_render;

private:
	friend class Def;

	static void Serialize( types::Buffer& buf, const StaticDef* def );
	static StaticDef* Unserialize( types::Buffer& buf, const std::string& name );

};

}
}
