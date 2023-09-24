#pragma once

#include "UIContainer.h"

#include "Surface.h"
#include "SimpleButton.h"

namespace ui {
namespace object {

CLASS( Scrollbar, UIContainer )

	enum scrollbar_direction_t {
		SD_VERTICAL,
		SD_HORIZONTAL
	};

	Scrollbar( const scrollbar_direction_t& direction, const std::string& class_name = "" )
		: UIContainer( class_name )
		, m_direction( direction ) {}

	void Create() override;
	void Iterate() override;
	void Align() override;
	void Destroy() override;

	void SetPercentage( const float percentage );
	const float GetPercentage() const;

private:

	struct {
		size_t frequency_ms = 100;
		util::Timer timer;
		int8_t value = 0;
	} m_arrow_scroll;
	void ProcessArrowScroll();

	struct {
		bool is_dragging = false;
		Vec2< ssize_t > drag_rel_position = {
			0,
			0
		};
		const event::UIEventHandler* on_mouse_move = nullptr;
		const event::UIEventHandler* on_mouse_up = nullptr;
	} m_slider_scroll;
	void UpdatePercentage();

	float m_percentage = 0.0f; // percentage of scroll (0 to 100)
	void RealignSlider();

	const scrollbar_direction_t m_direction;

	Surface* m_background = nullptr;
	SimpleButton* m_up = nullptr;
	SimpleButton* m_slider = nullptr;
	SimpleButton* m_down = nullptr;

};

}
}
