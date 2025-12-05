#include "Event.h"

#include "game/backend/Game.h"
#include "gse/value/Object.h"

namespace game {
namespace backend {
namespace event {

Event::Event( Game* game, const source_t source, const size_t caller, GSE_CALLABLE, const std::string& name, const gse::value::object_properties_t& data, const std::string& id )
	: gc::Object( gc_space )
	, m_game( game )
	, m_source( source )
	, m_caller( caller )
	, m_id(
		id.empty()
			? game->GenerateEventId()
			: id
	)
	, m_name( name )
	, m_original_data( data ) {
	UpdateData( GSE_CALL );
}

const types::Buffer Event::Serialize() const {
	types::Buffer buf;

	buf.WriteString( m_id );
	buf.WriteString( m_name );
	buf.WriteInt( m_caller );
	buf.WriteInt( m_original_data.size() );
	for ( const auto& it : m_original_data ) {
		if ( it.second->type == gse::Value::T_OBJECT ) {
			ASSERT( ( (gse::value::Object*)it.second )->object_class.empty(), "custom object in event data: " + ( (gse::value::Object*)it.second )->object_class );
		}
		buf.WriteString( it.first );
		it.second->Serialize( &buf, it.second );
	}

	return buf;
}

Event* const Event::Unserialize( Game* const game, const source_t source, GSE_CALLABLE, types::Buffer buffer ) {
	const auto id = buffer.ReadString();
	const auto name = buffer.ReadString();
	const auto caller = buffer.ReadInt();
	gse::value::object_properties_t data = {};
	const auto sz = buffer.ReadInt();
	for ( auto i = 0 ; i < sz ; i++ ) {
		const auto k = buffer.ReadString();
		data.insert( { k, gse::Value::Unserialize( GSE_CALL, &buffer ) } );
	}
	return new Event( game, source, caller, GSE_CALL, name, data, id );
}

const std::string Event::ToString() const {
	return "Event#" + m_id + "( " + m_name + " )"; // TODO
}

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

const size_t Event::GetCaller() const {
	return m_caller;
}

const Event::source_t Event::GetSource() const {
	return m_source;
}

const std::string& Event::GetId() const {
	return m_id;
}

const std::string& Event::GetEventName() const {
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
