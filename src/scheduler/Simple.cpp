#include <algorithm>

#include "Simple.h"

#include "common/Task.h"

namespace scheduler {

Simple::Simple() {

}

Simple::~Simple() {
	for ( auto& task : m_tasks ) {
		DELETE( task );
	}
}

void Simple::Start() {
	m_active = true;
	for ( auto it = m_tasks.begin() ; it < m_tasks.end() ; it++ ) {
		Log( "Starting task [" + ( *it )->GetName() + "]" );
		( *it )->Start();
	}
#ifdef DEBUG
	m_timer.SetInterval( 1000 );
#endif
}

void Simple::Stop() {
	for ( auto it = m_tasks.begin() ; it < m_tasks.end() ; it++ ) {
		Log( "Stopping task [" + ( *it )->GetName() + "]" );
		( *it )->Stop();
	}
	m_active = false;
}

void Simple::Iterate() {
	for ( auto& task : m_tasks_toadd ) {
		Log( "Adding task " + task->GetName() );
		AddTask( task );
	}
	m_tasks_toadd.clear();
	m_iterating = true;
	for ( auto it = m_tasks.begin() ; it < m_tasks.end() ; it++ ) {
		( *it )->Iterate();
	}
	m_iterating = false;
	for ( auto& task : m_tasks_toremove ) {
		Log( "Removing task " + task->GetName() );
		RemoveTask( task );
	}
	m_tasks_toremove.clear();

#ifdef DEBUG
	if ( m_timer.HasTicked() ) {
		DEBUG_STAT_INC( seconds_passed );
	}
#endif
}

void Simple::AddTask( common::Task* task ) {
	if ( m_iterating ) {
		m_tasks_toadd.push_back( task );
	}
	else {
		m_tasks.push_back( task );
		if ( m_active ) {
			Log( "Starting task [" + task->GetName() + "]" );
			task->Start();
		}
	}
}

void Simple::RemoveTask( common::Task* task ) {
	if ( m_iterating ) {
		m_tasks_toremove.push_back( task );
	}
	else {
		auto it = std::find( m_tasks.begin(), m_tasks.end(), task );
		ASSERT( it != m_tasks.end(), "removal of non-existent task " );
		if ( m_active ) {
			Log( "Stopping task [" + task->GetName() + "]" );
			task->Stop();
		}
		m_tasks.erase( it );
		DELETE( task );
	}
}

}
