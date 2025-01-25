#include "Surface.h"

#include "types/mesh/Rectangle.h"
#include "scene/actor/Mesh.h"
#include "ui/geometry/Rectangle.h"
#include "engine/Engine.h"
#include "loader/texture/TextureLoader.h"
#include "graphics/Graphics.h"
#include "types/texture/Texture.h"
#include "util/String.h"
#include "types/texture/Filter.h"

namespace ui {
namespace dom {

Surface::Surface( DOM_ARGS )
	: Area( DOM_ARGS_PASS, "surface" ) {

	NEW( m_mesh, types::mesh::Rectangle );
	m_geometry->AsRectangle()->SetMesh( m_mesh );
	m_actor = new scene::actor::Mesh( "UI::Surface", m_mesh );
	Actor( m_actor );

	Property(
		ctx, call_si, "background", gse::type::Type::T_STRING, VALUE( gse::type::Undefined ), PF_NONE,
		[ this ]( GSE_CALLABLE, const gse::Value& v ) {
			const auto& str = ( (gse::type::String*)v.Get() )->value;
			if ( str.empty() ) {
				throw gse::Exception( gse::EC.INVALID_ASSIGNMENT, "Property 'background' expects color code or texture path", ctx, call_si );
			}
			bool is_tiled = true;
			types::Color color = {};
			if ( TryParseColor( ctx, call_si, str, color ) ) {
				CreateTexture();
				m_background.texture->Resize( 1, 1 );
				m_background.texture->SetPixel( 0, 0, color );
				is_tiled = false;
			}
			else {
				// texture format examples:
				//   texture.pcx
				//   texture.pcx:filter1(arg1,arg2,...):filter2():...
				const auto params = util::String::Split( str, ':' );
				ASSERT_NOLOG( !params.empty(), "texture params empty" );
				auto* texture = g_engine->GetTextureLoader()->TryLoadCustomTexture( params.front() );
				if ( !texture ) {
					GSE_ERROR( gse::EC.LOADER_ERROR, "Could not load texture: " + params.front() );
				}
				types::texture::Filter filter( texture );
				std::vector< std::string > args = {};
				for ( auto it = params.begin() + 1 ; it != params.end() ; it++ ) {
					const auto brackets_begin = it->find( '(' );
					args = {};
					std::string f;
					if ( brackets_begin != std::string::npos ) {
						const auto brackets_end = it->rfind( ')' );
						if ( brackets_end == std::string::npos || brackets_end <= brackets_begin ) {
							GSE_ERROR( gse::EC.UI_ERROR, "Texture filter syntax error (unclosed bracket): " + *it );
						}
						args = util::String::Split( it->substr( brackets_begin + 1, brackets_end - brackets_begin - 1 ), ',' );
						f = it->substr( 0, brackets_begin );
					}
					else {
						f = *it;
					}

					const auto f_expect_args_fromto = [ &ctx, &call_si, &f, &args, &it ]( const uint8_t args_min, const uint8_t args_max ) {
						if ( args.size() < args_min || args.size() > args_max ) {
							GSE_ERROR( gse::EC.UI_ERROR, "Texture filter '" + f + "' expects " + ( args_min == args_max
								? std::to_string( args_min )
								: std::to_string( args_min ) + " to " + std::to_string( args_max )
							) + " parameters, got " + std::to_string( args.size() ) + ": " + *it );
						}
					};
					const auto f_expect_args = [ & f_expect_args_fromto ]( const uint8_t args ) {
						return f_expect_args_fromto( args, args );
					};

					if ( f == "stretch" ) {
						f_expect_args( 0 );
						is_tiled = false;
					}
					else if ( f == "tint" ) {
						f_expect_args( 2 );
						types::Color color;
						float intensity;
						ParseColor( ctx, call_si, args.at( 0 ), color );
						if ( !util::String::ParseFloat( args.at( 1 ), intensity ) ) {
							GSE_ERROR( gse::EC.UI_ERROR, "Invalid tint parameters (expected float, got " + args.at( 1 ) + "): " + *it );
						}
						if ( intensity < 0.0f || intensity > 1.0f ) {
							GSE_ERROR( gse::EC.UI_ERROR, "Invalid tint parameters (intensity must be between 0.0 and 1.0):" + *it );
						}
						filter.Tint( color, intensity );
					}
					else if ( f == "crop" ) {
						f_expect_args( 4 );
						long int x1, y1, x2, y2;
						if (
							!util::String::ParseInt( args.at( 0 ), x1 ) ||
								!util::String::ParseInt( args.at( 1 ), y1 ) ||
								!util::String::ParseInt( args.at( 2 ), x2 ) ||
								!util::String::ParseInt( args.at( 3 ), y2 )
							) {
							GSE_ERROR( gse::EC.UI_ERROR, "Invalid crop parameters (expected x1,y1,x2,y2, got " + args.at( 0 ) + "," + args.at( 1 ) + "," + args.at( 2 ) + "," + args.at( 3 ) + "): " + *it );
						}
						if ( x1 < 0 ) {
							GSE_ERROR( gse::EC.UI_ERROR, "Invalid crop parameters (x1 must be positive): " + *it );
						}
						if ( y1 < 0 ) {
							GSE_ERROR( gse::EC.UI_ERROR, "Invalid crop parameters (y1 must be positive): " + *it );
						}
						if ( x2 < x1 ) {
							GSE_ERROR( gse::EC.UI_ERROR, "Invalid crop parameters (x2 can't be less than x1): " + *it );
						}
						if ( y2 < y1 ) {
							GSE_ERROR( gse::EC.UI_ERROR, "Invalid crop parameters (y1 can't be less than y1): " + *it );
						}
						if ( x2 >= filter.GetWidth() ) {
							GSE_ERROR( gse::EC.UI_ERROR, "Invalid crop parameters (x2 must be less than texture width (" + std::to_string( filter.GetWidth() ) + ")): " + *it );
						}
						if ( y2 >= filter.GetHeight() ) {
							GSE_ERROR( gse::EC.UI_ERROR, "Invalid crop parameters (y2 must be less than texture height (" + std::to_string( filter.GetHeight() ) + ")): " + *it );
						}
						filter.Crop( x1, y1, x2, y2 );
					}
					else {
						GSE_ERROR( gse::EC.UI_ERROR, "Unknown texture filter: " + f );
					}
				}
				const auto result = filter.GetTexture();
				m_background.texture = result.texture;
				m_background.is_texture_owned = result.is_owned;
			}
			m_background.texture->FullUpdate(); // TODO: optimize?
			m_actor->SetTexture( m_background.texture );
			if ( is_tiled ) {
				m_geometry->AsRectangle()->SetTiled(
					{
						m_background.texture->m_width,
						m_background.texture->m_height
					}
				);
			}
			else {
				m_geometry->AsRectangle()->SetStretched();
			}
		},
		[ this ]( GSE_CALLABLE ) {
			ClearTexture();
		}
	);
}

Surface::~Surface() {
	if ( m_background.texture && m_background.is_texture_owned ) {
		delete m_background.texture;
	}
}

void Surface::CreateTexture() {
	if ( !m_background.texture || !m_background.is_texture_owned ) {
		m_background.texture = new types::texture::Texture();
		m_background.is_texture_owned = true;
		m_actor->SetTexture( m_background.texture );
	}
}

void Surface::ClearTexture() {
	m_actor->SetTexture( nullptr );
	if ( m_background.texture && m_background.is_texture_owned ) {
		delete m_background.texture;
	}
	m_background.texture = nullptr;
	m_background.is_texture_owned = false;
}

types::texture::Texture* Surface::GetOwnedTexturePtr() {
	if ( !m_background.texture ) {
		CreateTexture();
	}
	else {
		ASSERT_NOLOG( m_background.is_texture_owned, "GetOwnedTexturePtr on non-owned texture" );
	}
	return m_background.texture;
}

}
}
