#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <cstring>

#include "common/Common.h"

#include "Types.h"
#include "ui/event/Types.h"
#include "ui/theme/Types.h"

#include "types/Vec2.h"
#include "types/Color.h"

namespace scene {
class Scene;
namespace actor {
class Actor;
}
}

namespace ui {

class UI;
namespace event {
class UIEvent;
class UIEventHandler;
}
namespace theme {
class Style;
}

namespace object {

class UIContainer;

CLASS( UIObject, common::Class )
	UIObject( const std::string& class_name = "" );
	virtual ~UIObject();

	typedef float coord_t;
	typedef types::Vec2< coord_t > vertex_t;

	UIObject* GetParentObject() const;
	void SetParentObject( UIContainer* parent_object );

	virtual void RealignNow(); // slow, use only if up-to-date state is needed immediately
	void Realign(); // use in all other cases
	virtual void RealignMaybe(); // realign only if needed, to be called from UI

	virtual void Redraw();
	void UpdateObjectArea();

	void AddActor( scene::actor::Actor* actor );
	void RemoveActor( scene::actor::Actor* actor );
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
	virtual void UpdateZIndex();
	void SetLeft( const coord_t px );
	void SetRight( const coord_t px );
	void SetTop( const coord_t px );
	void SetBottom( const coord_t px );

	struct coord_box_t {
		coord_t left;
		coord_t top;
		coord_t right;
		coord_t bottom;

		bool operator!=( const coord_box_t& other ) const {
			return memcmp( this, &other, sizeof( other ) ) != 0;
		}

		operator bool() const {
			return left || top || right || bottom;
		}
	};

	void Maximize();
	void SetMargin( const coord_box_t px );
	void SetMargin( const coord_t px );
	const coord_box_t& GetMargin() const;

	void SetOverflowMargin( const coord_t px );
	virtual void SetWidth( const coord_t px );
	virtual void SetHeight( const coord_t px );
	virtual void ForceAspectRatio( const float aspect_ratio );
	void SetAlign( const alignment_t alignment );
	void SetHAlign( const alignment_t alignment );
	void SetVAlign( const alignment_t alignment );
	virtual void SetOverflow( const overflow_t overflow );

	const float GetZIndex() const;
	const overflow_t GetOverflow() const;

	virtual void ProcessEvent( event::UIEvent* event );

	struct object_area_t {
		coord_t left;
		coord_t right;
		coord_t top;
		coord_t bottom;
		coord_t width;
		coord_t height;

		bool operator!=( const object_area_t& other ) const {
			return memcmp( this, &other, sizeof( other ) ) != 0;
		}
	};

	vertex_t GetAreaPosition() const;
	virtual const object_area_t GetObjectArea();
	virtual const object_area_t GetInternalObjectArea(); // for UIContainer
	bool IsPointInside( const ssize_t x, const ssize_t y ) const;

	void SetClass( const std::string& style );

#ifdef DEBUG

	void ShowDebugFrame();
	void HideDebugFrame();

#else
	void ShowDebugFrame() {}
	void HideDebugFrame() {}
#endif

	void SetParentStyleObject( const UIContainer* object );
	void ForwardStyleAttribute( const attribute_type_t src_type, const attribute_type_t dst_type );
	void ForwardStyleAttribute( const attribute_type_t type );
	void ForwardStyleAttributesV( const std::vector< attribute_type_t > type );
	void ForwardStyleAttributesM( const std::unordered_map< attribute_type_t, attribute_type_t > types );
	void ForwardAllStyleAttributes();

	virtual void AddStyleModifier( const modifier_t modifier );
	virtual void RemoveStyleModifier( const modifier_t modifier );
	const bool HasStyleModifier( const modifier_t modifier ) const;

	const event::UIEventHandler* On( const std::vector< event::event_type_t >& types, event::handler_function_t func );
	const event::UIEventHandler* On( const event::event_type_t type, event::handler_function_t func );
	void Off( const event::UIEventHandler* handler );
	bool Trigger( const event::event_type_t type, const event::event_data_t* data );

	virtual void BlockEvents();
	virtual void UnblockEvents();

	void SetFocusable( bool is_focusable );

	void Select();
	void Deselect();

	virtual void Focus();
	virtual void Defocus();

	virtual void Show();
	virtual void ShowActors();
	virtual void Hide();
	virtual void HideActors();
	const bool IsVisible() const;

	virtual void SetAreaLimits( const coord_box_t limits );
	virtual void SetAreaLimitsMaybe( const coord_box_t limits ); // sets limits if not limited by object(s)
	virtual void SetAreaLimitsByObject( UIObject* source_object ); // make sure source object lives longer than this one!
	void AddAreaLimitsChild( UIObject* child_object );
	void RemoveAreaLimitsChild( UIObject* child_object );
	virtual void ClearAreaLimits();

	// bit flags
	typedef uint8_t event_context_t;
	static constexpr event_context_t EC_NONE = 0;
	static constexpr event_context_t EC_MOUSE = 1 << 0;
	static constexpr event_context_t EC_MOUSEMOVE = 1 << 1; // separate because handling is very different
	static constexpr event_context_t EC_KEYBOARD = 1 << 2;
	static constexpr event_context_t EC_PARENTAREA = 1 << 3; // will use area of parent element instead of own on mouse events
	static constexpr event_context_t EC_OFFCLICK_AWARE = 1 << 4; // object will catch clicks outside of it

	virtual void SetEventContexts( event_context_t contexts );
	virtual void AddEventContexts( event_context_t contexts );

protected:
	friend class UIContainer;
	friend class ::ui::UI;

	bool HasEventContext( event_context_t context ) const;

protected:
	friend class UIContainer;

	// callbacks. true if event is processed (then it won't be sent further)
	// TODO: get rid of those
	virtual bool OnMouseOver( const event::event_data_t* data ) { return false; };
	virtual bool OnMouseOut( const event::event_data_t* data ) { return false; };
	virtual bool OnMouseDown( const event::event_data_t* data ) { return false; };
	virtual bool OnMouseUp( const event::event_data_t* data ) { return false; };
	virtual bool OnMouseClick( const event::event_data_t* data ) { return false; };
	virtual bool OnKeyDown( const event::event_data_t* data ) { return false; };
	virtual bool OnKeyUp( const event::event_data_t* data ) { return false; };
	virtual bool OnKeyPress( const event::event_data_t* data ) { return false; };

	const coord_t ClampX( const coord_t value );
	const coord_t ClampY( const coord_t value );
	const vertex_t ClampXY( const vertex_t value );

	const coord_t UnclampX( const coord_t value );
	const coord_t UnclampY( const coord_t value );

	virtual void SetOverriddenEventContexts( event_context_t contexts );
	virtual void AddOverriddenEventContexts( event_context_t contexts );
	bool IsEventContextOverridden( event_context_t context ) const;

	UIContainer* m_parent_object = nullptr;

	std::vector< scene::actor::Actor* > m_actors = {};

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

	scene::Scene* GetSceneOfActor( const scene::actor::Actor* actor ) const;

	void IgnoreStyleAttribute( const attribute_type_t type );
	virtual void ApplyStyle();
	virtual void ReloadStyle();

	void ApplyStyleIfNeeded();
	void ApplyStyleIfNotLoaded();

	void ForwardStyleAttribute( const attribute_type_t src_type, const attribute_type_t dst_type, UIObject* child );
	void ForwardStyleAttribute( const attribute_type_t type, UIObject* child );

	bool Has( const attribute_type_t attribute_type, const modifier_t style_modifiers ) const;
	bool Has( const attribute_type_t attribute_type ) const;
	const float Get( const attribute_type_t attribute_type, const modifier_t style_modifiers ) const;
	const float Get( const attribute_type_t attribute_type ) const;
	const types::Color GetColor( const attribute_type_t attribute_type, const modifier_t style_modifiers ) const;
	const types::Color GetColor( const attribute_type_t attribute_type ) const;
	const void* GetObject( const attribute_type_t attribute_type, const modifier_t style_modifiers ) const;
	const void* GetObject( const attribute_type_t attribute_type ) const;

#ifdef DEBUG

	void CheckStylePtr() const;
	bool m_has_debug_frame = false;

#endif

	typedef std::unordered_set< event::event_type_t > events_t;

	void ListenToEvent( const event::event_type_t event );
	void ListenToEvents( const events_t& events );

	const std::string GetStyleModifiersString() const;

	void BlockRealigns();
	void UnblockRealigns();

	bool m_is_realign_needed = true;

	const std::string& GetStyleClass();
	const std::string SubClass( const std::string& subclass );

	coord_box_t m_margin = {};
	coord_t m_overflow_margin = 0;

	// redraws overlay
	void Refresh();

private:

	bool m_is_focusable = false;
	bool m_is_focused = false;

	bool m_is_visible = true;
	bool m_is_actually_visible = true;

	void SetAbsoluteZIndex( float z_index );

	bool m_are_realigns_blocked = false;
	bool m_are_events_blocked = false;

	event_context_t m_event_contexts = EC_NONE;
	event_context_t m_overridden_event_contexts = EC_NONE; // if parent captures any context - children can't have it until parent releases it

	events_t m_events_to_listen = {};

	bool m_is_applying_style = false;
	std::string m_style_class = "";
	bool m_style_loaded = false; // will load on first draw
	const theme::Style* m_style = nullptr;
	const UIContainer* m_parent_style_object = nullptr;
	std::unordered_map< attribute_type_t, attribute_type_t > m_parent_style_attributes = {};
	bool m_forward_all_style_attributes = false;
	std::unordered_set< attribute_type_t > m_ignore_style_attributes = {};

	const attribute_type_t GetParentAttribute( const attribute_type_t source_type ) const;

	modifier_t m_style_modifiers = M_NONE;

	event::event_handlers_t m_event_handlers = {};

	struct {
		bool enabled = false;
		std::unordered_set< UIObject* > source_objects = {};
		std::unordered_set< UIObject* > child_objects = {};
		coord_box_t limits;
	} m_area_limits = {};
	void UpdateAreaLimits();

};

}
}
