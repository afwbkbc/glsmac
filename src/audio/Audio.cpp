#include "Audio.h"

#include "scene/actor/Sound.h"

namespace audio {

Audio::~Audio() {
	// force delete all
	for ( auto& actor : m_removal_queue ) {
		DELETE( actor );
	}
}

void Audio::RemoveAndDeleteActor( scene::actor::Sound* actor ) {
	if ( actor->IsReadyToBeDeleted() ) {
		RemoveActor( actor );
		DELETE( actor );
	}
	else {
		ASSERT( find( m_removal_queue.begin(), m_removal_queue.end(), actor ) == m_removal_queue.end(), "actor already in removal queue" );
		m_removal_queue.push_back( actor );
	}
}

void Audio::Iterate() {
	for ( auto it = m_removal_queue.begin() ; it != m_removal_queue.end() ; it++ ) {
		scene::actor::Sound* actor = *it;
		if ( actor->IsReadyToBeDeleted() ) {
			RemoveActor( actor );
			DELETE( actor );
			m_removal_queue.erase( it );
			it--;
		}
	}
}

}
