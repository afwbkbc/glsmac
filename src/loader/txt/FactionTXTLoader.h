#pragma once

#include <unordered_map>

#include "TXTLoader.h"

namespace loader {
namespace txt {

CLASS( FactionTXTLoader, TXTLoader )

	struct faction_data_t {
		struct {
			std::vector< std::string > land;
			std::vector< std::string > water;
		} bases_names;
	};
	const faction_data_t& GetFactionData( resource::resource_t res );
	const faction_data_t& GetFactionData( const std::string& filename );

protected:

private:

	std::unordered_map< std::string, faction_data_t > m_faction_data = {};

	const faction_data_t& GetFactionDataImpl( const std::string& path );
	const std::vector< std::string >& GetSection( const txt_data_t& data, const std::string& name ) const;

};

}
}
