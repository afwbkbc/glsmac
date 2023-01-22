#pragma once

#include "base/Module.h"

#include <string>
#include <unordered_map>

#include "object/Root.h"
#include "scene/Scene.h"
#include "util/Clamper.h"

#include "event/UIEvent.h"
#include "event/UIEventHandler.h"

#include "scene/actor/Mesh.h"
#include "scene/mesh/Rectangle.h"
#include "types/Texture.h"
#include "types/Vec2.h"
#include "util/Timer.h"

#include "theme/Theme.h"

using namespace std;
using namespace scene;
using namespace types;

namespace ui {

using namespace event;
using namespace object;
	
CLASS( UI, base::Module )

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
	
	void ProcessEvent( UIEvent* event );
	void SendMouseMoveEvent( UIObject* object );
	const UIEventHandler* AddGlobalEventHandler( const UIEvent::event_type_t event_type, const UIEventHandler::handler_function_t& handler );
	void RemoveGlobalEventHandler( const UIEventHandler* event_handler );
	
	void SetTheme( theme::Theme* theme );
	void UnsetTheme();
	
	const theme::Style* GetStyle( const string& style_class ) const;
	
#if DEBUG
	void ShowDebugFrame( const UIObject* object );
	void HideDebugFrame( const UIObject* object );
	void ResizeDebugFrame( const UIObject* object );
#else
	void ShowDebugFrame( const UIObject* object ) {}
	void HideDebugFrame( const UIObject* object ) {}
	void ResizeDebugFrame( const UIObject* object ) {}
#endif
	
protected:
	object::Root m_root_object;

	Scene *m_shape_scene = nullptr;
	Scene *m_text_scene = nullptr;

	struct {
		util::Clamper<coord_t> x;
		util::Clamper<coord_t> y;
	} m_clamp = {};
	
	Vec2<size_t> m_last_mouse_position = { 0, 0 };
	
	const theme::Theme* GetTheme() const;
	theme::Theme* m_theme = nullptr;
	
private:

#if DEBUG	
	Scene *m_debug_scene;
	
	typedef struct {
		Texture* texture;
		mesh::Rectangle * mesh;
		actor::Mesh* actor;
	} debug_frame_data_t;
	
	typedef unordered_map< const UIObject*, debug_frame_data_t > debug_frames_map_t;
	debug_frames_map_t m_debug_frames = {};
	
	void ResizeDebugFrame( const UIObject* object, const debug_frame_data_t* data );
#endif
	
};

} /* namespace ui */
