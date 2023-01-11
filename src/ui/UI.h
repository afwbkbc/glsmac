#pragma once

#include "base/Module.h"

#include <string>
#include <unordered_map>

#include "object/Root.h"
#include "scene/Scene.h"
#include "util/Clamper.h"

#include "event/UIEvent.h"

#include "scene/actor/MeshActor.h"
#include "scene/mesh/vec2/Rectangle.h"
#include "types/Texture.h"
#include "types/Vec2.h"

using namespace std;
using namespace scene;
using namespace types;

namespace ui {

using namespace object;
	
MAJOR_CLASS( UI, base::Module )

	typedef float coord_t;
	typedef Vec2<coord_t> vertex_t;

	void Start();
	void Stop();
	void Iterate();
	
	void AddObject( object::UIObject *object );
	void RemoveObject( object::UIObject *object );
	
	Scene *GetShapeScene();
	Scene *GetTextScene();
	
	const coord_t ClampX( const coord_t value ) const;
	const coord_t ClampY( const coord_t value ) const;
	
	void Resize();
	
	void SendEvent( const event::UIEvent* event );
	
	void SendMouseMoveEvent( UIObject* object );
	void ShowDebugFrame( const UIObject* object );
	void HideDebugFrame( const UIObject* object );
	void ResizeDebugFrame( const UIObject* object );
	
protected:
	object::Root m_root_object;

	Scene *m_shape_scene;
	Scene *m_text_scene;

	struct {
		util::Clamper<coord_t> x;
		util::Clamper<coord_t> y;
	} m_clamp;
	
	Vec2<size_t> m_last_mouse_position = { 0, 0 };
	
private:
	
	// debug stuff
	
	Scene *m_debug_scene;
	
	typedef struct {
		Texture* texture;
		mesh::vec2::Rectangle * mesh;
		actor::MeshActor* actor;
	} debug_frame_data_t;
	
	typedef unordered_map< const UIObject*, debug_frame_data_t > debug_frames_map_t;
	debug_frames_map_t m_debug_frames;
	
	void ResizeDebugFrame( const UIObject* object, const debug_frame_data_t* data );
};

} /* namespace ui */
