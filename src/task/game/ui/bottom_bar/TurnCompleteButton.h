#pragma once

#include "ui/object/Button.h"

#include "util/Timer.h"
#include "ui/object/SoundEffect.h"

namespace task {
namespace game {
namespace ui {

CLASS( TurnCompleteButton, ::ui::object::Button )

	TurnCompleteButton();

	void Create() override;
	void Iterate() override;
	void Destroy() override;

	void SetTurnCompleteStatus( const bool is_turn_complete );

private:
	const size_t FLASH_INTERVAL_MS = 10;
	const float FLASH_ALPHA_MIN = 0.0f;
	const float FLASH_ALPHA_MAX = 1.0f;
	const float FLASH_ALPHA_STEP = 0.0125f;
	util::Timer m_flash_timer;
	float m_flash_alpha = 0.0f;
	bool m_flash_reverse = false;

	::ui::object::Surface* m_flashing = nullptr;
	::ui::object::SoundEffect* m_sound = nullptr;

};

}
}
}
