#include "Rules.h"

#include "gse/value/Array.h"

namespace game {
namespace backend {
namespace rules {

const int Rules::GetDefaultDifficultyLevelV() const {
	return m_difficulty_levels.GetValueUnsafe( GetDefaultDifficultyLevel() );
}

void Rules::Initialize() {
	if ( !m_is_initialized ) {
		InitRules();
		m_is_initialized = true;
	}
}

WRAPIMPL_DYNAMIC_GETTERS( Rules )
			WRAPIMPL_GET_CUSTOM( "difficulty_levels", Array, WrapDifficultyLevels( gc_space ) )
WRAPIMPL_DYNAMIC_SETTERS( Rules )
WRAPIMPL_DYNAMIC_ON_SET( Rules )
WRAPIMPL_DYNAMIC_END()

const types::Buffer Rules::Serialize() const {
	types::Buffer buf;

	// no need for now, difficulty levels are hardcoded
/*	buf.WriteInt( m_difficulty_levels.size() );
	for ( auto& it : m_difficulty_levels ) {
		buf.WriteString( it.first );
		buf.WriteString( it.second.Serialize().ToString() );
	}*/

	return buf;
}

void Rules::Unserialize( types::Buffer buf ) {

	// no need for now, difficulty levels are hardcoded
	/*
	m_difficulty_levels.clear();
	const size_t difficulty_levels_count = buf.ReadInt();
	for ( size_t i = 0 ; i < difficulty_levels_count ; i++ ) {
		const std::string difficulty_level_name = buf.ReadString();
		m_difficulty_levels[ difficulty_level_name ].Unserialize( buf.ReadString() );
	}*/

	m_is_initialized = true;

}

const gse::value::array_elements_t Rules::WrapDifficultyLevels( gc::Space* const gc_space ) {
	gse::value::array_elements_t result = {};
	const auto& vk = m_difficulty_levels.GetVK();
	std::map< int, std::string > kv = {};
	for ( const auto& it : vk ) {
		kv.insert( { it.first, it.second } );
	}
	result.reserve( kv.size() );
	for ( const auto& it : kv ) {
		result.push_back( VALUE( gse::value::String, , it.second ) ); // TODO: full object?
	}
	return result;
}

}
}
}
