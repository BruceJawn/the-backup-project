/**
 * @author Pierre-Yves Gatouillat ==>> gatouillatpy@gmail.com
 */

#include "AS3.h"

short buffer[16384];

#include "../client/snd_local.h"

qboolean SNDDMA_Init( void )
{
	extern void trace( char* fmt, ... );
	trace( "SNDDMA_Init()" );
	
	memset( (void*)&dma, 0, sizeof(dma) );
	dma.speed = 44100;
	dma.channels = 2;
	dma.samplebits = 16;
	dma.samples = 4096;
	dma.submission_chunk = 1;
	dma.buffer = buffer;
	
	return qtrue;
}

int	SNDDMA_GetDMAPos( void )
{
	return 0;
}

void SNDDMA_Shutdown( void )
{
	extern void trace( char* fmt, ... );
	trace( "SNDDMA_Shutdown()" );

	memset( (void*)&dma, 0, sizeof(dma) );
}

void SNDDMA_BeginPainting( void ) 
{
}

void SNDDMA_Submit( void )
{
}