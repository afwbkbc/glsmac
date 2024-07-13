#pragma once

#include "common/Common.h"

namespace game {
namespace backend {

CLASS( Account, common::Class )

	Account();

	// getter-setter model because we want every update to be saved to file

	const std::string& GetGSID() const;
	const std::string& GetLastPlayerName() const;
	void SetLastPlayerName( const std::string& value );
	const std::string& GetLastGameName() const;
	void SetLastGameName( const std::string& value );
	const std::string& GetLastRemoteAddress() const;
	void SetLastRemoteAddress( const std::string& value );

private:

	// GLSMAC ID, used mainly for binding to slots in multiplayer games
	std::string m_gsid = "";

	// cache last values for menus like 'join game' / 'host game'
	struct {
		std::string player_name = "";
		std::string game_name = "";
		std::string remote_address = "";
	} m_last_values;

	const std::string GetPath() const;

	void Create();
	void Save();
	void Load();
};

}
}
