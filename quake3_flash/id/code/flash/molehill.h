/**
 * @author Pierre-Yves Gatouillat ==>> gatouillatpy@gmail.com
 */

#ifndef MOLEHILL_H
#define MOLEHILL_H

//#define USE_MOLEHILL_CACHE

#include <AS3.h>

#include "float4.h"
#include "float4x4.h"

/***************************************************/
/** CONSTANTS                                      */
/***************************************************/

/**
 * Molehill <=> OpenGL
 *
 * MiniASM Notes :
 * - 32 bits length instructions
 *   - 16 higher bits for µ-instruction
 *   - 16 lower bitq for µ-specification
 * - 3 parameter types : int, void*, float
 */

#define MOLEHILL_BEGIN					0x00010000
#define MOLEHILL_END					0x00020000

#define MOLEHILL_CLEAR					0x00110000
#define MOLEHILL_CLEAR_EX				0x00120000
#define MOLEHILL_FLUSH					0x00130000

#define MOLEHILL_BIND					0x00210000

#define MOLEHILL_SET_DEPTHWRITE			0x00310000
#define MOLEHILL_SET_DEPTHFUNC			0x00320000
#define MOLEHILL_SET_BLENDING			0x00330000
#define MOLEHILL_SET_VIEWPORT			0x00340000
#define MOLEHILL_SET_CULLING			0x00350000
#define MOLEHILL_SET_ALPHATEST			0x00360000
#define MOLEHILL_SET_ALPHAFUNC			0x00370000
#define MOLEHILL_SET_POLYGONOFFSET		0x00380000
#define MOLEHILL_SET_DEPTHRANGE			0x00390000
#define MOLEHILL_SET_TEXTUREWRAPMODE	0x00400000

#define MOLEHILL_MATRIX					0x00410000
#define MOLEHILL_VERTICES				0x00420000
#define MOLEHILL_INDICES				0x00430000
#define MOLEHILL_VERTEX_POINTER			0x00440000
#define MOLEHILL_COLOR_POINTER			0x00450000
#define MOLEHILL_TEXCOORD_POINTER		0x00460000
#define MOLEHILL_INDEX_POINTER			0x00470000

#define MOLEHILL_DRAW					0x00510000
#define MOLEHILL_DRAW_FAST				0x00520000

#define MOLEHILL_UPLOAD_CACHE			0x00610000
#define MOLEHILL_DRAW_CACHE				0x00620000

#define MOLEHILL_TRIANGLE_LIST			0x00000001
#define MOLEHILL_TRIANGLE_STRIP			0x00000002
#define MOLEHILL_TRIANGLE_FAN			0x00000003
#define MOLEHILL_QUAD_LIST				0x00000011
#define MOLEHILL_POLYGON				0x00000024

#define MOLEHILL_FALSE					0x00000000
#define MOLEHILL_TRUE					0x00000001

#define MOLEHILL_ALWAYS					0x00000001
#define MOLEHILL_LESS_OR_EQUAL			0x00000002
#define MOLEHILL_EQUAL					0x00000003
#define MOLEHILL_GREATER_OR_EQUAL		0x00000004

#define MOLEHILL_ONE					0x00000001
#define MOLEHILL_ZERO					0x00000002
#define MOLEHILL_SRC_ALPHA				0x00000011
#define MOLEHILL_SRC_COLOR				0x00000012
#define MOLEHILL_ONE_MINUS_SRC_ALPHA	0x00000021
#define MOLEHILL_ONE_MINUS_SRC_COLOR	0x00000022
#define MOLEHILL_DST_ALPHA				0x00000031
#define MOLEHILL_DST_COLOR				0x00000032
#define MOLEHILL_ONE_MINUS_DST_ALPHA	0x00000041
#define MOLEHILL_ONE_MINUS_DST_COLOR	0x00000042

#define MOLEHILL_NONE					0x00000001
#define MOLEHILL_FRONT					0x00000002
#define MOLEHILL_BACK					0x00000003

#define MOLEHILL_WORLD					0x00000001
#define MOLEHILL_PROJECTION				0x00000002

#define MOLEHILL_I8						0x00000001
#define MOLEHILL_B8G8R8					0x00000002
#define MOLEHILL_A8B8G8R8				0x00000003

#define MOLEHILL_CLAMP					0x00000001
#define MOLEHILL_REPEAT					0x00000002

/***************************************************/
/** GLOBALS                                        */
/***************************************************/

extern void trace( char *fmt, ... );

extern AS3_Val _renderer;

extern unsigned masm_data[65536];
extern unsigned* masm_ptr;

extern float4x4 matrix_stack_data[15];
extern float4x4* matrix_stack_ptr;

extern float4x4 world_matrix;
extern float4x4 projection_matrix;
extern float4x4* matrix;

extern float vertices[65536];
extern float colors[65536];
extern float texcoords[32768];

extern int vertex_count;
extern int index_count;

extern float color_r;
extern float color_g;
extern float color_b;
extern float color_a;

extern float texcoord_u;
extern float texcoord_v;

extern float sum_vertex_x;
extern float sum_vertex_y;
extern float sum_vertex_z;
extern float sum_vertex_w;

extern float sum_color_r;
extern float sum_color_g;
extern float sum_color_b;
extern float sum_color_a;

extern float sum_texcoord_u;
extern float sum_texcoord_v;

extern int use_texturing;

extern int polygon_type;

extern int texture_id;

extern int need_draw;

extern int vertex_offset;

extern int cache_count[];

/***************************************************/
/** CACHE FUNCTIONS                                */
/***************************************************/

/**
* Use cache functions in order to avoid uploading
* the same vertices and indices multiple times.
*/

inline static int molehill_calcHash( float* vertices, int vertex_size )
{
	int i, k, n;
	
	k = 7 * vertex_size; 
	n = 144 * vertex_size;
	
	float hash = 0.0f;
	
	for ( i = 0 ; i < n ; i += k )
	{
		hash += vertices[i+0];
		hash += vertices[i+1];
		hash += vertices[i+2];
	}
	
	if ( hash < -999.999f )
		hash *= -1.0f;
	else if ( hash < 0.0f )
		hash *= -999.999f;
	else if ( hash < +999.999f )
		hash *= +999.999f;
		
	return (int)hash % 65536;
}

inline static void molehill_uploadCache( int hash )
{
	*masm_ptr = MOLEHILL_UPLOAD_CACHE;			masm_ptr++;
	*(int*)masm_ptr = hash;						masm_ptr++;
}

inline static void molehill_drawCache( int hash )
{
	float4x4 final_matrix;
	matrixMultiply( &final_matrix, projection_matrix, world_matrix );
	
	float* final_matrix_ptr = (float*)&final_matrix;
	
	*masm_ptr = MOLEHILL_MATRIX;				masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[ 0];	masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[ 1];	masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[ 2];	masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[ 3];	masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[ 4];	masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[ 5];	masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[ 6];	masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[ 7];	masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[ 8];	masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[ 9];	masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[10];	masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[11];	masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[12];	masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[13];	masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[14];	masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[15];	masm_ptr++;
	
	*masm_ptr = MOLEHILL_DRAW_CACHE;			masm_ptr++;
	*(int*)masm_ptr = hash;						masm_ptr++;
	
	need_draw = 0;
	
	vertex_offset = 0;
}

/***************************************************/
/** DRAW FUNCTION                                  */
/***************************************************/

inline static void molehill_draw()
{
	float4x4 final_matrix;
	matrixMultiply( &final_matrix, projection_matrix, world_matrix );
	
	float* final_matrix_ptr = (float*)&final_matrix;
	
	*masm_ptr = MOLEHILL_MATRIX;				masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[ 0];	masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[ 1];	masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[ 2];	masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[ 3];	masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[ 4];	masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[ 5];	masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[ 6];	masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[ 7];	masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[ 8];	masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[ 9];	masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[10];	masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[11];	masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[12];	masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[13];	masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[14];	masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[15];	masm_ptr++;
	
	*masm_ptr = MOLEHILL_DRAW;					masm_ptr++;

	need_draw = 0;
	
	vertex_offset = 0;
}

/***************************************************/
/** BUFFER FUNCTIONS                               */
/***************************************************/

inline static void molehill_clear( float red, float green, float blue, float alpha, float depth, int stencil )
{
	*masm_ptr = MOLEHILL_CLEAR;					masm_ptr++;
	*(float*)masm_ptr = red;					masm_ptr++;
	*(float*)masm_ptr = green;					masm_ptr++;
	*(float*)masm_ptr = blue;					masm_ptr++;
	*(float*)masm_ptr = alpha;					masm_ptr++;
	*(float*)masm_ptr = depth;					masm_ptr++;
	*(int*)masm_ptr = stencil;					masm_ptr++;
}

inline static void molehill_clearDepthStencil( float depth, int stencil )
{
	*masm_ptr = MOLEHILL_CLEAR_EX;				masm_ptr++;
	*(float*)masm_ptr = depth;					masm_ptr++;
	*(int*)masm_ptr = stencil;					masm_ptr++;
}

inline static void molehill_flush()
{
	if ( need_draw ) molehill_draw();
	
	*masm_ptr = MOLEHILL_FLUSH;
	masm_ptr++;
}

/***************************************************/
/** VERTEX FUNCTIONS                               */
/***************************************************/

inline static void molehill_begin( int type )
{
	if ( polygon_type != type )
	{
		if ( need_draw ) molehill_draw();
	}

	polygon_type = type;
	
	vertex_count = 0;
	index_count = 0;
	
	color_r = 1.0f;
	color_g = 1.0f;
	color_b = 1.0f;
	color_a = 1.0f;
	
	texcoord_u = 0.0f;
	texcoord_v = 0.0f;
	
	sum_vertex_x = 0.0f;
	sum_vertex_y = 0.0f;
	sum_vertex_z = 0.0f;
	sum_vertex_w = 0.0f;

	sum_color_r = 0.0f;
	sum_color_g = 0.0f;
	sum_color_b = 0.0f;
	sum_color_a = 0.0f;
	
	sum_texcoord_u = 0.0f;
	sum_texcoord_v = 0.0f;

	use_texturing = 0;
}

inline static void molehill_setVertex( float x, float y, float z, float w )
{
	int p = 4 * vertex_count;
	
	vertices[p + 0] = x;
	vertices[p + 1] = y;
	vertices[p + 2] = z;
	vertices[p + 3] = w;
	
	colors[p + 0] = color_r;
	colors[p + 1] = color_g;
	colors[p + 2] = color_b;
	colors[p + 3] = color_a;
	
	sum_vertex_x += x;
	sum_vertex_y += y;
	sum_vertex_z += z;
	sum_vertex_w += w;
	
	sum_color_r += color_r;
	sum_color_g += color_g;
	sum_color_b += color_b;
	sum_color_a += color_a;
	
	if ( use_texturing )
	{
		p = 2 * vertex_count;
		
		texcoords[p + 0] = texcoord_u;
		texcoords[p + 1] = texcoord_v;
		
		sum_texcoord_u += texcoord_u;
		sum_texcoord_v += texcoord_v;
	}

	vertex_count++;
}

inline static void molehill_setColor( float r, float g, float b, float a )
{
	color_r = r;
	color_g = g;
	color_b = b;
	color_a = a;
}

inline static void molehill_setTexcoord( float u, float v )
{
	texcoord_u = u;
	texcoord_v = v;
	
	use_texturing = 1;
}

inline static void molehill_end()
{
	int k;
	int n;
	int p0;
	int p1;
	
	if ( vertex_count == 0 ) return;
	
	if ( use_texturing )
	{
		*masm_ptr = MOLEHILL_VERTICES + 0x0A;		masm_ptr++;
		*(int*)masm_ptr = vertex_count;				masm_ptr++;
		
		for ( k = 0, p0 = 0, p1 = 0 ; k < vertex_count ; k++, p0 += 4, p1 += 2 )
		{
			*(float*)masm_ptr = vertices[p0 + 0];		masm_ptr++;
			*(float*)masm_ptr = vertices[p0 + 1];		masm_ptr++;
			*(float*)masm_ptr = vertices[p0 + 2];		masm_ptr++;
			*(float*)masm_ptr = vertices[p0 + 3];		masm_ptr++;
			
			*(float*)masm_ptr = colors[p0 + 0];			masm_ptr++;
			*(float*)masm_ptr = colors[p0 + 1];			masm_ptr++;
			*(float*)masm_ptr = colors[p0 + 2];			masm_ptr++;
			*(float*)masm_ptr = colors[p0 + 3];			masm_ptr++;
			
			*(float*)masm_ptr = texcoords[p1 + 0];		masm_ptr++;
			*(float*)masm_ptr = texcoords[p1 + 1];		masm_ptr++;
		}
	}
	else
	{
		*masm_ptr = MOLEHILL_VERTICES + 0x08;		masm_ptr++;
		*(int*)masm_ptr = vertex_count;				masm_ptr++;
		
		for ( k = 0, p0 = 0 ; k < vertex_count ; k++, p0 += 4 )
		{
			*(float*)masm_ptr = vertices[p0 + 0];		masm_ptr++;
			*(float*)masm_ptr = vertices[p0 + 1];		masm_ptr++;
			*(float*)masm_ptr = vertices[p0 + 2];		masm_ptr++;
			*(float*)masm_ptr = vertices[p0 + 3];		masm_ptr++;
			
			*(float*)masm_ptr = colors[p0 + 0];			masm_ptr++;
			*(float*)masm_ptr = colors[p0 + 1];			masm_ptr++;
			*(float*)masm_ptr = colors[p0 + 2];			masm_ptr++;
			*(float*)masm_ptr = colors[p0 + 3];			masm_ptr++;
		}
	}
	
	if ( polygon_type == MOLEHILL_TRIANGLE_LIST )
	{
		index_count = vertex_count;
	
		*masm_ptr = MOLEHILL_INDICES;				masm_ptr++;
		*(int*)masm_ptr = index_count;				masm_ptr++;
		
		n = vertex_count - 1;

		for ( k = 0 ; k <= n ; k++ )
		{
			*(int*)masm_ptr = vertex_offset + k;		masm_ptr++;
		}
	}
	else if ( polygon_type == MOLEHILL_TRIANGLE_STRIP )
	{
		index_count = 3 * (vertex_count - 2);
	
		*masm_ptr = MOLEHILL_INDICES;				masm_ptr++;
		*(int*)masm_ptr = index_count;				masm_ptr++;
		
		n = vertex_count - 3;
		
		for ( k = 0 ; k <= n ; k++ )
		{
			*(int*)masm_ptr = vertex_offset + k + 2;	masm_ptr++;
			*(int*)masm_ptr = vertex_offset + k + 1;	masm_ptr++;
			*(int*)masm_ptr = vertex_offset + k + 0;	masm_ptr++;
		}
	}
	else if ( polygon_type == MOLEHILL_TRIANGLE_FAN )
	{
		index_count = 3 * (vertex_count - 2);
	
		*masm_ptr = MOLEHILL_INDICES;				masm_ptr++;
		*(int*)masm_ptr = index_count;				masm_ptr++;
		
		n = vertex_count - 2;

		for ( k = 1 ; k <= n ; k++ )
		{
			*(int*)masm_ptr = vertex_offset + k + 1;	masm_ptr++;
			*(int*)masm_ptr = vertex_offset + k + 0;	masm_ptr++;
			*(int*)masm_ptr = vertex_offset + 0;		masm_ptr++;
		}
	}
	else if ( polygon_type == MOLEHILL_QUAD_LIST )
	{
		index_count = 3 * vertex_count / 2;
	
		*masm_ptr = MOLEHILL_INDICES;				masm_ptr++;
		*(int*)masm_ptr = index_count;				masm_ptr++;
		
		n = vertex_count - 1;

		for ( k = 0 ; k <= n ; k += 4 )
		{
			*(int*)masm_ptr = vertex_offset + k + 0;	masm_ptr++;
			*(int*)masm_ptr = vertex_offset + k + 1;	masm_ptr++;
			*(int*)masm_ptr = vertex_offset + k + 2;	masm_ptr++;
			*(int*)masm_ptr = vertex_offset + k + 0;	masm_ptr++;
			*(int*)masm_ptr = vertex_offset + k + 2;	masm_ptr++;
			*(int*)masm_ptr = vertex_offset + k + 3;	masm_ptr++;
		}
	}
	else if ( polygon_type == MOLEHILL_POLYGON )
	{
		index_count = 3 * (vertex_count - 2);
	
		*masm_ptr = MOLEHILL_INDICES;				masm_ptr++;
		*(int*)masm_ptr = index_count;				masm_ptr++;
		
		n = vertex_count - 2;

		for ( k = 1 ; k <= n ; k++ )
		{
			*(int*)masm_ptr = vertex_offset + k + 1;	masm_ptr++;
			*(int*)masm_ptr = vertex_offset + k + 0;	masm_ptr++;
			*(int*)masm_ptr = vertex_offset + 0;		masm_ptr++;
		}
	}
	
	need_draw = 1;
	
	vertex_offset += vertex_count;
}

/***************************************************/
/** TEXTURE FUNCTIONS                              */
/***************************************************/

inline static void molehill_bindTexture( int id )
{
	if ( texture_id != id )
	{
		if ( need_draw ) molehill_draw();
		
		texture_id = id;
		
		*masm_ptr = MOLEHILL_BIND;					masm_ptr++;
		*(int*)masm_ptr = id;						masm_ptr++;
	}
}

inline static void molehill_uploadTexture( int format, int level, int width, int height, void* data )
{
	typedef unsigned int uint;
	typedef unsigned char byte;
	
	static uint pixels[1024*1024];
	
	int k, n;
	
	byte a, r, g, b, i;
	
	byte* src = (byte*)data; 
	byte* dst = (byte*)pixels; 
	
	if ( format == MOLEHILL_I8 )
	{
		for ( k = 0, n = width * height ; k < n ; k++ )
		{
			i = *(src++);
			
			*(dst++) = i;
			*(dst++) = i;
			*(dst++) = i;
			*(dst++) = i;
		}
	}
	else if ( format == MOLEHILL_B8G8R8 )
	{
		for ( k = 0, n = width * height ; k < n ; k++ )
		{
			r = *(src++);
			g = *(src++);
			b = *(src++);
			a = *(src++);
			
			*(dst++) = b;
			*(dst++) = g;
			*(dst++) = r;
			*(dst++) = 0xFF;
		}
	}
	else if ( format == MOLEHILL_A8B8G8R8 )
	{
		for ( k = 0, n = width * height ; k < n ; k++ )
		{
			r = *(src++);
			g = *(src++);
			b = *(src++);
			a = *(src++);
			
			*(dst++) = b;
			*(dst++) = g;
			*(dst++) = r;
			*(dst++) = a;
		}
	}
	else
	{
		trace( "ERROR in molehill_uploadTexture( %d, %d, %d, %d, %d )", format, level, width, height, data ); 
	}
	
	AS3_Val bind_params = AS3_Array( "AS3ValType", AS3_Int(texture_id) );
	AS3_CallS( "bindTexture", _renderer, bind_params );
	
	AS3_Val upload_params = AS3_Array( "AS3ValType,AS3ValType,AS3ValType,AS3ValType", AS3_Int(level), AS3_Int(width), AS3_Int(height), AS3_Ptr(pixels) );
	AS3_CallS( "uploadTexture", _renderer, upload_params );
}

/***************************************************/
/** RENDER FUNCTIONS                               */
/***************************************************/

inline static void molehill_setBlending( int srcBlend, int destBlend )
{
	if ( need_draw ) molehill_draw();
	
	*masm_ptr = MOLEHILL_SET_BLENDING + (srcBlend << 8) + (destBlend << 0);
	masm_ptr++;
}

inline static void molehill_setDepthWrite( int value )
{
	if ( need_draw ) molehill_draw();
	
	*masm_ptr = MOLEHILL_SET_DEPTHWRITE + value;
	masm_ptr++;
}

inline static void molehill_setDepthFunc( int func )
{
	if ( need_draw ) molehill_draw();
	
	*masm_ptr = MOLEHILL_SET_DEPTHFUNC + func;
	masm_ptr++;
}

inline static void molehill_setViewport( int left, int top, int width, int height )
{
	if ( need_draw ) molehill_draw();
	
	*masm_ptr = MOLEHILL_SET_VIEWPORT;			masm_ptr++;
	*(int*)masm_ptr = left;						masm_ptr++;
	*(int*)masm_ptr = top;						masm_ptr++;
	*(int*)masm_ptr = width;					masm_ptr++;
	*(int*)masm_ptr = height;					masm_ptr++;
}

inline static void molehill_setCulling( int cullAway )
{
	if ( need_draw ) molehill_draw();
	
	*masm_ptr = MOLEHILL_SET_CULLING + cullAway;
	masm_ptr++;
}

inline static void molehill_setAlphaTest( int value )
{
	if ( need_draw ) molehill_draw();
	
	*masm_ptr = MOLEHILL_SET_ALPHATEST + value;
	masm_ptr++;
}

inline static void molehill_setAlphaFunc( int func, float reference )
{
	if ( need_draw ) molehill_draw();
	
	*masm_ptr = MOLEHILL_SET_ALPHAFUNC + func;	masm_ptr++;
	*(float*)masm_ptr = reference;				masm_ptr++;
}

inline static void molehill_setPolygonOffset( int value, float factor, float unit )
{
	if ( need_draw ) molehill_draw();
	
	*masm_ptr = MOLEHILL_SET_POLYGONOFFSET + value;		masm_ptr++;
	*(float*)masm_ptr = factor;							masm_ptr++;
	*(float*)masm_ptr = unit;							masm_ptr++;
}

inline static void molehill_setDepthRange( int value, float near, float far )
{
	if ( need_draw ) molehill_draw();
	
	*masm_ptr = MOLEHILL_SET_DEPTHRANGE + value;		masm_ptr++;
	*(float*)masm_ptr = near;							masm_ptr++;
	*(float*)masm_ptr = far;							masm_ptr++;
}

inline static void molehill_setTextureWrapMode( int value )
{
	if ( need_draw ) molehill_draw();
	
	*masm_ptr = MOLEHILL_SET_TEXTUREWRAPMODE + value;
	masm_ptr++;
}

/***************************************************/
/** MATRIX FUNCTIONS                               */
/***************************************************/

inline static void molehill_identity()
{
	if ( need_draw ) molehill_draw();
	
	matrixIdentity( matrix );
}

inline static void molehill_translate( float x, float y, float z )
{
	if ( need_draw ) molehill_draw();
	
	float4x4 original_matrix;
	float4x4 operand_matrix;
	
	matrixClone( &original_matrix, *matrix );
	matrixTranslation( &operand_matrix, x, y, z );
	matrixMultiply( matrix, original_matrix, operand_matrix );
}

inline static void molehill_rotate( float angle, float x, float y, float z )
{
	if ( need_draw ) molehill_draw();
	
	float4x4 original_matrix;
	float4x4 operand_matrix;
	
	matrixClone( &original_matrix, *matrix );
	matrixRotationAxis( &operand_matrix, angle * M_PI / 180.0, x, y, z );
	matrixMultiply( matrix, original_matrix, operand_matrix );
}

inline static void molehill_scale( float x, float y, float z )
{
	if ( need_draw ) molehill_draw();
	
	float4x4 original_matrix;
	float4x4 operand_matrix;
	
	matrixClone( &original_matrix, *matrix );
	matrixScale( &operand_matrix, x, y, z );
	matrixMultiply( matrix, original_matrix, operand_matrix );
}

inline static void molehill_frustum( float left, float right, float bottom, float top, float nearVal, float farVal )
{
	if ( need_draw ) molehill_draw();
	
	matrixFrustum( matrix, left, right, bottom, top, nearVal, farVal );
}

inline static void molehill_ortho( float left, float right, float bottom, float top, float nearVal, float farVal )
{
	if ( need_draw ) molehill_draw();
	
	matrixOrtho( matrix, left, right, bottom, top, nearVal, farVal );
}

inline static void molehill_push()
{
	if ( need_draw ) molehill_draw();
	
	memcpy( matrix_stack_ptr, matrix, 16 * sizeof(float) );
	
	matrix_stack_ptr += 1;
}

inline static void molehill_pop()
{
	if ( need_draw ) molehill_draw();
	
	matrix_stack_ptr -= 1;
	
	memcpy( matrix, matrix_stack_ptr, 16 * sizeof(float) );
}

inline static void molehill_setMatrixType( int type )
{
	if ( need_draw ) molehill_draw();
	
	if ( type == MOLEHILL_WORLD )
		matrix = &world_matrix;
	else if ( type == MOLEHILL_PROJECTION )
		matrix = &projection_matrix;
}

inline static void molehill_getMatrix( float* data )
{
	if ( need_draw ) molehill_draw();
	
	memcpy( data, matrix, 16 * sizeof(float) );
}

inline static void molehill_setMatrix( float* data )
{
	if ( need_draw ) molehill_draw();
	
	float4x4 temp; 
	memcpy( &temp, data, 16 * sizeof(float) );
	
	matrixTranspose( matrix, temp );
}

/***************************************************/
/** FAST DRAW FUNCTIONS                            */
/***************************************************/

inline static void molehill_setTexcoordPointer( void* data, int count )
{
	*masm_ptr = MOLEHILL_TEXCOORD_POINTER;		masm_ptr++;
	*(void**)masm_ptr = data;					masm_ptr++;
	*(int*)masm_ptr = count;					masm_ptr++;
}

inline static void molehill_setColorPointer( void* data, int count )
{
	*masm_ptr = MOLEHILL_COLOR_POINTER;			masm_ptr++;
	*(void**)masm_ptr = data;					masm_ptr++;
	*(int*)masm_ptr = count;					masm_ptr++;
}

inline static void molehill_setVertexPointer( void* data, int count )
{
	*masm_ptr = MOLEHILL_VERTEX_POINTER;		masm_ptr++;
	*(void**)masm_ptr = data;					masm_ptr++;
	*(int*)masm_ptr = count;					masm_ptr++;
}

inline static void molehill_setIndexPointer( void* data, int count )
{
	*masm_ptr = MOLEHILL_INDEX_POINTER;			masm_ptr++;
	*(void**)masm_ptr = data;					masm_ptr++;
	*(int*)masm_ptr = count;					masm_ptr++;
}

inline static void molehill_drawFast()
{
	float4x4 final_matrix;
	matrixMultiply( &final_matrix, projection_matrix, world_matrix );
	
	float* final_matrix_ptr = (float*)&final_matrix;
	
	*masm_ptr = MOLEHILL_MATRIX;				masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[ 0];	masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[ 1];	masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[ 2];	masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[ 3];	masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[ 4];	masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[ 5];	masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[ 6];	masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[ 7];	masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[ 8];	masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[ 9];	masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[10];	masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[11];	masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[12];	masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[13];	masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[14];	masm_ptr++;
	*(float*)masm_ptr = final_matrix_ptr[15];	masm_ptr++;

	*masm_ptr = MOLEHILL_DRAW_FAST;				masm_ptr++;
	
	extern void molehill_stop();
	extern void molehill_start();
	
	molehill_stop();
	molehill_start();
}

#endif