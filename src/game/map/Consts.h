#pragma once

#include <unordered_map>

#include "Types.h"
#include "game/settings/Types.h"
#include "game/map/tile/Types.h"

#include "types/Vec2.h"
#include "types/Vec3.h"
#include "types/Color.h"

#include "util/Clamper.h"

namespace game {
namespace map {

struct Consts {

	const struct {
		const struct {

			const types::Vec2< uint32_t > dimensions = {
				56,
				56
			};
			const types::Vec2< uint32_t > radius = dimensions / (uint32_t)2;

			const pcx_texture_coordinates_t water[2] = {
				{ 280, 79 },
				{ 280, 136 },
			};
			const pcx_texture_coordinates_t sunny_mesa[8] = {
				{ 768, 15 },
				{ 825, 15 },
				{ 882, 15 },
				{ 939, 15 },
				{ 768, 72 },
				{ 825, 72 },
				{ 882, 72 },
				{ 939, 72 },
			};
			const pcx_texture_coordinates_t rocks[4] = {
				{ 1,   1 },
				{ 58,  1 },
				{ 115, 1 },
				{ 172, 1 }
			};
			const pcx_texture_coordinates_t dunes[1] = {
				{
					229,
					1
				}
			};
			const pcx_texture_coordinates_t arid[1] = {
				{
					1,
					58
				}
			};
			const pcx_texture_coordinates_t moist[16] = {
				{ 1,   115 },
				{ 58,  115 },
				{ 115, 115 },
				{ 172, 115 },
				{ 1,   172 },
				{ 58,  172 },
				{ 115, 172 },
				{ 172, 172 },
				{ 1,   229 },
				{ 58,  229 },
				{ 115, 229 },
				{ 172, 229 },
				{ 1,   286 },
				{ 58,  286 },
				{ 115, 286 },
				{ 172, 286 },
			};
			const pcx_texture_coordinates_t rainy[16] = {
				{ 1,   343 },
				{ 58,  343 },
				{ 115, 343 },
				{ 172, 343 },
				{ 1,   400 },
				{ 58,  400 },
				{ 115, 400 },
				{ 172, 400 },
				{ 1,   457 },
				{ 58,  457 },
				{ 115, 457 },
				{ 172, 457 },
				{ 1,   514 },
				{ 58,  514 },
				{ 115, 514 },
				{ 172, 514 },
			};
			const pcx_texture_coordinates_t forest[16] = {
				{ 526, 6 },
				{ 583, 6 },
				{ 640, 6 },
				{ 697, 6 },
				{ 526, 63 },
				{ 583, 63 },
				{ 640, 63 },
				{ 697, 63 },
				{ 526, 120 },
				{ 583, 120 },
				{ 640, 120 },
				{ 697, 120 },
				{ 526, 177 },
				{ 583, 177 },
				{ 640, 177 },
				{ 697, 177 },
			};
			const pcx_texture_coordinates_t jungle[16] = {
				{ 526, 259 },
				{ 583, 259 },
				{ 640, 259 },
				{ 697, 259 },
				{ 526, 316 },
				{ 583, 316 },
				{ 640, 316 },
				{ 697, 316 },
				{ 526, 373 },
				{ 583, 373 },
				{ 640, 373 },
				{ 697, 373 },
				{ 526, 430 },
				{ 583, 430 },
				{ 640, 430 },
				{ 959, 463 },
			};
			const pcx_texture_coordinates_t fungus_land[16] = {
				{ 280, 516 },
				{ 337, 516 },
				{ 394, 516 },
				{ 451, 516 },
				{ 280, 573 },
				{ 337, 573 },
				{ 394, 573 },
				{ 451, 573 },
				{ 280, 630 },
				{ 337, 630 },
				{ 394, 630 },
				{ 451, 630 },
				{ 280, 687 },
				{ 337, 687 },
				{ 394, 687 },
				{ 451, 687 },
			};
			const pcx_texture_coordinates_t fungus_water[16] = {
				{ 508, 516 },
				{ 565, 516 },
				{ 622, 516 },
				{ 679, 516 },
				{ 508, 573 },
				{ 565, 573 },
				{ 622, 573 },
				{ 679, 573 },
				{ 508, 630 },
				{ 565, 630 },
				{ 622, 630 },
				{ 679, 630 },
				{ 508, 687 },
				{ 565, 687 },
				{ 622, 687 },
				{ 679, 687 },
			};
			const pcx_texture_coordinates_t river[16] = {
				{ 280, 259 },
				{ 337, 259 },
				{ 394, 259 },
				{ 451, 259 },
				{ 280, 316 },
				{ 337, 316 },
				{ 394, 316 },
				{ 451, 316 },
				{ 280, 373 },
				{ 337, 373 },
				{ 394, 373 },
				{ 451, 373 },
				{ 280, 430 },
				{ 337, 430 },
				{ 394, 430 },
				{ 451, 430 },
			};
			const pcx_texture_coordinates_t farm[9] = {
				{ 775, 219 },
				{ 832, 219 },
				{ 889, 219 },
				{ 775, 276 },
				{ 832, 276 },
				{ 889, 276 },
				{ 775, 333 },
				{ 832, 333 },
				{ 889, 333 },
			};
			const pcx_texture_coordinates_t road[9] = {
				{ 775, 395 },
				{ 832, 395 },
				{ 889, 395 },
				{ 775, 452 },
				{ 832, 452 },
				{ 889, 452 },
				{ 775, 509 },
				{ 832, 509 },
				{ 889, 509 },
			};
			const pcx_texture_coordinates_t mag_tube[9] = {
				{ 775, 566 },
				{ 832, 566 },
				{ 889, 566 },
				{ 775, 623 },
				{ 832, 623 },
				{ 889, 623 },
				{ 775, 680 },
				{ 832, 680 },
				{ 889, 680 },
			};
		} texture_pcx;
		const struct {

			const types::Vec2< uint32_t > dimensions = {
				100,
				62
			};
			const types::Vec2< uint32_t > center = {
				50,
				25
			};

			const pcx_texture_coordinates_t nutrient_bonus_water[2] = {
				{ 1,   253 },
				{ 102, 253 },
			};
			const pcx_texture_coordinates_t nutrient_bonus_land[2] = {
				{ 203, 253 },
				{ 304, 253 },
			};
			const pcx_texture_coordinates_t minerals_bonus_water[2] = {
				{ 1,   316 },
				{ 102, 316 },
			};
			const pcx_texture_coordinates_t minerals_bonus_land[2] = {
				{ 203, 316 },
				{ 304, 316 },
			};
			const pcx_texture_coordinates_t energy_bonus_water[2] = {
				{ 1,   379 },
				{ 102, 379 },
			};
			const pcx_texture_coordinates_t energy_bonus_land[2] = {
				{ 203, 379 },
				{ 304, 379 },
			};
			const pcx_texture_coordinates_t uranium[1] = {
				{
					822,
					253
				},
			};
			const pcx_texture_coordinates_t geothermal[1] = {
				{
					822,
					190
				},
			};
			const pcx_texture_coordinates_t unity_pod_water[3] = {
				{ 418, 379 },
				{ 620, 379 },
				{ 822, 379 },
			};
			const pcx_texture_coordinates_t unity_pod_land[3] = {
				{ 519, 379 },
				{ 721, 379 },
				{ 923, 379 },
			};
			const pcx_texture_coordinates_t farm_water[1] = {
				{
					607,
					190
				},
			};
			const pcx_texture_coordinates_t farm_land[4] = {
				{ 923, 453 },
				{ 923, 516 },
				{ 923, 579 },
				{ 923, 642 },
			};
			const pcx_texture_coordinates_t solar_water[1] = {
				{
					506,
					127
				},
			};
			const pcx_texture_coordinates_t solar_land[1] = {
				{
					607,
					127
				},
			};
			const pcx_texture_coordinates_t mine_water[1] = {
				{
					506,
					64
				},
			};
			const pcx_texture_coordinates_t mine_land[1] = {
				{
					607,
					64
				},
			};
			const pcx_texture_coordinates_t soil_enricher[4] = {
				{ 822, 453 },
				{ 822, 516 },
				{ 822, 579 },
				{ 822, 642 },
			};
			const pcx_texture_coordinates_t condenser[1] = {
				{
					506,
					253
				},
			};
			const pcx_texture_coordinates_t mirror[1] = {
				{
					607,
					253
				},
			};
			const pcx_texture_coordinates_t borehole[1] = {
				{
					708,
					253
				},
			};
			const pcx_texture_coordinates_t sensor[1] = {
				{
					708,
					316
				},
			};
			const pcx_texture_coordinates_t bunker[1] = {
				{
					506,
					316
				},
			};
			const pcx_texture_coordinates_t airbase[1] = {
				{
					607,
					316
				},
			};
			const pcx_texture_coordinates_t monolith[1] = {
				{
					304,
					0
				},
				//{ 304, 1 },
			};

		} ter1_pcx;
	} tc;
	const struct {
		const types::Vec3 scale = {
			1.0f,
			1.0f,
			2.0f
		};
		const types::Vec2< float > radius = {
			scale.x / 2,
			scale.y / 2
		};
		const float rotated_width = sqrt( pow( scale.x, 2 ) + pow( scale.y, 2 ) );
		const tile::elevation_t maximum_allowed_slope_elevation = 650; // TODO: fix black lines when texture is perpendicular to camera
		const struct {
			const float texture_edge_stretch_min = 0.05f;
			const float texture_edge_stretch_max = 0.15f;
			const float texture_center_stretch = 4.0f;
			const float center_coordinates_shift = 2.0f;
		} random;
		const util::Clamper< float > elevation_to_vertex_z = {
			{
				{
					tile::ELEVATION_MIN,
					tile::ELEVATION_MAX
				},
				{
					-scale.z,
					scale.z
				}
			}
		};
		const util::Clamper< float > elevation_to_water_r = {
			{
				{
					tile::ELEVATION_LEVEL_TRENCH,
					tile::ELEVATION_LEVEL_COAST
				},
				{
					0.6f,
					1.3f
				}
			}
		};
		const util::Clamper< float > elevation_to_water_g = {
			{
				{
					tile::ELEVATION_LEVEL_TRENCH,
					tile::ELEVATION_LEVEL_COAST
				},
				{
					0.6f,
					1.8f
				}
			}
		};
		const util::Clamper< float > elevation_to_water_b = {
			{
				{
					tile::ELEVATION_LEVEL_TRENCH,
					tile::ELEVATION_LEVEL_COAST
				},
				{
					0.8f,
					1.8f
				}
			}
		};
		const util::Clamper< float > elevation_to_water_a = {
			{
				{
					tile::ELEVATION_LEVEL_TRENCH,
					tile::ELEVATION_LEVEL_COAST
				},
				{
					1.0f,
					0.5f
				}
			}
		};
		const float water_level_z = elevation_to_vertex_z.Clamp( tile::ELEVATION_LEVEL_COAST ); // water is always on water level
	} tile;
	const struct {
		// to compensate for view angle difference from SMAC's
		const float y_scale = 0.8f;
	} sprite;
	const types::Vec3 map_position = {
		0.0f,
		0.0f,
		0.0f
	};
	const types::Vec3 map_rotation = {
		0.0f,
		0.0f,
		0.0f
	};
	const types::Color underwater_tint = {
		0.0f,
		0.2f,
		0.5f,
		1.0f
	};
	const struct {
		//const types::Color coastline_tint = { 0.7f, 0.7f, 0.7f, 1.0f };
		const types::Color coastline_tint = {
			1.0f,
			1.0f,
			1.0f,
			1.0f
		};
		const float coast_water_alpha = 0.4f;
		const float coast_water_center_alpha = coast_water_alpha / 1.5f;
		const float coast_water_center_alpha_corner_mod = 0.7f;
		const types::Color border_color = {
			0.425f,
			0.378f,
			0.311f,
			1.0f
		};
		const float border_alpha = 0.7f;
		const float border_size = 6.0f;
		const struct {
			const float range = 0.35f;
			const float frequency = 0.15f;
			const uint8_t passes = 4;
			const float cut = 0.3f;
			const float round_range = 2.0f;
		} perlin;
	} coastlines;
	const struct {
		const std::string default_map_directory = "maps";
		const std::string default_map_filename = "untitled";
		const std::string default_map_extension = ".gsm";
	} fs;
	const std::unordered_map< settings::map_config_value_t, types::Vec2< size_t > > map_sizes = {
		// original SMAC sizes (1:1)
		/*
		{ settings::MAP_CONFIG_TINY, { 48, 48 } },
		{ settings::MAP_CONFIG_SMALL, { 64, 64 } },
		{ settings::MAP_CONFIG_STANDARD, { 80, 80 } },
		{ settings::MAP_CONFIG_LARGE, { 100, 100 } },
		{ settings::MAP_CONFIG_HUGE, { 128, 128 } }
		 */
		// 2:1 sizes (better for minimap and solves horizontal repeat problem for widescreens)
		{ settings::MAP_CONFIG_TINY,     { 68,  34 } },
		{ settings::MAP_CONFIG_SMALL,    { 88,  44 } },
		{ settings::MAP_CONFIG_STANDARD, { 112, 56 } },
		{ settings::MAP_CONFIG_LARGE,    { 140, 70 } },
		{ settings::MAP_CONFIG_HUGE,     { 180, 90 } }
	};
#ifdef DEBUG
	const struct {
		const std::string lastseed_filename = "lastmap.seed";
		const std::string lastmap_filename = "lastmap.gsm";
		const std::string lastdump_filename = "lastmap.gsmd";
	} debug;
#endif
};
extern Consts s_consts;

}

}
