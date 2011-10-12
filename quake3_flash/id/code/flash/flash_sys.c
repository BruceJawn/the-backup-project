/**
 * @author Pierre-Yves Gatouillat ==>> gatouillatpy@gmail.com
 */

#define	MAX_FOUND_FILES	0x1000

#define	MAX_QUED_EVENTS		256
#define	MASK_QUED_EVENTS	( MAX_QUED_EVENTS - 1 )

#include <errno.h>
#include <stdio.h>
#include <math.h>
#include "../qcommon/qcommon.h"

char* files[200];
int fc = 0;

sysEvent_t eventQue[MAX_QUED_EVENTS];

int eventHead = 0;
int eventTail = 0;
byte sys_packetReceived[MAX_MSGLEN];

void S_Update_();
extern void IN_Init( void );
extern void IN_Frame( void );

//===================================================================

qboolean Sys_LowPhysicalMemory()
{
	return qfalse;
}

void Sys_BeginProfiling( void )
{
}

long fastftol( float f )
{
	return (long)f;
}

void Sys_SnapVector( float *v )
{
	v[0] = floor(v[0]);
	v[1] = floor(v[1]);
	v[2] = floor(v[2]);
}

/*void VM_Compile( vm_t *vm, vmHeader_t *header )
{
}

int	VM_CallCompiled( vm_t *vm, int *args )
{
}*/

void Sys_ListFilteredFiles( const char* basedir, char* subdirs, char* filter, char** list, int* numfiles )
{
	char		search[MAX_OSPATH], newsubdirs[MAX_OSPATH];
	char		filename[MAX_OSPATH];
	int			findhandle;

	extern void trace( char* fmt, ... );
	trace( "Sys_ListFilteredFiles( %s, %s, %s, %d, %d )", basedir, subdirs, filter, list, numfiles );

	/*if ( *numfiles >= MAX_FOUND_FILES - 1 ) {
		return;
	}

	if (strlen(subdirs)) {
		Com_sprintf( search, sizeof(search), "%s\\%s\\*", basedir, subdirs );
	}
	else {
		Com_sprintf( search, sizeof(search), "%s\\*", basedir );
	}

	findhandle = _findfirst (search, &findinfo);
	if (findhandle == -1) {
		return;
	}

	do {
		if (findinfo.attrib & _A_SUBDIR) {
			if (Q_stricmp(findinfo.name, ".") && Q_stricmp(findinfo.name, "..")) {
				if (strlen(subdirs)) {
					Com_sprintf( newsubdirs, sizeof(newsubdirs), "%s\\%s", subdirs, findinfo.name);
				}
				else {
					Com_sprintf( newsubdirs, sizeof(newsubdirs), "%s", findinfo.name);
				}
				Sys_ListFilteredFiles( basedir, newsubdirs, filter, list, numfiles );
			}
		}
		if ( *numfiles >= MAX_FOUND_FILES - 1 ) {
			break;
		}
		Com_sprintf( filename, sizeof(filename), "%s\\%s", subdirs, findinfo.name );
		if (!Com_FilterPath( filter, filename, qfalse ))
			continue;
		list[ *numfiles ] = CopyString( filename );
		(*numfiles)++;
	} while ( _findnext (findhandle, &findinfo) != -1 );

	_findclose (findhandle);*/
}

static qboolean strgtr(const char* s0, const char* s1)
{
	int l0, l1, i;

	l0 = strlen(s0);
	l1 = strlen(s1);

	if (l1<l0) {
		l0 = l1;
	}

	for(i=0;i<l0;i++) {
		if (s1[i] > s0[i]) {
			return qtrue;
		}
		if (s1[i] < s0[i]) {
			return qfalse;
		}
	}
	return qfalse;
}

char** Sys_ListFiles( const char* directory, const char* extension, char* filter, int* numfiles, qboolean wantsubs )
{
	int nfiles;
	char** listCopy;
	char* list[200];
	int finddirs;
	char* curfile;
	int curlen;
	int dirlen;
	int extlen;
	int i;
	
	if ( filter )
	{
		nfiles = 0;
		Sys_ListFilteredFiles( directory, "", filter, list, &nfiles );

		list[nfiles] = 0;
		*numfiles = nfiles;

		if ( !nfiles ) return NULL;

		listCopy = Z_Malloc( ( nfiles + 1 ) * sizeof( *listCopy ) );
		
		for ( i = 0 ; i < nfiles ; i++ ) listCopy[i] = list[i];
		
		listCopy[i] = NULL;

		return listCopy;
	}

	if ( !extension ) extension = "";

	dirlen = strlen( directory );
	extlen = strlen( extension );
	
	nfiles = 0;

	for ( i = 0 ; i < fc ; i++ )
	{
		curfile = files[i];
		curlen = strlen( curfile );
		
		if ( strncmp( curfile, directory, dirlen ) == 0 )
		{
			if ( strncmp( curfile+curlen-extlen, extension, extlen ) == 0 )
			{
				list[nfiles] = Z_Malloc( (curlen + 1) * sizeof(char) );
				strcpy( list[nfiles], curfile+dirlen+1 ); nfiles++;
			}
		}
	}

	// return a copy of the list
	*numfiles = nfiles;

	if ( !nfiles ) return NULL;

	listCopy = Z_Malloc( (nfiles + 1) * sizeof( *listCopy ) );
	for ( i = 0 ; i < nfiles ; i++ ) listCopy[i] = list[i];
	
	listCopy[i] = NULL;

	return listCopy;
}

void Sys_FreeFileList( char** list )
{
	int i;

	if ( !list ) return;

	for ( i = 0 ; list[i] ; i++ ) Z_Free( list[i] );

	Z_Free( list );
}

void Sys_ShowConsole( int visLevel, qboolean quitOnClose )
{
}

int Sys_Milliseconds( void )
{
	extern int as3GetRealTime( void );
	return as3GetRealTime();
}

void Sys_QueEvent( int time, sysEventType_t type, int value, int value2, int ptrLength, void *ptr )
{
	sysEvent_t  *ev;

	ev = &eventQue[ eventHead & MASK_QUED_EVENTS ];

	if ( eventHead - eventTail >= MAX_QUED_EVENTS )
	{
		Com_Printf( "Sys_QueEvent: overflow\n" );

		if ( ev->evPtr ) Z_Free( ev->evPtr );
		
		eventTail++;
	}

	eventHead++;

	if ( time == 0 )
		time = Sys_Milliseconds();

	ev->evTime = time;
	ev->evType = type;
	ev->evValue = value;
	ev->evValue2 = value2;
	ev->evPtrLength = ptrLength;
	ev->evPtr = ptr;
}

sysEvent_t Sys_GetEvent( void )
{
	sysEvent_t  ev;
  /*char    *s;
  msg_t   netmsg;
  netadr_t  adr;*/

	if ( eventHead > eventTail )
	{
		eventTail++;
		return eventQue[ ( eventTail - 1 ) & MASK_QUED_EVENTS ];
	}

  /*
  // pump the message loop
  // in vga this calls KBD_Update, under X, it calls GetEvent
  Sys_SendKeyEvents ();

  // check for console commands
  s = Sys_ConsoleInput();
  if ( s )
  {
    char  *b;
    int   len;

    len = strlen( s ) + 1;
    b = Z_Malloc( len );
    strcpy( b, s );
    Sys_QueEvent( 0, SE_CONSOLE, 0, 0, len, b );
  }*/

	IN_Frame();

  /*
  // check for network packets
  MSG_Init( &netmsg, sys_packetReceived, sizeof( sys_packetReceived ) );
  if ( Sys_GetPacket ( &adr, &netmsg ) )
  {
    netadr_t    *buf;
    int       len;

    // copy out to a seperate buffer for qeueing
    len = sizeof( netadr_t ) + netmsg.cursize;
    buf = Z_Malloc( len );
    *buf = adr;
    memcpy( buf+1, netmsg.data, netmsg.cursize );
    Sys_QueEvent( 0, SE_PACKET, 0, 0, len, buf );
  }*/

	if ( eventHead > eventTail )
	{
		eventTail++;
		return eventQue[ ( eventTail - 1 ) & MASK_QUED_EVENTS ];
	}

	extern int as3GetRealTime( void );
	
	memset( &ev, 0, sizeof( ev ) );
	ev.evTime = as3GetRealTime();

	return ev;
}

void Com_Memcpy( void* dest, const void* src, const size_t count )
{
	memcpy( dest, src, count );
}

void Com_Memset( void* dest, const int val, const size_t count )
{
	memset( dest, val, count );
}

char* Sys_Cwd( void )
{
	return "";
}

char* Sys_DefaultCDPath( void )
{
	return ".";
}

char* Sys_DefaultInstallPath( void )
{
	return Sys_Cwd();
}

char* Sys_DefaultHomePath( void )
{
	return 0;
}

void Sys_BeginStreamedFile( fileHandle_t f, int readAhead )
{
}

void Sys_EndStreamedFile( fileHandle_t f )
{
}

int Sys_StreamedRead( void *buffer, int size, int count, fileHandle_t f )
{
   return FS_Read( buffer, size * count, f );
}

void Sys_StreamSeek( fileHandle_t f, int offset, int origin )
{
   FS_Seek( f, offset, origin );
}

void Sys_Print( const char* msg )
{
	extern void trace( char* fmt, ... );
	trace( msg );	
}

void Sys_Error( const char *error, ... )
{
	va_list argptr;

	printf( "Sys_Error: " );	
	va_start( argptr, error );
	vprintf( error, argptr );
	va_end( argptr );
	printf( "\n" );
}

void Sys_Quit( void )
{
}

void Sys_UnloadGame( void )
{
}

void* Sys_GetGameAPI( void* parms )
{
	return NULL;
}

char* Sys_GetClipboardData( void )
{
	return NULL;
}

void Sys_Mkdir( const char* path )
{
}

char* Sys_FindFirst( char* path, unsigned musthave, unsigned canthave )
{
	extern void trace( char* fmt, ... );
	trace( "Sys_FindFirst( %s, %d, %d )", path, musthave, canthave );
	
	return NULL;
}

char* Sys_FindNext( unsigned musthave, unsigned canthave )
{
	extern void trace( char* fmt, ... );
	trace( "Sys_FindNext( %d, %d )", musthave, canthave );

	return NULL;
}

void Sys_FindClose( void )
{
}

void Sys_Init( void )
{
}

void Sys_EarlyOutput( char* string )
{
	printf( "%s", string );
}
