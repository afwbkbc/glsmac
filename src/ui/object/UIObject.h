#pragma once

#include <unordered_set>

#include "base/Base.h"

#include "ui/event/UIEvent.h"

#include "scene/Scene.h"
#include "scene/actor/Actor.h"

#include "ui/event/UIEvent.h"

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

MAJOR_CLASS( UIObject, base::Base )
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
	
	virtual void SendEvent( const event::UIEvent* event );
	
	vertex_t GetAreaPosition() const;
	pair<vertex_t, vertex_t> GetAreaGeometry() const;
	bool IsPointInside( const size_t x, const size_t y ) const;
	
	void SetClass( const string& style );
	
#if DEBUG
	void ShowDebugFrame();
	void HideDebugFrame();
#else
	void ShowDebugFrame() {}
	void HideDebugFrame() {}
#endif
	
protected:
	
	// callbacks
	virtual void OnMouseOver( const UIEvent::event_data_t* data ) {};
	virtual void OnMouseOut( const UIEvent::event_data_t* data ) {};
	virtual void OnMouseDown( const UIEvent::event_data_t* data ) {};
	virtual void OnKeyDown( const UIEvent::event_data_t* data ) {};
	virtual void OnClick( const UIEvent::event_data_t* data ) {};
	
	const coord_t ClampX( const coord_t value );
	const coord_t ClampY( const coord_t value );
	const vertex_t ClampXY( const vertex_t value );

	bool m_is_focused = false;
	bool m_is_focusable = false;
	
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
		coord_t left;
		coord_t right;
		coord_t top;
		coord_t bottom;
	} m_position;
	struct {
		coord_t width;
		coord_t height;
		coord_t aspect_ratio;
		bool force_aspect_ratio;
	} m_size;
	struct {
		coord_t left;
		coord_t right;
		coord_t top;
		coord_t bottom;
	} m_padding;
	
	typedef uint8_t state_t;
	const static state_t STATE_NONE = 0;
	const static state_t STATE_MOUSEOVER = 1;
	state_t m_state = STATE_NONE;
	
	scene::Scene *GetSceneOfActor( const Actor *actor ) const;
	
	virtual void ApplyStyle();
	
	bool Has( const Style::attribute_type_t attribute_type ) const;
	const ssize_t Get( const Style::attribute_type_t attribute_type ) const;
	const Color GetColor( const Style::attribute_type_t attribute_type ) const;
	const void* GetObject( const Style::attribute_type_t attribute_type ) const;
	
#if DEBUG
	void CheckStylePtr() const;
	
	bool m_has_debug_frame = false;
#endif
	
private:
	string m_style_class = "";
	bool m_style_loaded = false; // will load on first draw
	const Style* m_style = nullptr;
	
	void ApplyStyleIfNeeded();
	
	Style::modifier_t m_style_modifiers = Style::M_NONE;
	void AddStyleModifier( const Style::modifier_t modifier );
	void RemoveStyleModifier( const Style::modifier_t modifier );
	
};

} /* namespace object */
} /* namespace ui */

#include "UIContainer.h"

