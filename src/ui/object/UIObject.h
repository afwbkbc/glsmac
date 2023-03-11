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
	UIObject( const std::string& class_name = "" );
	virtual ~UIObject();

	typedef uint8_t alignment_t;
	const static alignment_t ALIGN_NONE = 0;
	const static alignment_t ALIGN_LEFT = 1 << 0;
	const static alignment_t ALIGN_RIGHT = 1 << 1;
	const static alignment_t ALIGN_HCENTER = ALIGN_LEFT | ALIGN_RIGHT;
	const static alignment_t ALIGN_TOP = 1 << 2;
	const static alignment_t ALIGN_BOTTOM = 1 << 3;
	const static alignment_t ALIGN_VCENTER = ALIGN_TOP | ALIGN_BOTTOM;
	const static alignment_t ALIGN_CENTER = ALIGN_HCENTER | ALIGN_VCENTER;

	enum overflow_t {
		OVERFLOW_VISIBLE, // Allow children to be rendered outside of this object (default)
		OVERFLOW_HIDDEN, // Hide parts of children that are outside of this object
		OVERFLOW_GROW // Grow automatically when children are added
	};

	typedef float coord_t;
	typedef Vec2< coord_t > vertex_t;

	UIObject *GetParentObject() const;
	void SetParentObject( UIContainer *parent_object );
	
	virtual void RealignNow(); // slow, use only if up-to-date state is needed immediately
	void Realign(); // use in all other cases
	virtual void RealignMaybe(); // realign only if needed, to be called from UI
	
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

	const coord_t GetLeft() const;
	const coord_t GetRight() const;
	const coord_t GetTop() const;
	const coord_t GetBottom() const;
	const coord_t GetWidth() const;
	const coord_t GetHeight() const;
	
	virtual void SetZIndex( float z_index );
	void UpdateZIndex();
	void SetLeft( const coord_t px );
	void SetRight( const coord_t px );
	void SetTop( const coord_t px );
	void SetBottom( const coord_t px );
	
	struct coord_box_t {
		coord_t left;
		coord_t top;
		coord_t right;
		coord_t bottom;
		bool operator != ( const coord_box_t& other ) const {
			return memcmp( this, &other, sizeof( other ) ) != 0;
		}
		operator bool() const {
			return left || top || right || bottom;
		}
	};

	void Maximize();
	void SetMargin( const coord_box_t px );
	void SetMargin( const coord_t px );
	
	void SetOverflowMargin( const coord_t px );
	virtual void SetWidth( const coord_t px );
	virtual void SetHeight( const coord_t px );
	virtual void ForceAspectRatio( const float aspect_ratio );
	void SetAlign( const alignment_t alignment );
	void SetHAlign( const alignment_t alignment );
	void SetVAlign( const alignment_t alignment );
	void SetOverflow( const overflow_t overflow );

	const float GetZIndex() const;
	const overflow_t GetOverflow() const;
	
	enum event_type_t : uint8_t {
		EV_MOUSEOVER,
		EV_MOUSEOUT,
	};
	
	virtual void ProcessEvent( event::UIEvent* event );
	
	struct object_area_t {
		coord_t left;
		coord_t right;
		coord_t top;
		coord_t bottom;
		coord_t width;
		coord_t height;
		bool operator != ( const object_area_t& other ) const {
			return memcmp( this, &other, sizeof( other ) ) != 0;
		}
	};
	
	vertex_t GetAreaPosition() const;
	virtual const object_area_t GetObjectArea();
	virtual const object_area_t GetInternalObjectArea(); // for UIContainer
	bool IsPointInside( const size_t x, const size_t y ) const;
	
	void SetClass( const std::string& style );
	
#ifdef DEBUG
	void ShowDebugFrame();
	void HideDebugFrame();
	
	// use these to track lifecycle of specific object
	void SetTesting( const bool testing );
	const bool IsTesting() const;
	void TestBreakpoint(); // set gdb breakpoint inside
#else
	void ShowDebugFrame() {}
	void HideDebugFrame() {}
#endif
	
	void ForwardStyleAttribute( const Style::attribute_type_t src_type, const Style::attribute_type_t dst_type );
	void ForwardStyleAttribute( const Style::attribute_type_t type );
	void ForwardStyleAttributesV( const std::vector< Style::attribute_type_t > type );
	void ForwardStyleAttributesM( const std::unordered_map< Style::attribute_type_t, Style::attribute_type_t > types );
	
	virtual void AddStyleModifier( const Style::modifier_t modifier );
	virtual void RemoveStyleModifier( const Style::modifier_t modifier );
	const bool HasStyleModifier( const Style::modifier_t modifier ) const;
	
	const UIEventHandler* On( const std::vector< UIEvent::event_type_t >& types, UIEventHandler::handler_function_t func );
	const UIEventHandler* On( const UIEvent::event_type_t type, UIEventHandler::handler_function_t func );
	void Off( const UIEventHandler* handler );
	bool Trigger( const UIEvent::event_type_t type, const UIEvent::event_data_t* data );
	
	virtual void BlockEvents();
	virtual void UnblockEvents();
	
	void SetFocusable( bool is_focusable );
	
	void Select();
	void Deselect();
	
	virtual void Focus();
	virtual void Defocus();
	
	virtual void Show();
	virtual void Hide();
	
	virtual void SetAreaLimits( const coord_box_t limits );
	virtual void SetAreaLimitsByObject( UIObject* source_object ); // make sure source object lives longer than this one!
	void AddAreaLimitsChild( UIObject* child_object );
	void RemoveAreaLimitsChild( UIObject* child_object );
	
	typedef std::unordered_map< UIEvent::event_type_t, std::vector< UIEventHandler* > > event_handlers_t;
	
	// bit flags
	typedef uint8_t event_context_t;
	static constexpr event_context_t EC_NONE = 0;
	static constexpr event_context_t EC_MOUSE = 1;
	static constexpr event_context_t EC_MOUSEMOVE = 2; // separate because handling is very different
	static constexpr event_context_t EC_KEYBOARD = 4;
	
	virtual void SetEventContexts( event_context_t contexts );
	virtual void AddEventContexts( event_context_t contexts );
	
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
	
	const coord_t UnclampX( const coord_t value );
	const coord_t UnclampY( const coord_t value );

	virtual void SetOverriddenEventContexts( event_context_t contexts );
	virtual void AddOverriddenEventContexts( event_context_t contexts );
	bool HasEventContext( event_context_t context ) const;
	bool IsEventContextOverridden( event_context_t context ) const;
	
	UIContainer *m_parent_object = nullptr;

	std::vector<Actor*> m_actors = {};

	size_t m_depth = 0;
	float m_z_index = 0.5f;
	
	float m_absolute_z_index = 0.0f;

	object_area_t m_object_area = {};
	
	bool m_created = false;
	overflow_t m_overflow = OVERFLOW_VISIBLE;

	enum stick_bits_t {
		STICK_NONE = 0,
		STICK_LEFT = 1 << 0,
		STICK_RIGHT = 1 << 1,
		STICK_WIDTH = 1 << 2,
		STICK_TOP = 1 << 3,
		STICK_BOTTOM = 1 << 4,
		STICK_HEIGHT = 1 << 5,
	};
	unsigned char m_stick_bits = STICK_NONE;
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
	
	bool m_is_testing = false;
#endif
	
	typedef std::unordered_set< UIEvent::event_type_t > events_t;
	
	void ListenToEvent( const UIEvent::event_type_t event );
	void ListenToEvents( const events_t& events );
	
	const std::string GetStyleModifiersString() const;
	
	void BlockRealigns();
	void UnblockRealigns();
	
	bool m_is_realign_needed = true;
	
	const std::string& GetStyleClass();

	coord_box_t m_margin = {};
	coord_t m_overflow_margin = 0;
	
	// redraws overlay
	void Refresh();
	
private:
	
	bool m_is_focusable = false;
	bool m_is_focused = false;
	
	bool m_is_visible = true;
	
	void SetAbsoluteZIndex( float z_index );
	
	bool m_are_realigns_blocked = false;
	bool m_are_events_blocked = false;
	
	event_context_t m_event_contexts = EC_NONE;
	event_context_t m_overridden_event_contexts = EC_NONE; // if parent captures any context - children can't have it until parent releases it
	
	events_t m_events_to_listen = {};
	
	std::string m_style_class = "";
	bool m_style_loaded = false; // will load on first draw
	const Style* m_style = nullptr;
	std::unordered_map< Style::attribute_type_t, Style::attribute_type_t > m_parent_style_attributes = {};
	void ApplyStyleIfNeeded();
	
	const Style::attribute_type_t GetParentAttribute( const Style::attribute_type_t source_type ) const;
	
	Style::modifier_t m_style_modifiers = Style::M_NONE;
	
	event_handlers_t m_event_handlers = {};

	struct {
		bool enabled = false;
		UIObject* source_object = nullptr;
		std::unordered_set< UIObject* > child_objects = {};
		coord_box_t limits;
	} m_area_limits = {};
	void UpdateAreaLimits();
	
};

} /* namespace object */
} /* namespace ui */

#include "UIContainer.h"
