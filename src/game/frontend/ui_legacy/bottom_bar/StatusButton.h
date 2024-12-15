#pragma once

#include "ui_legacy/object/Button.h"

#include "game/backend/turn/Types.h"

#include "util/Timer.h"

namespace ui_legacy::object {
class SoundEffect;
}

namespace game {
namespace frontend {

class Game;

namespace ui_legacy {

CLASS( StatusButton, ::ui_legacy::object::Button )

	StatusButton( Game* game );

	void Create() override;
	void Iterate() override;
	void Destroy() override;

	void SetStatus( const backend::turn::turn_status_t status );

private:
	const size_t FLASH_INTERVAL_MS = 10;

	Game* m_game;

	backend::turn::turn_status_t m_status_type = backend::turn::TS_NONE;

	util::Timer m_flash_timer;
	float m_flash_alpha = 0.0f;
	float m_flash_alpha_target = 0.0f;
	float m_flash_alpha_step = 0.0f;
	bool m_flash_reverse = false;

	bool m_is_clickable = false;
	::ui_legacy::object::Surface* m_flashing = nullptr;
	::ui_legacy::object::SoundEffect* m_sound = nullptr;

	void StartFlashing( const float alpha_per_sec, const float alpha_max );
	void StopFlashing();

};

}
}
}
