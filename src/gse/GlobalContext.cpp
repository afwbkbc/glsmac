#include "GlobalContext.h"

#include "util/FS.h"
#include "util/String.h"

namespace gse {

GlobalContext::GlobalContext( GSE* gse, const std::string& source_path )
	: Context( gse )
	, m_script_info(
		{
			source_path,
			util::FS::GetBaseName( source_path ),
			util::FS::GetDirName( source_path ),
		}
	)
	, m_source_lines(
		source_path.empty()
			? source_lines_t{}
			: util::String::SplitToLines( util::FS::ReadFile( source_path ) )
	) {}

Context* GlobalContext::GetParentContext() const {
	return nullptr;
}

Context* GlobalContext::GetCallerContext() const {
	return nullptr;
}

const bool GlobalContext::IsTraceable() const {
	return false;
}

static const std::string s_empty = "";
const std::string& GlobalContext::GetSourceLine( const size_t line_num ) const {
	if ( line_num == 0 ) {
		return s_empty;
	}
	ASSERT_NOLOG( line_num <= m_source_lines.size(), "source line overflow" );
	return m_source_lines.at( line_num - 1 );
}

const si_t& GlobalContext::GetSI() const {
	THROW( "global contexts are not supposed to have si" );
}

const Context::script_info_t& GlobalContext::GetScriptInfo() const {
	return m_script_info;
}

void GlobalContext::AddSourceLine( const std::string& source_line ) {
	m_source_lines.push_back( source_line );
}

void GlobalContext::AddSourceLines( const source_lines_t& source_lines ) {
	m_source_lines.insert( m_source_lines.begin(), source_lines.begin(), source_lines.end() );
}

}
