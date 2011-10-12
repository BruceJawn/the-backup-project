/**
 * @author Pierre-Yves Gatouillat ==>> gatouillatpy@gmail.com
 */

#include "float4.h"
#include "float4x4.h"

#ifdef FLASH
	
//#define USE_MOLEHILL_CACHE

/***************************************************/
/** GLOBALS                                        */
/***************************************************/

AS3_Val _renderer;

unsigned masm_data[1048576];
unsigned* masm_ptr;

float4x4 matrix_stack_data[20];
float4x4* matrix_stack_ptr;

float4x4 world_matrix;
float4x4 projection_matrix;
float4x4* matrix;

float vertices[65536];
float colors[65536];
float texcoords[32768];

int vertex_count;
int index_count;

float color_r;
float color_g;
float color_b;
float color_a;

float texcoord_u;
float texcoord_v;

float sum_vertex_x;
float sum_vertex_y;
float sum_vertex_z;
float sum_vertex_w;

float sum_color_r;
float sum_color_g;
float sum_color_b;
float sum_color_a;

float sum_texcoord_u;
float sum_texcoord_v;

int use_texturing;

int texture_id;

int polygon_type;

int need_draw;

int vertex_offset;

int cache_count[65536];

/***************************************************/
/** INIT                                           */
/***************************************************/

void molehill_init()
{
	masm_ptr = masm_data;
	
	matrix_stack_ptr = matrix_stack_data;
	
	memset( cache_count, 0, 65536 * sizeof(int) );
}

/***************************************************/
/** CONTROL                                        */
/***************************************************/

#define MOLEHILL_BEGIN					0x00010000
#define MOLEHILL_END					0x00020000

void molehill_start()
{
	int k;
	
	masm_ptr = masm_data;
	
	*masm_ptr = MOLEHILL_BEGIN;
	masm_ptr++;
	
	need_draw = 0;
	
	vertex_offset = 0;
	
#ifdef USE_MOLEHILL_CACHE
 	for ( k = 0 ; k < 65536 ; k++ )
	{
		if ( cache_count[k] > 0 ) cache_count[k]--;
	}
#endif
}

void molehill_stop()
{
	*masm_ptr = MOLEHILL_END;
	masm_ptr++;

	AS3_Val params = AS3_Array( "AS3ValType", AS3_Ptr(masm_data) );
	AS3_CallS( "execute", _renderer, params );
}

#endif