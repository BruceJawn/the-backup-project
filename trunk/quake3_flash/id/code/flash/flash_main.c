/**
 * @author Pierre-Yves Gatouillat ==>> gatouillatpy@gmail.com
 */

#include <errno.h>
#include <stdio.h>
#include "../qcommon/qcommon.h"

#include <AS3.h>

int _curTime;
int _oldTime;

AS3_Val _swfMain;

extern AS3_Val _flash_sample_data;

extern char* files[];
extern int fc;

extern void S_Update_();

extern void IN_Init( void );
extern void IN_Frame( void );
extern void IN_MouseUp( int button );
extern void IN_MouseDown( int button );
extern void IN_KeybUp( int keycode );
extern void IN_KeybDown( int keycode );

//===================================================================

#include "molehill.c"

static int readByteArray( void* cookie, char* dst, int size )
{
	return AS3_ByteArray_readBytes( dst, (AS3_Val)cookie, size );
}
 
static int writeByteArray( void *cookie, const char *src, int size )
{
	return AS3_ByteArray_writeBytes( (AS3_Val)cookie, (char*)src, size );
}

static fpos_t seekByteArray( void *cookie, fpos_t offs, int whence )
{
	return AS3_ByteArray_seek( (AS3_Val)cookie, offs, whence );
}

static int closeByteArray( void* cookie )
{
	AS3_Val zero = AS3_Int(0);
	AS3_SetS( (AS3_Val)cookie, "position", zero );
	AS3_Release( zero );
	
	return 0;
}

FILE* as3OpenWriteFile( const char* filename )
{
	FILE* ret;
	AS3_Val byteArray;

	AS3_Val params = AS3_Array( "AS3ValType", AS3_String(filename) );
	
	byteArray = AS3_CallS( "fileWriteSharedObject", _swfMain, params );
	AS3_Release( params );

	ret = funopen( (void*)byteArray, readByteArray, writeByteArray, seekByteArray, closeByteArray );
	
	return ret;
}

void as3UpdateFileSharedObject( const char* filename )
{
	AS3_Val params = AS3_Array( "AS3ValType", AS3_String(filename) );

	AS3_CallS( "fileUpdateSharedObject", _swfMain, params );

	AS3_Release( params );
}

void as3ReadFileSharedObject( const char* filename )
{
	AS3_Val params = AS3_Array( "AS3ValType", AS3_String(filename) );

	AS3_CallS( "fileReadSharedObject", _swfMain, params );

	AS3_Release( params );
}

int as3GetRealTime( void )
{
	AS3_Val params = AS3_Array( "" );

	AS3_Val retval = AS3_CallS( "getRealTime", _swfMain, params );

	AS3_Release( params );

	return AS3_IntValue( retval );
}

void trace( char* fmt, ... )
{
	va_list argptr;
	static char msg[1000];
	AS3_Val as3Str;
	
	va_start( argptr, fmt );
	vsprintf( msg, fmt, argptr );
	va_end( argptr );

	AS3_Val params = AS3_Array( "AS3ValType", AS3_String(msg) );
	AS3_CallS( "logToConsole", _swfMain, params );
	AS3_Release( params );
}

AS3_Val swcInit( void *data, AS3_Val args )
{
	AS3_ArrayValue( args, "AS3ValType", &_swfMain );

	return AS3_Ram();
}

AS3_Val swcFrame( void *data, AS3_Val args )
{
	AS3_ArrayValue( args, "IntType", &_curTime );

	molehill_start();
	
	Com_Frame();
	
	molehill_stop();
	
	_oldTime = _curTime;

	return AS3_Ptr(0);
}

AS3_Val swcWriteSoundData( void* data, AS3_Val args )
{
	int delta;
	
	AS3_ArrayValue( args, "AS3ValType,IntType", &_flash_sample_data, &delta );

	extern int s_soundtime; s_soundtime += delta;
	
	S_Update_();
	
	return NULL;
}

AS3_Val swcKeyUp( void* data, AS3_Val args )
{
	int key;
	AS3_ArrayValue( args, "IntType", &key );

	extern byte _asToQKey[256];
	IN_KeybUp( _asToQKey[key] );

	return NULL;
}

AS3_Val swcKeyDown( void* data, AS3_Val args )
{
	int key;
	AS3_ArrayValue( args, "IntType", &key );

	extern byte _asToQKey[256];
	IN_KeybDown( _asToQKey[key] );

	return NULL;
}

AS3_Val swcMouseUp( void* data, AS3_Val args )
{
	int btn;
	AS3_ArrayValue( args, "IntType", &btn );

	IN_MouseUp( btn );

	return NULL;
}

AS3_Val swcMouseDown( void* data, AS3_Val args )
{
	int btn;
	AS3_ArrayValue( args, "IntType", &btn );

	IN_MouseDown( btn );
	
	return NULL;
}

AS3_Val swcMouseMove( void* data, AS3_Val args )
{
	extern int mouse_x, mouse_y;
	int delta_x, delta_y;
	
	AS3_ArrayValue( args, "IntType,IntType", &delta_x, &delta_y );
	
	mouse_x += delta_x;
	mouse_y += delta_y;

	return NULL;
}

AS3_Val swcRegisterFile( void* data, AS3_Val args )
{
	char* file;
	
	AS3_ArrayValue( args, "StrType", &file );

	files[fc++] = file;

	return NULL;
}

AS3_Val swcInitQuake( void *data, AS3_Val args )
{
	int argc;
	char* argv;

	AS3_ArrayValue( args, "AS3ValType", &_renderer );
	
	molehill_init();

	argc = 1;
	argv = "";
	
	int len, i;
	char* cmdline;
	
	// Sys_SetDefaultCDPath( argv[0] );

	for ( len = 1, i = 1; i < argc; i++ )
		len += strlen(argv[i]) + 1;
		
	cmdline = malloc( len );
	*cmdline = 0;
	
	for ( i = 1 ; i < argc ; i++ )
	{
		if ( i > 1 )
			strcat( cmdline, " " );
			
		strcat( cmdline, argv[i] );
	}
	
	molehill_start();
	
	Com_Init( cmdline );

	NET_Init();
	
	IN_Init();
	
	molehill_stop();

	return NULL;
}

//===================================================================

void main( int argc, char **argv )
{
	int i;

	AS3_Val swcEntries[] = 
	{
		AS3_Function( NULL, swcInit ),
		AS3_Function( NULL, swcFrame ),		
		AS3_Function( NULL, swcKeyUp ),
		AS3_Function( NULL, swcKeyDown ),
		AS3_Function( NULL, swcMouseUp ),
		AS3_Function( NULL, swcMouseDown ),
		AS3_Function( NULL, swcMouseMove ),
		AS3_Function( NULL, swcWriteSoundData ),
		AS3_Function( NULL, swcRegisterFile ),
		AS3_Function( NULL, swcInitQuake )
	};

	AS3_Val result = AS3_Object
	(
		"swcInit:AS3ValType,\
		swcFrame:AS3ValType,\
		swcKeyUp:AS3ValType,\
		swcKeyDown:AS3ValType,\
		swcMouseUp:AS3ValType,\
		swcMouseDown:AS3ValType,\
		swcMouseMove:AS3ValType,\
		swcWriteSoundData:AS3ValType,\
		swcRegisterFile:AS3ValType,\
		swcInitQuake:AS3ValType",
		swcEntries[0],
		swcEntries[1],
		swcEntries[2],
		swcEntries[3],
		swcEntries[4],
		swcEntries[5],
		swcEntries[6],
		swcEntries[7],
		swcEntries[8],
		swcEntries[9]
	);

	for ( i = 0 ; i < 10 ; i++ )
		AS3_Release( swcEntries[i] );
	
	AS3_LibInit( result );
}


