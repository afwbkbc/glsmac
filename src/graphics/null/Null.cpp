#include "Null.h"

namespace graphics {
namespace null {

void Null::AddScene( scene::Scene* scene ) {
	// nothing
}

void Null::RemoveScene( scene::Scene* scene ) {
	// nothing
}

const unsigned short Null::GetWindowWidth() const {
	return 0;
}

const unsigned short Null::GetWindowHeight() const {
	return 0;
}

const unsigned short Null::GetViewportWidth() const {
	return 0;
}

const unsigned short Null::GetViewportHeight() const {
	return 0;
}

void Null::LoadTexture( types::Texture* texture ) {
	// nothing
}

void Null::UnloadTexture( const types::Texture* texture ) {
	// nothing
}

void Null::EnableTexture( const types::Texture* texture ) {
	// nothing
}

void Null::DisableTexture() {
	// nothing
}

const bool Null::IsFullscreen() const {
	return false;
}

void Null::SetFullscreen() {
	// nothing
}

void Null::SetWindowed() {
	// nothing
}

void Null::RedrawOverlay() {
	// nothing
}

const bool Null::IsMouseLocked() const {
	return false;
}

void Null::ResizeWindow( const size_t width, const size_t height ) {
	// nothing
}

}
}