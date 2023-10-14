#pragma once

#include "Connection.h"

namespace game {
namespace connection {

CLASS( Client, Connection )

	Client( LocalSettings* const settings );

	std::function< void() > m_on_players_list_update = nullptr;
	std::function< void( const game_state_t game_state ) > m_on_game_state_change = nullptr;
	std::function< void( const float progress ) > m_on_map_progress = nullptr; // progress is from 0.0f to 1.0f
	std::function< void( const std::string serialized_tiles ) > m_on_map_data = nullptr;

	void UpdateSlot( const size_t slot_num, Slot* slot ) override;
	void Message( const std::string& message ) override;

	const game_state_t GetGameState() const;
	void RequestMap();

	void ResetHandlers() override;

protected:
	void ProcessEvent( const network::Event& event ) override;

private:
	void Error( const std::string& reason );

	struct {
		bool is_downloading = false;
		int total_size = 0;
		int downloaded_size = 0;
		std::string buffer = "";
	} m_map_download_state = {};
	void RequestNextMapChunk();
};

}
}
