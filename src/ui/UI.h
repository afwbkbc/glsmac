#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <vector>

#include "base/Module.h"

#include "object/Root.h"
#include "scene/Scene.h"
#include "util/Clamper.h"

#include "event/UIEvent.h"
#include "event/UIEventHandler.h"

#include "scene/actor/Mesh.h"
#include "types/mesh/Rectangle.h"
#include "types/Texture.h"
#include "types/Vec2.h"
#include "types/Matrix44.h"
#include "util/Timer.h"

#include "theme/Theme.h"

#include "object/Popup.h"
#include "object/Surface.h"

#include "module/Loader.h"

using namespace scene;
using namespace types;

namespace ui {

typedef std::function<void()> object_iterate_handler_t;
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
	
	Scene *GetShapeScene( const types::mesh::Mesh* mesh );
	Scene *GetTextScene();
	
	void SetWorldUIMatrix( const types::Matrix44& matrix );
	const types::Matrix44& GetWorldUIMatrix() const;
	
	const coord_t ClampX( const coord_t value ) const;
	const coord_t ClampY( const coord_t value ) const;
	const coord_t UnclampX( const coord_t value ) const;
	const coord_t UnclampY( const coord_t value ) const;
	
	void ProcessEvent( UIEvent* event );
	void SendMouseMoveEvent( UIObject* object );
	
	enum global_event_handler_order_t {
		GH_BEFORE, // will be called before UI elements
		GH_AFTER, // will be called after UI elements
	};
	const UIEventHandler* AddGlobalEventHandler( const UIEvent::event_type_t event_type, const UIEventHandler::handler_function_t& func, const global_event_handler_order_t order );
	void RemoveGlobalEventHandler( const UIEventHandler* handler );
	
	void AddTheme( theme::Theme* theme );
	void RemoveTheme( theme::Theme* theme );
	
	const theme::Style* GetStyle( const std::string& style_class ) const;
	
	void AddIterativeObject( void* object, const object_iterate_handler_t handler );
	void RemoveIterativeObject( void* object );
	
	module::Loader* GetLoader() const;
	
	void BlockEvents();
	void UnblockEvents();
	
	void AddToFocusableObjects( UIObject* object );
	void RemoveFromFocusableObjects( UIObject* object );
	
	void FocusObject( UIObject* object );
	
	void Redraw();
	
	bool HasPopup() const;
	void OpenPopup( Popup* popup );
	void ClosePopup( Popup* popup, bool force = false );
	void CloseLastPopup( bool force = false );
	
#ifdef DEBUG
	void ShowDebugFrame( UIObject* object );
	void HideDebugFrame( UIObject* object );
	void ResizeDebugFrame( UIObject* object );
#else
	void ShowDebugFrame( const UIObject* object ) {}
	void HideDebugFrame( const UIObject* object ) {}
	void ResizeDebugFrame( const UIObject* object ) {}
#endif
	
private:
	object::Root m_root_object;

	Scene *m_shape_scene_simple2d = nullptr;
	Scene *m_shape_scene_ortho = nullptr;

	struct {
		util::Clamper<coord_t> x;
		util::Clamper<coord_t> y;
	} m_clamp = {};
	
	Vec2<ssize_t> m_last_mouse_position = { 0, 0 };
	
	typedef std::unordered_set< theme::Theme* > themes_t;
	const themes_t GetThemes() const;
	themes_t m_themes = {};
	
	bool m_is_redraw_needed = false;
	
	types::Matrix44 m_world_ui_matrix = {};

	std::unordered_map< global_event_handler_order_t, UIObject::event_handlers_t > m_global_event_handlers = {};
	void TriggerGlobalEventHandlers( global_event_handler_order_t order, UIEvent* event );
	
	std::vector< UIObject* > m_focusable_objects_order = {};
	std::unordered_set< UIObject* > m_focusable_objects = {};
	UIObject* m_focused_object = nullptr;
	void UpdateFocusableObjectsOrder();
	void FocusNextObject();
	
	module::Loader* m_loader = nullptr;
	
	std::unordered_map< void*, object_iterate_handler_t > m_iterative_objects = {};
	std::vector< void* > m_iterative_objects_to_remove = {};
	
	const UIEventHandler* m_keydown_handler = nullptr;
	
	std::vector< Popup* > m_popups = {};
	
#ifdef DEBUG	
	Scene *m_debug_scene;
	
	struct debug_frame_data_t {
		Texture* texture;
		mesh::Rectangle * mesh;
		actor::Mesh* actor;
	};
	
	typedef std::unordered_map< UIObject*, debug_frame_data_t > debug_frames_map_t;
	debug_frames_map_t m_debug_frames = {};
	
	void ResizeDebugFrame( UIObject* object, const debug_frame_data_t* data );
#endif
	
};

} /* namespace ui */
