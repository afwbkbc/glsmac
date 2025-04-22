#include "Event.h"

#include "game/backend/Game.h"
#include "gse/value/Object.h"

namespace game {
namespace backend {
namespace event {

Event::Event( Game* game, const size_t caller, GSE_CALLABLE, const std::string& name, const gse::value::object_properties_t& data )
	: gc::Object( gc_space )
	, m_game( game )
	, m_caller( caller )
	, m_id( game->GenerateEventId() )
	, m_name( name )
	, m_original_data( data ) {
	UpdateData( GSE_CALL );
}

const types::Buffer Event::Serialize() const {
	types::Buffer buf;

	buf.WriteString( m_name );
	for ( const auto& it : m_data ) {
		buf.WriteString( it.first );
		it.second->Serialize( &buf, it.second );
	}

	return buf;
}

void Event::Unserialize( GSE_CALLABLE, types::Buffer buffer ) {
	m_name = buffer.ReadString();

	m_data.clear();
	const auto sz = buffer.ReadInt();
	for ( auto i = 0 ; i < sz ; i++ ) {
		const auto k = buffer.ReadString();
		m_data.insert( { k, gse::Value::Unserialize( GSE_CALL, &buffer ) } );
	}
}

#if defined(DEBUG) || defined(FASTDEBUG)
const std::string Event::ToString() {
	return "Event#" + m_id + "( " + m_name + " )"; // TODO
}
#endif

void Event::GetReachableObjects( std::unordered_set< gc::Object* >& reachable_objects ) {
	gc::Object::GetReachableObjects( reachable_objects );

	GC_DEBUG_BEGIN( "Event" );

	GC_DEBUG_BEGIN( "data" );
	for ( const auto& it : m_data ) {
		GC_REACHABLE( it.second );
	}
	GC_DEBUG_END();

	GC_DEBUG_END();
}

const std::string& Event::GetName() const {
	return m_name;
}

const gse::value::object_properties_t& Event::GetData() const {
	return m_data;
}

void Event::UpdateData( GSE_CALLABLE ) {
	m_data = {
		{ "game", m_game->Wrap( GSE_CALL, true ) },
		{ "caller", VALUE( gse::value::Int, , m_caller ) },
		{ "data",   VALUE( gse::value::Object, , GSE_CALL_NOGC, m_original_data ) },
	};
}

}
}
}
