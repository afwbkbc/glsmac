#pragma once

#include <vector>

#include "Context.h"

namespace gse {

class GSE;

namespace context {

class GlobalContext : public Context {
public:

	typedef std::vector< std::string > source_lines_t;

	GlobalContext( GSE* gse, const std::string& source_path );

	Context* GetParentContext() const override;
	const bool IsTraceable() const override;
	const std::string& GetSourceLine( const size_t line_num ) const override;
	const si_t& GetSI() const override;
	const script_info_t& GetScriptInfo() const override;

	void AddSourceLine( const std::string& source_line );
	void AddSourceLines( const source_lines_t& source_lines );
private:
	const script_info_t m_script_info;
	source_lines_t m_source_lines;

};

}
}
