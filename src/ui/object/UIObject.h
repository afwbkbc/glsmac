#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "base/Base.h"

#include "ui/event/UIEvent.h"

#include "scene/Scene.h"
#include "scene/actor/Actor.h"

#include "ui/event/UIEvent.h"
#include "ui/event/UIEventHandler.h"

#include "ui/theme/Style.h"

using namespace std;
using namespace types;

using namespace scene;
namespace scene {
using namespace actor;
}

namespace ui {

using namespace event;
using namespace theme;

namespace object {

class UIContainer;

CLASS( UIObject, base::Base )
	UIObject( const string& class_name = "" );
	virtual ~UIObject();

	typedef uint8_t alignment_t;
	const static alignment_t ALIGN_LEFT = 1;
	const static alignment_t ALIGN_RIGHT = 2;
	const static alignment_t ALIGN_HCENTER = 3;
	const static alignment_t ALIGN_TOP = 4;
	const static alignment_t ALIGN_BOTTOM = 8;
	const static alignment_t ALIGN_VCENTER = 12;
	const static alignment_t ALIGN_CENTER = 15;

	enum overflow_t {
		OVERFLOW_VISIBLE,
		OVERFLOW_HIDDEN
	};

	typedef float coord_t;
	typedef Vec2<coord_t> vertex_t;

	UIObject *GetParentObject() const;
	void SetParentObject( UIContainer *parent_object );
	virtual void Realign();
	virtual void Redraw();
	void UpdateObjectArea();

	void AddActor( Actor* actor );
	void RemoveActor( Actor* actor );
	void EnableActors();
	void DisableActors();

	virtual void Create();
	virtual void Destroy();
	virtual void Iterate();
	virtual void Align();
	virtual void Draw();

	size_t GetWidth() const;
	size_t GetHeight() const;
	
	virtual void SetZIndex( float z_index );
	void UpdateZIndex();
	void SetLeft( const coord_t px );
	void SetRight( const coord_t px );
	void SetTop( const coord_t px );
	void SetBottom( const coord_t px );
	void Maximize();
	void SetPadding( const coord_t px );
	void SetWidth( const coord_t px );
	void SetHeight( const coord_t px );
	void ForceAspectRatio( const float aspect_ratio );
	void SetAlign( const alignment_t alignment );
	void SetHAlign( const alignment_t alignment );
	void SetVAlign( const alignment_t alignment );

	const float GetZIndex() const { return m_z_index; };
	const overflow_t GetOverflow() const { return m_overflow; };
	
	enum event_type_t : uint8_t {
		EV_MOUSEOVER,
		EV_MOUSEOUT,
	};
	
	virtual void ProcessEvent( event::UIEvent* event );
	
	vertex_t GetAreaPosition() const;
	pair<vertex_t, vertex_t> GetAreaGeometry() const;
	bool IsPointInside( const size_t x, const size_t y ) const;
	
	void SetClass( const string& style );
	
#ifdef DEBUG
	void ShowDebugFrame();
	void HideDebugFrame();
#else
	void ShowDebugFrame() {}
	void HideDebugFrame() {}
#endif
	
	void ForwardStyleAttribute( const Style::attribute_type_t src_type, const Style::attribute_type_t dst_type );
	void ForwardStyleAttribute( const Style::attribute_type_t type );
	void ForwardStyleAttributesV( const vector< Style::attribute_type_t > type );
	void ForwardStyleAttributesM( const unordered_map< Style::attribute_type_t, Style::attribute_type_t > types );
	
	virtual void AddStyleModifier( const Style::modifier_t modifier );
	virtual void RemoveStyleModifier( const Style::modifier_t modifier );
	const bool HasStyleModifier( const Style::modifier_t modifier ) const;
	
	const UIEventHandler* On( const UIEvent::event_type_t type, UIEventHandler::handler_function_t func );
	void Off( const UIEventHandler* handler );
	bool Trigger( const UIEvent::event_type_t type, const UIEvent::event_data_t* data );
	
	virtual void BlockEvents();
	virtual void UnblockEvents();
	
	void SetFocusable( bool is_focusable );
	
	virtual void Focus();
	virtual void Defocus();
	
	typedef unordered_map< UIEvent::event_type_t, vector< UIEventHandler* > > event_handlers_t;
	
protected:
	friend class UIContainer;
	
	// callbacks. true if event is processed (then it won't be sent further)
	virtual bool OnMouseOver( const UIEvent::event_data_t* data ) { return true; };
	virtual bool OnMouseOut( const UIEvent::event_data_t* data ) { return true; };
	virtual bool OnMouseDown( const UIEvent::event_data_t* data ) { return true; };
	virtual bool OnMouseUp( const UIEvent::event_data_t* data ) { return true; };
	virtual bool OnMouseClick( const UIEvent::event_data_t* data ) { return true; };
	virtual bool OnKeyDown( const UIEvent::event_data_t* data ) { return true; };
	virtual bool OnKeyUp( const UIEvent::event_data_t* data ) { return true; };
	virtual bool OnKeyPress( const UIEvent::event_data_t* data ) { return true; };
	
	const coord_t ClampX( const coord_t value );
	const coord_t ClampY( const coord_t value );
	const vertex_t ClampXY( const vertex_t value );

	// bit flags
	typedef uint8_t event_context_t;
	static const event_context_t EC_NONE = 0;
	static const event_context_t EC_MOUSE = 1;
	static const event_context_t EC_MOUSEMOVE = 2; // separate because handling is very different
	static const event_context_t EC_KEYBOARD = 4;
	
	virtual void SetEventContexts( event_context_t contexts );
	virtual void SetOverriddenEventContexts( event_context_t contexts );
	bool HasEventContext( event_context_t context ) const;
	bool IsEventContextOverridden( event_context_t context ) const;
	
	UIContainer *m_parent_object = nullptr;

	unordered_set<Actor *> m_actors = {};

	size_t m_depth = 0;
	float m_z_index = 0.5f;
	
	float m_absolute_z_index = 0.0f;

	typedef struct object_area_struct {
		coord_t left;
		coord_t right;
		coord_t top;
		coord_t bottom;
		coord_t width;
		coord_t height;
		bool operator != ( const struct object_area_struct& other ) const {
			return memcmp( this, &other, sizeof(struct object_area_struct) ) != 0;
		}
	} object_area_t;
	object_area_t m_object_area = {};
	
	bool m_created = false;
	overflow_t m_overflow = OVERFLOW_VISIBLE;

	enum stick_bits_t {
		STICK_LEFT = 1,
		STICK_RIGHT = 2,
		STICK_WIDTH = 4,
		STICK_TOP = 8,
		STICK_BOTTOM = 16,
		STICK_HEIGHT = 32,
	};
	unsigned char m_stick_bits = 0;
	alignment_t m_align = ALIGN_LEFT | ALIGN_TOP;
	struct {
		coord_t left = 0;
		coord_t right = 0;
		coord_t top = 0;
		coord_t bottom = 0;
	} m_position = {};
	struct {
		coord_t width = 0;
		coord_t height = 0;
		coord_t aspect_ratio;
		bool force_aspect_ratio = false;
	} m_size = {};
	struct {
		coord_t left = 0;
		coord_t right = 0;
		coord_t top = 0;
		coord_t bottom = 0;
	} m_padding = {};
	
	typedef uint8_t state_t;
	const static state_t STATE_NONE = 0;
	const static state_t STATE_MOUSEOVER = 1;
	state_t m_state = STATE_NONE;
	
	scene::Scene *GetSceneOfActor( const Actor *actor ) const;
	
	virtual void ApplyStyle();
	virtual void ReloadStyle();
	
	void ForwardStyleAttribute( const Style::attribute_type_t src_type, const Style::attribute_type_t dst_type, UIObject* child );
	void ForwardStyleAttribute( const Style::attribute_type_t type, UIObject* child );
	
	bool Has( const Style::attribute_type_t attribute_type, const Style::modifier_t style_modifiers ) const;
	bool Has( const Style::attribute_type_t attribute_type ) const;
	const float Get( const Style::attribute_type_t attribute_type, const Style::modifier_t style_modifiers ) const;
	const float Get( const Style::attribute_type_t attribute_type ) const;
	const Color GetColor( const Style::attribute_type_t attribute_type, const Style::modifier_t style_modifiers ) const;
	const Color GetColor( const Style::attribute_type_t attribute_type ) const;
	const void* GetObject( const Style::attribute_type_t attribute_type, const Style::modifier_t style_modifiers ) const;
	const void* GetObject( const Style::attribute_type_t attribute_type ) const;
	
#ifdef DEBUG
	void CheckStylePtr() const;
	
	bool m_has_debug_frame = false;
#endif
	
	typedef unordered_set< UIEvent::event_type_t > events_t;
	
	void ListenToEvent( const UIEvent::event_type_t event );
	void ListenToEvents( const events_t& events );
	
	const string GetStyleModifiersString() const;
	
	void BlockRealigns();
	void UnblockRealigns();
	
private:
	
	bool m_is_focusable = false;
	bool m_is_focused = false;
	
	void SetAbsoluteZIndex( float z_index );
	
	bool m_are_realigns_blocked = false;
	bool m_are_events_blocked = false;
	
	event_context_t m_event_contexts = EC_NONE;
	event_context_t m_overridden_event_contexts = EC_NONE; // if parent captures any context - children can't have it until parent releases it
	
	events_t m_events_to_listen = {};
	
	string m_style_class = "";
	bool m_style_loaded = false; // will load on first draw
	const Style* m_style = nullptr;
	unordered_map< Style::attribute_type_t, Style::attribute_type_t > m_parent_style_attributes = {};
	void ApplyStyleIfNeeded();
	
	const Style::attribute_type_t GetParentAttribute( const Style::attribute_type_t source_type ) const;
	
	Style::modifier_t m_style_modifiers = Style::M_NONE;
	
	event_handlers_t m_event_handlers = {};
	
};

} /* namespace object */
} /* namespace ui */

#include "UIContainer.h"

