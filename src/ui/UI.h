#pragma once

#include <unordered_map>

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

namespace geometry {
class Geometry;
}

namespace dom {
class Root;
}

class Class;

CLASS2( UI, common::Class, gse::Wrappable )

	UI( GSE_CALLABLE );
	~UI();

	WRAPDEFS_PTR( UI );

	scene::Scene* const m_scene = nullptr;

	const types::mesh::coord_t ClampX( const coord_t& x ) const;
	const types::mesh::coord_t ClampY( const coord_t& y ) const;
	const types::Vec2< types::mesh::coord_t > ClampXY( const types::Vec2< coord_t >& xy ) const;

	ui::Class* const GetClass( const std::string& name ) const;

private:
	dom::Root* m_root;

	struct {
		util::Clamper< float > x;
		util::Clamper< float > y;
	} m_clamp = {};

	std::unordered_map< std::string, ui::Class* > m_classes = {};

	void Resize();

};

}
