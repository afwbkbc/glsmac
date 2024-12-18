#pragma once

#include "common/Common.h"
#include "gse/Wrappable.h"

#include "Types.h"

#include "gse/type/Object.h"
#include "util/Clamper.h"
#include "types/Vec2.h"
#include "types/mesh/Types.h"

namespace scene {
class Scene;
}

namespace ui {

class Geometry;

namespace dom {
class Root;
}

CLASS2( UI, common::Class, gse::Wrappable )

	UI( GSE_CALLABLE );
	~UI();

	WRAPDEFS_PTR( UI );

	scene::Scene* const m_scene = nullptr;

private:
	dom::Root* m_root;

	struct {
		util::Clamper< float > x;
		util::Clamper< float > y;
	} m_clamp = {};

	void Resize();

private:
	friend class Geometry;
	const types::Vec2< types::mesh::coord_t > Clamp( const types::Vec2< coord_t >& xy ) const;

};

}
