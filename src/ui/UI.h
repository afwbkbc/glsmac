#pragma once

#include <unordered_map>
#include <functional>

#include "common/Common.h"
#include "gse/Wrappable.h"

#include "Types.h"

#include "gse/value/Object.h"
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

class Object;
}

class Class;

CLASS2( UI, common::Class, gse::Wrappable )

	UI( GSE_CALLABLE );
	~UI();

	void Iterate();

	WRAPDEFS_PTR( UI );

	scene::Scene* const m_scene = nullptr;

	const types::mesh::coord_t ClampX( const coord_t& x ) const;
	const types::mesh::coord_t ClampY( const coord_t& y ) const;
	const types::Vec2< types::mesh::coord_t > ClampXY( const types::Vec2< coord_t >& xy ) const;

	ui::Class* const GetClass( const std::string& name ) const;

	const types::Vec2< ssize_t >& GetLastMousePosition() const;

	void Destroy( GSE_CALLABLE );

private:
	friend class dom::Object;
	typedef std::function< void() > f_iterable_t;
	void AddIterable( const dom::Object* const obj, const f_iterable_t& f );
	void RemoveIterable( const dom::Object* const obj );

private:
	dom::Root* m_root;

	struct {
		util::Clamper< float > x;
		util::Clamper< float > y;
	} m_clamp = {};

	types::Vec2< ssize_t > m_last_mouse_position = {};

	std::unordered_map< std::string, ui::Class* > m_classes = {};

	void Resize();

	std::unordered_map< const dom::Object*, f_iterable_t > m_iterables = {};
};

}
