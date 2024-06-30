#pragma once

#include "UIObject.h"

namespace types {
class Sound;
}

namespace scene::actor {
class Sound;
}

namespace ui {
namespace object {

CLASS( SoundEffect, UIObject )

	SoundEffect( const std::string& class_name = "" );

	void SetSound( const types::Sound* sound );
	void SetVolume( const float volume );
	void SetAutoPlay( const bool autoplay );
	void SetAutoStop( const bool autostop );
	void SetRepeatable( const bool repeatable );
	void SetStartDelay( const size_t start_delay );

	void Play();
	void Pause();
	void Stop();

	void Create() override;
	void Iterate() override;
	void Destroy() override;

protected:

	virtual void ApplyStyle() override;

	const types::Sound* m_sound = nullptr;

	float m_volume = 1.0;
	bool m_autoplay = false;
	bool m_autostop = false;
	bool m_repeatable = false;
	size_t m_start_delay = 0;

	std::vector< scene::actor::Sound* > m_actors = {};
};

}
}
