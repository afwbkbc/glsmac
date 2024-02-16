#pragma once

#include <string>

#include "types/Serializable.h"

#include "types/Color.h"

namespace game {
namespace rules {

CLASS( Faction, types::Serializable )

	Faction();
	Faction( const std::string& id, const std::string& name, const types::Color& color );

	std::string m_id = "";
	std::string m_name = "";
	types::Color m_color = {};

	const types::Buffer Serialize() const override;
	void Unserialize( types::Buffer buf ) override;
};

}
}
