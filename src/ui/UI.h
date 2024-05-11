#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <vector>

#include "base/Module.h"

#include "Types.h"
#include "event/Types.h"

#include "util/Clamper.h"
#include "types/Vec2.h"
#include "types/Matrix44.h"

namespace types {
namespace texture {
class Texture;
}
namespace mesh {
class Mesh;
class Rectangle;
}
}

namespace scene {
class Scene;
namespace actor {
class Mesh;
}
}

namespace ui {

namespace module {
class Module;
class Error;
class Loader;
}
namespace object {
class Root;
class Popup;
class UIObject;
}
namespace event {
class UIEvent;
class UIEventHandler;
}
namespace theme {
class Theme;
class Style;
}

CLASS( UI, base::Module )

	typedef float coord_t;

	void Start() override;
	void Stop() override;
	void Iterate() override;

	void AddObject( object::UIObject* object );
	void RemoveObject( object::UIObject* object );

	scene::Scene* GetShapeScene( const types::mesh::Mesh* mesh );
	scene::Scene* GetTextScene();

	void SetWorldUIMatrix( const types::Matrix44& matrix );
	const types::Matrix44& GetWorldUIMatrix() const;

	const coord_t ClampX( const coord_t value ) const;
	const coord_t ClampY( const coord_t value ) const;
	const coord_t UnclampX( const coord_t value ) const;
	const coord_t UnclampY( const coord_t value ) const;

	void ProcessEvent( event::UIEvent* event );
	void SendMouseMoveEvent( object::UIObject* object );

	enum global_event_handler_order_t {
		GH_BEFORE, // will be called before UI elements
		GH_AFTER, // will be called after UI elements
	};
	const event::UIEventHandler* AddGlobalEventHandler( const event::event_type_t event_type, const event::handler_function_t& func, const global_event_handler_order_t order );
	void RemoveGlobalEventHandler( const event::UIEventHandler* handler );

	void AddTheme( theme::Theme* theme );
	void RemoveTheme( theme::Theme* theme );

	const theme::Style* GetStyle( const std::string& style_class ) const;

	void AddIterativeObject( void* object, const ui_handler_t handler );
	void RemoveIterativeObject( void* object );

	// no direct access to ui modules because they may have been already destroyed while other thread tries to call them

	void ShowError( const std::string& text, const ui_handler_t on_close = UH() {} ) const;
	void HideError() const;

	typedef std::function< bool() > loader_cancel_handler_t;
	void ShowLoader( const std::string& text, const loader_cancel_handler_t on_cancel = 0 ) const;
	void SetLoaderText( const std::string& text, bool is_cancelable = true ) const;
	void HideLoader() const;

	void BlockEvents();
	void UnblockEvents();

	void AddToFocusableObjects( object::UIObject* object );
	void RemoveFromFocusableObjects( object::UIObject* object );

	void FocusObject( object::UIObject* object );

	void Redraw();

	bool HasPopup() const;
	void OpenPopup( object::Popup* popup );
	void ClosePopup( object::Popup* popup, bool force = false );
	void CloseLastPopup( bool force = false );

	// popups
	void Error( const std::string& text );
	void Confirm( const std::string& text, const ui_handler_t on_confirm );

#ifdef DEBUG

	void ShowDebugFrame( object::UIObject* object );
	void HideDebugFrame( object::UIObject* object );
	void ResizeDebugFrame( object::UIObject* object );

#else
	void ShowDebugFrame( const object::UIObject* object ) {}
	void HideDebugFrame( const object::UIObject* object ) {}
	void ResizeDebugFrame( const object::UIObject* object ) {}
#endif

protected:
	friend class module::Module;
	void ActivateModule( module::Module* module );
	void DeactivateModule( module::Module* module );

private:
	object::Root* m_root_object;

	scene::Scene* m_shape_scene_simple2d = nullptr;
	scene::Scene* m_shape_scene_ortho = nullptr;

	struct {
		util::Clamper< coord_t > x;
		util::Clamper< coord_t > y;
	} m_clamp = {};

	types::Vec2< ssize_t > m_last_mouse_position = {
		0,
		0
	};

	typedef std::unordered_set< theme::Theme* > themes_t;
	const themes_t GetThemes() const;
	themes_t m_themes = {};

	bool m_is_redraw_needed = false;

	types::Matrix44 m_game_ui_matrix = {};

	std::unordered_map< global_event_handler_order_t, event::event_handlers_t > m_global_event_handlers = {};
	void TriggerGlobalEventHandlers( global_event_handler_order_t order, event::UIEvent* event );

	std::vector< object::UIObject* > m_focusable_objects_order = {};
	std::unordered_set< object::UIObject* > m_focusable_objects = {};
	object::UIObject* m_focused_object = nullptr;
	void UpdateFocusableObjectsOrder();
	void FocusNextObject();

	module::Module* m_active_module = nullptr;

	module::Error* m_error = nullptr;
	module::Loader* m_loader = nullptr;

	std::unordered_map< void*, ui_handler_t > m_iterative_objects = {};
	std::vector< void* > m_iterative_objects_to_remove = {};

	std::vector< object::Popup* > m_popups = {};
	std::vector< object::Popup* > m_popups_close_queue = {};

	std::unordered_set< object::UIObject* > m_offclick_aware_objects = {};

#ifdef DEBUG

	scene::Scene* m_debug_scene;

	struct debug_frame_data_t {
		types::texture::Texture* texture;
		types::mesh::Rectangle* mesh;
		scene::actor::Mesh* actor;
	};

	typedef std::unordered_map< object::UIObject*, debug_frame_data_t > debug_frames_map_t;
	debug_frames_map_t m_debug_frames = {};

	void ResizeDebugFrame( object::UIObject* object, const debug_frame_data_t* data );

#endif

};

}
