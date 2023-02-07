#pragma once

#include "UIObject.h"

#include "types/Sound.h"
#include "scene/actor/Sound.h"

namespace ui {
namespace object {

CLASS( SoundEffect, UIObject )

	SoundEffect( const string& class_name = "" );

	void SetSound( const types::Sound* sound );
	void SetVolume( const float volume );
	void SetAutoPlay( const bool autoplay );
	void SetRepeatable( const bool repeatable );
	void SetStartDelay( const size_t start_delay );

	void Play();
	void Pause();
	void Stop();
	
	void Create();
	void Iterate();
	void Destroy();
	
protected:
	
	virtual void ApplyStyle();
	
	const types::Sound* m_sound = nullptr;

	float m_volume = 1.0;
	bool m_autoplay = false;
	bool m_repeatable = false;
	size_t m_start_delay = 0;
	
	vector< actor::Sound* > m_actors = {};
};

}
}
