#pragma once

#include <vector>

#include "base/Base.h"

#include "scene/Scene.h"
#include "scene/actor/Actor.h"

namespace ui {
namespace object {

class UIContainer;

MAJOR_CLASS( UIObject, base::Base )
	UIObject();

	typedef unsigned char alignment_t;
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
	typedef types::Vec2<coord_t> vertex_t;

	UIObject *GetParentObject() const;
	void SetParentObject( UIContainer *parent_object );
	virtual void Realign();
	virtual void Redraw();
	void UpdateObjectArea();

	void CreateActors();
	void DestroyActors();

	virtual void Create();
	virtual void Destroy();
	virtual void Align();
	virtual void Draw();

	virtual void SetZIndex( float z_index );
	void SetLeft( const coord_t px );
	void SetRight( const coord_t px );
	void SetTop( const coord_t px );
	void SetBottom( const coord_t px );
	void SetWidth( const coord_t px );
	void SetHeight( const coord_t px );
	void SetAlign( const alignment_t alignment );
	void SetHAlign( const alignment_t alignment );
	void SetVAlign( const alignment_t alignment );

	const float GetZIndex() const { return m_z_index; };
	const overflow_t GetOverflow() const { return m_overflow; };
	/*virtual const coord_t GetLeft() const = 0;
	virtual const coord_t GetTop() const = 0;
	virtual const coord_t GetRight() const = 0;
	virtual const coord_t GetBottom() const = 0;
	coord_t GetWidth() const { return ( abs( GetRight() - GetLeft() ) ); }
	coord_t GetHeight() const { return ( abs( GetBottom() - GetTop() ) ); }*/
protected:
	const coord_t ClampX( const coord_t value );
	const coord_t ClampY( const coord_t value );
	const vertex_t ClampXY( const vertex_t value );

	UIContainer *m_parent_object = NULL;

	std::vector<scene::actor::Actor *> m_actors;

	float m_z_index = 0.0f;

	struct {
		coord_t left;
		coord_t right;
		coord_t top;
		coord_t bottom;
		coord_t width;
		coord_t height;
	} m_object_area;
	bool m_created = false;
	overflow_t m_overflow = OVERFLOW_VISIBLE;
private:
	scene::Scene *GetSceneOfActor( const scene::actor::Actor *actor ) const;
	struct {
		coord_t left;
		coord_t right;
		coord_t top;
		coord_t bottom;
	} m_position;
	struct {
		coord_t width;
		coord_t height;
	} m_size;
	struct {
		coord_t left;
		coord_t right;
		coord_t top;
		coord_t bottom;
	} m_padding;
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
};

} /* namespace object */
} /* namespace ui */

#include "UIContainer.h"

