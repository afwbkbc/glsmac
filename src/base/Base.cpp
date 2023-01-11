#include "Base.h"

#include "engine/Engine.h"

namespace base {

std::atomic<size_t> g_next_object_id;

Base::Base()
	: m_object_id( g_next_object_id++ )
{
	//
}

Base::~Base() {
	//
};

const std::string Base::GetNamespace() const {
	return "";
}

const std::string Base::GetName() const {
	if ( !m_name.empty() )
		return GetNamespace() + m_name + "::#" + std::to_string(m_object_id);
	else
		return GetNamespace() + "#" + std::to_string(m_object_id);
}

const std::string& Base::GetLocalName() const {
	return m_name;
}

void Base::Log( const std::string &text ) const {
	if ( g_engine != NULL )
		g_engine->GetLogger()->Log( "<" + GetName() + "> " + text );
}


} /* namespace base */
