#pragma once

#include "ui/object/Button.h"

#include "game/backend/turn/Types.h"

#include "util/Timer.h"

namespace ui::object {
class SoundEffect;
}

namespace task {
namespace game {
class Game;
namespace ui {

CLASS( StatusButton, ::ui::object::Button )

	StatusButton( Game* game );

	void Create() override;
	void Iterate() override;
	void Destroy() override;

	void SetStatus( const ::game::backend::turn::turn_status_t status );

private:
	const size_t FLASH_INTERVAL_MS = 10;

	Game* m_game;

	::game::backend::turn::turn_status_t m_status_type = ::game::backend::turn::TS_NONE;

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
