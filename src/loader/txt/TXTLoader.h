#pragma once

#include <unordered_map>
#include <string>

#include "loader/Loader.h"

namespace loader {
namespace txt {

CLASS( TXTLoader, Loader )

protected:

	struct txt_data_t {
		typedef std::vector< std::string > lines_t;
		typedef std::unordered_map< std::string, std::vector< std::string > > sections_t;
		txt_data_t( const std::string& source );
		lines_t lines = {};
		sections_t sections = {};
	};

	std::unordered_map< std::string, txt_data_t > m_txt_data = {};

	const txt_data_t& GetTXTData( const std::string& path );
};

}
}
