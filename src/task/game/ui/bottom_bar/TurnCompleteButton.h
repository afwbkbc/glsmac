#pragma once

#include "ui/object/Button.h"

#include "util/Timer.h"
#include "ui/object/SoundEffect.h"

namespace task {
namespace game {
class Game;
namespace ui {

CLASS( TurnCompleteButton, ::ui::object::Button )

	TurnCompleteButton( Game* game );

	void Create() override;
	void Iterate() override;
	void Destroy() override;

	void SetTurnActiveStatus( const bool is_turn_active );
	void SetFlashingEnabled( const bool is_flashing_enabled, const bool play_sound );

private:
	Game* m_game;

	bool m_is_turn_active;

	const size_t FLASH_INTERVAL_MS = 10;
	const float FLASH_ALPHA_MIN = 0.0f;
	const float FLASH_ALPHA_MAX_ACTIVE = 1.0f;
	const float FLASH_ALPHA_MAX_INACTIVE = 0.25f;
	const float FLASH_ALPHA_STEP_ACTIVE = 0.0125f;
	const float FLASH_ALPHA_STEP_INACTIVE = 0.003125f;
	util::Timer m_flash_timer;
	float m_flash_alpha = 0.0f;
	bool m_flash_reverse = false;

	::ui::object::Surface* m_flashing = nullptr;
	::ui::object::SoundEffect* m_sound = nullptr;

};

}
}
}
