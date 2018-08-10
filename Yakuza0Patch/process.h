#pragma once
#include <windows.h>
#include <cstdint>
#include <vector>
#include <TlHelp32.h>

#undef max

class c_process
{
	struct module_info_t
	{
		int    m_pid{ };
		HANDLE m_handle{ };

		uintptr_t m_base{ };
		uintptr_t m_size{ };
	};

	module_info_t m_mod_info;

	static int get_pid ( const char* proc_name )
	{
		PROCESSENTRY32 proc_entry;
		HANDLE		   t32_snapshot;
		int			   pid = 0;

		t32_snapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
		proc_entry.dwSize = sizeof( proc_entry );

		for ( Process32First( t32_snapshot, &proc_entry ); Process32Next( t32_snapshot, &proc_entry ); )
		{
			if ( strcmp( proc_name, proc_entry.szExeFile ) == 0 )
			{
				pid = proc_entry.th32ProcessID;
				break;
			}
		};

		CloseHandle( t32_snapshot );
		return pid;
	}

	static bool get_module_info( const char* module_name, module_info_t* mod_info )
	{
		MODULEENTRY32 mod_entry;
		HANDLE		  t32_snapshot;
		bool          found = false;

		t32_snapshot = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, mod_info->m_pid );
		mod_entry.dwSize = sizeof( mod_entry );

		if ( !Module32First( t32_snapshot, &mod_entry ) )
			return false;

		do
		{
			if ( strcmp( mod_entry.szModule, module_name ) == 0 )
			{
				found = true;
				mod_info->m_base = uintptr_t( mod_entry.modBaseAddr );
				mod_info->m_size = uintptr_t( mod_entry.modBaseSize );
				break;
			}
		}
		while ( Module32Next( t32_snapshot, &mod_entry ) );

		CloseHandle( t32_snapshot );

		return found;
	}

public:
	~c_process( )
	{
		close( );
	}

	bool open( const char* proc_name )
	{
		// cleanup
		close( );

		// find pid
		m_mod_info.m_pid = get_pid( proc_name );
		if ( !m_mod_info.m_pid )
			return false;

		// open target
		m_mod_info.m_handle = OpenProcess( PROCESS_ALL_ACCESS, 0, m_mod_info.m_pid );
		if ( !m_mod_info.m_handle )
			return false;

		// get module info
		return get_module_info( proc_name, &m_mod_info );
	}

	void close( )
	{
		if ( m_mod_info.m_handle )
		{
			CloseHandle( m_mod_info.m_handle );
			m_mod_info.m_handle = nullptr;
		}
	}

	__forceinline bool read( uintptr_t address, void* buffer, size_t size )
	{
		if ( !m_mod_info.m_handle )
			return false;

		return ReadProcessMemory( m_mod_info.m_handle, (void*)address, buffer, size, nullptr );
	}

	__forceinline bool write( uintptr_t address, void* buffer, size_t size )
	{
		if ( !m_mod_info.m_handle )
			return false;

		return WriteProcessMemory( m_mod_info.m_handle, (void*)address, buffer, size, nullptr );
	}

	uintptr_t scan( const char* pattern )
	{
		const auto get_pattern = []( const char* raw_pattern ) -> std::vector< uintptr_t >
		{
			std::vector< uintptr_t > pattern;
			char* start = (char*)raw_pattern;
			char* end = (char*)(raw_pattern) + strlen( raw_pattern );

			for ( char* byte = start; byte < end; ++byte )
			{
				if ( *byte == '?' )
				{
					if ( *( ++byte ) == '?' )
						byte++;

					pattern.push_back( std::numeric_limits< uintptr_t >::max( ) );
				}
				else
				{
					pattern.push_back( strtoul( byte, &byte, 16 ) );
				}
			}

			return pattern;
		};

		auto bytes = get_pattern( pattern );

		uint8_t* bin = new uint8_t[m_mod_info.m_size];
		read( m_mod_info.m_base, bin, m_mod_info.m_size );

		for ( uintptr_t address = 0; address < m_mod_info.m_size - bytes.size( ); address++ )
		{
			bool found = true;

			for ( uintptr_t token = 0; token < bytes.size( ); ++token )
			{
				if ( bytes.data( )[token] != std::numeric_limits< uintptr_t >::max( ) && bin[address + token] != bytes.data( )[token] )
				{
					found = false;
					break;
				}
			}

			if ( found )
			{
				delete[] bin;
				return m_mod_info.m_base + address;
			}
		}

		delete[] bin;
		return 0;
	}

	bool patch( const char* pattern, ptrdiff_t offset, std::vector<uint8_t> patch )
	{
		uintptr_t patch_address = scan( pattern );
		if ( !patch_address )
			return false;

		patch_address += offset;

		write( patch_address, patch.data( ), patch.size( ) );
		return true;
	}
};
