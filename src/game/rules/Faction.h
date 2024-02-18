#pragma once

#include <string>

#include "types/Serializable.h"

#include "types/Color.h"

#include "gse/Value.h"

namespace game {
namespace rules {

CLASS( Faction, types::Serializable )

	Faction();
	Faction( const std::string& id, const std::string& name );

	std::string m_id = "";
	std::string m_name = "";

	struct {
		types::Color text = {};
		types::Color border = {};
	} m_colors = {};

	void ImportPCX( const std::string& pcx_file );

	const types::Buffer Serialize() const override;
	void Unserialize( types::Buffer buf ) override;

	WRAPDEFS_PTR( Faction )

};

}
}
