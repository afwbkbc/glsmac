#pragma once

#include "base/Module.h"

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <functional>

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

#include "module/Loader.h"

using namespace std;
using namespace scene;
using namespace types;

namespace ui {

typedef function<void()> object_iterate_handler_t;
#define IH( ... ) [ __VA_ARGS__ ] () -> void
	
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
	
	void AddTheme( theme::Theme* theme );
	void RemoveTheme( theme::Theme* theme );
	
	const theme::Style* GetStyle( const string& style_class ) const;
	
	void AddIterativeObject( void* object, const object_iterate_handler_t handler );
	void RemoveIterativeObject( void* object );
	
	module::Loader* GetLoader() const;
	
	void BlockEvents();
	void UnblockEvents();
	
	void AddToFocusableObjects( UIObject* object );
	void RemoveFromFocusableObjects( UIObject* object );
	
	void FocusObject( UIObject* object );
	
#ifdef DEBUG
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
	
	typedef unordered_set< theme::Theme* > themes_t;
	const themes_t GetThemes() const;
	themes_t m_themes = {};
	
private:

	vector< UIObject* > m_focusable_objects_order = {};
	unordered_set< UIObject* > m_focusable_objects = {};
	UIObject* m_focused_object = nullptr;
	void UpdateFocusableObjectsOrder();
	void FocusNextObject();
	
	module::Loader* m_loader = nullptr;
	
	unordered_map< void*, object_iterate_handler_t > m_iterative_objects = {};
	vector< void* > m_iterative_objects_to_remove = {};
	
#ifdef DEBUG	
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
