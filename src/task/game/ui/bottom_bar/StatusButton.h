#pragma once

#include "ui/object/Button.h"

#include "util/Timer.h"
#include "ui/object/SoundEffect.h"
#include "game/Turn.h"

namespace task {
namespace game {
class Game;
namespace ui {

CLASS( StatusButton, ::ui::object::Button )

	StatusButton( Game* game );

	void Create() override;
	void Iterate() override;
	void Destroy() override;

	void SetStatus( const ::game::Turn::turn_status_t status );

private:
	const size_t FLASH_INTERVAL_MS = 10;

	Game* m_game;

	::game::Turn::turn_status_t m_status_type = ::game::Turn::TS_NONE;

	util::Timer m_flash_timer;
	float m_flash_alpha = 0.0f;
	float m_flash_alpha_target = 0.0f;
	float m_flash_alpha_step = 0.0f;
	bool m_flash_reverse = false;

	bool m_is_clickable = false;
	::ui::object::Surface* m_flashing = nullptr;
	::ui::object::SoundEffect* m_sound = nullptr;

	void StartFlashing( const float alpha_per_sec, const float alpha_max );
	void StopFlashing();

};

}
}
}
