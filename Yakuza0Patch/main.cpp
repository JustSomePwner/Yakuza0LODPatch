#include <windows.h>
#include <stdio.h>
#include "process.h"

int main( )
{
	SetConsoleTitleA( "Yakuza 0 LOD patch" );
	printf( "> community does what Sega QA don't \n" );

	c_process process;

	if ( process.open( "Yakuza0.exe" ) )
	{
		// ORIGINAL: cmovnz  eax, ecx
		// CUSTOM:   mov     al 0x1 nop
		if ( process.patch( "0F 45 C1 88 05 ? ? ? ? 81 63", 0, { 0xB0, 0x01, 0x90 } ) )
		{
			printf( "> patch #1: SUCCESS \n" );
		}
		else
		{
			printf( "> patch #1: FAILED \n" );
		}

		// ORIGINAL: mov     dword ptr [rbx], 4
		// CUSTOM:   bunch a nopssss
		if ( process.patch( "C7 03 ? ? ? ? 48 8B 74 24 30 40", 0, { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 } ) )
		{
			printf( "> patch #2: SUCCESS \n" );
		}
		else
		{
			printf( "> patch #2: FAILED \n" );
		}

		// ORIGINAL: mov     [rsp+48h+var_10], 40800000h
		// CUSTOM:   mov     [rsp+48h+var_10], 42480000h
		if ( process.patch( "C7 44 24 38 00 00 80 40 C7 44 24 ? ? ? ? ? F3 0F 10 44 84", 6, { 0x48, 0x42 } ) )
		{
			printf( "> patch #3: SUCCESS \n" );
		}
		else
		{
			printf( "> patch #3: FAILED \n" );
		}

		// ORIGINAL:  movss   xmm4, [rsp+arg_20]
		//            movzx   ecx, dl
		// CUSTOM:    mov     rax,0x0
		//            ret
		if ( process.patch( "F3 0F 10 64 24 ? 0F B6 CA", 0, { 0x48, 0xC7, 0xC0, 0, 0, 0, 0, 0xC3 } ) )
		{
			printf( "> patch #4: SUCCESS \n" );
		}
		else
		{
			printf( "> patch #4: FAILED \n" );
		}

		process.close( );
	}
	else
	{
		printf( "> could not open target \n" );
	}

	system( "pause" );
}