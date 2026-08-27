#include <cstring>
#include <limits>

#include "Buffer.h"

namespace types {

Buffer::Buffer() {
	allocated_len = 0;
	lenw = 0;
	lenr = 0;
	data = nullptr;
	dw = nullptr;
	dr = nullptr;
}

Buffer::Buffer( const std::string& val ) {
	if ( val.size() > std::numeric_limits< uint32_t >::max() ) {
		THROW( "buffer input is too large" );
	}
	allocated_len = static_cast< uint32_t >( val.size() );
	lenw = allocated_len;
	lenr = 0;
	data = lenw > 0
		? (data_t*)malloc( allocated_len )
		: nullptr;
	if ( lenw > 0 && !data ) {
		THROW( "unable to allocate buffer ( " + std::to_string( allocated_len ) + " bytes )" );
	}
	if ( lenw > 0 ) {
		memcpy( data, val.data(), lenw );
	}
	dw = data
		? data + lenw
		: nullptr;
	dr = data;
}

Buffer::~Buffer() {
	if ( data ) {
		free( data );
	}
}

Buffer::Buffer( const Buffer& other ) {
	allocated_len = other.allocated_len;
	lenw = other.lenw;
	lenr = other.lenr;
	if ( other.data ) {
		data_t* newptr = (data_t*)malloc( allocated_len );
		if ( !newptr ) {
			THROW( "unable to copy buffer ( " + std::to_string( allocated_len ) + " bytes )" );
		}
		data = newptr;
		memcpy( data, other.data, lenw );
	}
	else {
		data = nullptr;
	}
	dw = data
		? data + lenw
		: nullptr;
	dr = data
		? data + lenr
		: nullptr;
}

Buffer::Buffer( Buffer&& other ) noexcept {
	allocated_len = other.allocated_len;
	lenw = other.lenw;
	lenr = other.lenr;
	data = other.data;
	dw = other.dw;
	dr = other.dr;

	other.allocated_len = 0;
	other.lenw = 0;
	other.lenr = 0;
	other.data = nullptr;
	other.dw = nullptr;
	other.dr = nullptr;
}

Buffer& Buffer::operator=( const Buffer& other ) {
	if ( this != &other ) {
		data_t* new_data = nullptr;
		if ( other.data ) {
			new_data = (data_t*)malloc( other.allocated_len );
			if ( !new_data ) {
				THROW( "unable to assign buffer ( " + std::to_string( other.allocated_len ) + " bytes )" );
			}
			memcpy( new_data, other.data, other.lenw );
		}
		if ( data ) {
			free( data );
		}

		allocated_len = other.allocated_len;
		lenw = other.lenw;
		lenr = other.lenr;
		data = new_data;
		dw = data
			? data + lenw
			: nullptr;
		dr = data
			? data + lenr
			: nullptr;
	}
	return *this;
}

Buffer& Buffer::operator=( Buffer&& other ) noexcept {
	if ( this != &other ) {
		if ( data ) {
			free( data );
		}

		allocated_len = other.allocated_len;
		lenw = other.lenw;
		lenr = other.lenr;
		data = other.data;
		dw = other.dw;
		dr = other.dr;

		other.allocated_len = 0;
		other.lenw = 0;
		other.lenr = 0;
		other.data = nullptr;
		other.dw = nullptr;
		other.dr = nullptr;
	}
	return *this;
}

void Buffer::Alloc( uint32_t size ) {
	if ( size > ( std::numeric_limits< uint32_t >::max )() - lenw ) {
		THROW( "buffer allocation size overflow" );
	}
	const uint32_t new_size = lenw + size;
	if ( new_size > allocated_len ) {
		const uint64_t rounded_size =
			( static_cast< uint64_t >( new_size ) + BUFFER_ALLOC_CHUNK - 1 ) /
			BUFFER_ALLOC_CHUNK * BUFFER_ALLOC_CHUNK;
		const uint32_t new_allocated_len = rounded_size > ( std::numeric_limits< uint32_t >::max )()
			? ( std::numeric_limits< uint32_t >::max )()
			: static_cast< uint32_t >( rounded_size );
		data_t* new_data = nullptr;
		if ( data ) {
			//Log( "Reallocating " + to_string( new_allocated_len ) + " bytes" );
			new_data = (data_t*)realloc( data, new_allocated_len );
		}
		else {
			//Log( "Allocating " + to_string( new_allocated_len ) + " bytes" );
			new_data = (data_t*)malloc( new_allocated_len );
		}
		if ( !new_data ) {
			THROW( "unable to allocate buffer ( " + std::to_string( new_allocated_len ) + " bytes )" );
		}
		data = new_data;
		allocated_len = new_allocated_len;
		dw = ptr( data, lenw, 0 );
		dr = ptr( data, lenr, 0 );
	}
	lenw = new_size;
}

// note: mostly THROWs instead of ASSERTs, because we need that validation in release mode too to prevent buffer overflows
void Buffer::WriteImpl( type_t type, const char* s, const uint32_t sz ) {
	ASSERT( type > T_NONE && type < T_MAX, "invalid buffer write type " + std::to_string( type ) );
	//Log( "Writing " + to_string( sz ) + " bytes (type=" + to_string( type ) + ")" );
	checksum_t c = 0;
	const uint64_t total_size = sizeof( type ) + sizeof( sz ) + static_cast< uint64_t >( sz ) + sizeof( c );
	if ( total_size > ( std::numeric_limits< uint32_t >::max )() ) {
		THROW( "serialized buffer field is too large" );
	}
	if ( sz > 0 && !s ) {
		THROW( "serialized buffer field data is null" );
	}
	Alloc( static_cast< uint32_t >( total_size ) );
	memcpy( dw, &type, sizeof( type ) );
	dw += sizeof( type );
	memcpy( dw, &sz, sizeof( sz ) );
	dw += sizeof( sz );

	for ( uint32_t i = 0 ; i < sz ; i++ ) {
		c ^= ( *( dw++ ) = *( s++ ) );
	}

	//Log( "Writing checksum (" + to_string( c ) + ")" );
	*( dw++ ) = c;

	ASSERT( dw - data == lenw, "buffer write bytes count mismatch ( " + std::to_string( dw - data ) + " != " + std::to_string( lenw ) + " )" );
	//Log( "Written successfully" );
}

char* Buffer::ReadImpl( type_t need_type, char* s, uint32_t* sz, const uint32_t need_sz ) {
	ASSERT( need_type > T_NONE && need_type < T_MAX, "invalid buffer read type " + std::to_string( need_type ) );
	const uint32_t header_size = sizeof( type_t ) + sizeof( *sz );
	const uint32_t checksum_size = sizeof( checksum_t );
	type_t type = T_NONE;
	if ( GetRemaining() < header_size ) {
		THROW( "buffer ends prematurely (while reading header)" );
	}
	const auto* read_ptr = data + lenr;
	memcpy( &type, read_ptr, sizeof( type ) );
	read_ptr += sizeof( type );
	if ( type != need_type ) {
		THROW( "unexpected type on buffer read ( " + std::to_string( need_type ) + " != " + std::to_string( type ) + " )" );
	}
	memcpy( sz, read_ptr, sizeof( *sz ) );
	read_ptr += sizeof( *sz );
	if ( need_sz && ( need_sz != *sz ) ) {
		THROW( "buffer read size mismatch ( " + std::to_string( need_sz ) + " != " + std::to_string( *sz ) + " )" );
	}
	const uint64_t total_size = static_cast< uint64_t >( header_size ) + *sz + checksum_size;
	if ( total_size > GetRemaining() ) {
		THROW( "buffer ends prematurely (while reading data)" );
	}

	checksum_t need_c = 0;
	for ( uint32_t i = 0 ; i < *sz ; i++ ) {
		need_c ^= static_cast< checksum_t >( read_ptr[ i ] );
	}
	const checksum_t c = static_cast< checksum_t >( read_ptr[ *sz ] );
	if ( need_c != c ) {
		THROW( "buffer read checksum mismatch ( " + std::to_string( need_c ) + " != " + std::to_string( c ) + " )" );
	}

	if ( s == nullptr && *sz > 0 ) {
		s = (char*)malloc( *sz );
		if ( !s ) {
			THROW( "unable to allocate serialized field ( " + std::to_string( *sz ) + " bytes )" );
		}
	}
	if ( *sz > 0 ) {
		memcpy( s, read_ptr, *sz );
	}

	lenr += static_cast< uint32_t >( total_size );
	dr = data + lenr;
	ASSERT( dr - data == lenr, "buffer read bytes count mismatch ( " + std::to_string( dr - data ) + " != " + std::to_string( lenr ) + " )" );

	return s;
}

void Buffer::WriteBool( const bool val ) {
	const uint8_t bval = val
		? 1
		: 0;
	WriteImpl( T_BOOL, (const char*)&bval, sizeof( bval ) );
}

const bool Buffer::ReadBool() {
	uint8_t boolval = 0;
	uint32_t sz = 0;
	ReadImpl( T_BOOL, (char*)&boolval, &sz, sizeof( boolval ) );
	if ( boolval > 1 ) {
		THROW( "invalid serialized boolean: " + std::to_string( boolval ) );
	}
	return boolval == 1;
}

void Buffer::WriteInt( const long long int val ) {
	WriteImpl( T_INT, (const char*)&val, sizeof( val ) );
}

const long long int Buffer::ReadInt() {
	long long int val = 0;
	uint32_t sz = 0;
	ReadImpl( T_INT, (char*)&val, &sz, sizeof( val ) );
	return val;
}

const size_t Buffer::ReadCollectionSize( const std::string& name ) {
	const auto count = ReadInt< size_t >( name + " count" );
	const size_t min_element_size = sizeof( type_t ) + sizeof( uint32_t ) + sizeof( checksum_t );
	const size_t max_count = GetRemaining() / min_element_size;
	if ( count > max_count ) {
		THROW( "invalid serialized " + name + " count: " + std::to_string( count ) );
	}
	return count;
}

void Buffer::WriteFloat( const float val ) {
	WriteImpl( T_FLOAT, (const char*)&val, sizeof( val ) );
}

const float Buffer::ReadFloat() {
	float val = 0;
	uint32_t sz = 0;
	ReadImpl( T_FLOAT, (char*)&val, &sz, sizeof( val ) );
	return val;
}

void Buffer::WriteString( const std::string& val ) {
	if ( val.size() > ( std::numeric_limits< uint32_t >::max )() ) {
		THROW( "serialized string is too large" );
	}
	WriteImpl( T_STRING, val.data(), static_cast< uint32_t >( val.size() ) );
}

const std::string Buffer::ReadString() {
	uint32_t sz = 0;
	char* res_data = ReadImpl( T_STRING, nullptr, &sz );
	if ( sz > 0 ) {
		std::string result = std::string( res_data, sz );
		free( res_data );
		return result;
	}
	else {
		return "";
	}
}

void Buffer::WriteVec2u( const Vec2< uint32_t > val ) {
	WriteImpl( T_VEC2U, (const char*)&val, sizeof( val ) );
}

const Vec2< uint32_t > Buffer::ReadVec2u() {
	types::Vec2< uint32_t > val = {
		0,
		0
	};
	uint32_t sz = 0;
	ReadImpl( T_VEC2U, (char*)&val, &sz, sizeof( val ) );
	return val;
}

void Buffer::WriteVec2f( const Vec2< float > val ) {
	WriteImpl( T_VEC2F, (const char*)&val, sizeof( val ) );
}

const Vec2< float > Buffer::ReadVec2f() {
	types::Vec2< float > val = {
		0,
		0
	};
	uint32_t sz = 0;
	ReadImpl( T_VEC2F, (char*)&val, &sz, sizeof( val ) );
	return val;
}

void Buffer::WriteVec3( const types::Vec3 val ) {
	WriteImpl( T_VEC3, (const char*)&val, sizeof( val ) );
}

const types::Vec3 Buffer::ReadVec3() {
	types::Vec3 val;
	uint32_t sz = 0;
	ReadImpl( T_VEC3, (char*)&val, &sz, sizeof( val ) );
	return val;
}

void Buffer::WriteColor( const Color& val ) {
	WriteImpl( T_COLOR, (const char*)&val.value, sizeof( val.value ) );
}

void Buffer::ReadColor( Color& val ) {
	uint32_t sz = 0;
	ReadImpl( T_COLOR, (char*)&val.value, &sz, sizeof( val.value ) );
}

void Buffer::WriteData( const void* data, const uint32_t len ) {
	WriteImpl( T_DATA, (const char*)data, len );
}

const void* Buffer::ReadData( const uint32_t len ) {
	uint32_t sz = 0;
	const void* val = ReadImpl( T_DATA, nullptr, &sz, len );
	if ( sz != len ) {
		free( (void*)val );
		THROW( "buffer data read size mismatch" );
	}
	return val;
}

const std::string Buffer::ToString() const {
	return data
		? std::string( (const char*)data, lenw )
		: "";
}

const uint32_t Buffer::GetRemaining() const {
	ASSERT( lenr <= lenw, "buffer read position overflow" );
	return lenw - lenr;
}

}
