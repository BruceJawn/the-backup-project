/**
 * @author Pierre-Yves Gatouillat ==>> gatouillatpy@gmail.com
 */

#ifndef FLASH_GL_WRAPPER_H
#define FLASH_GL_WRAPPER_H

#include "molehill.h"

extern float _clear_depth;
extern float _clear_red;
extern float _clear_green;
extern float _clear_blue;
extern float _clear_alpha;

extern int _culling_mode;
extern int _culling_enabled;

extern int _blending_src_op;
extern int _blending_dst_op;

extern int _depth_func;

extern GLfloat* _vertex_array_pointer;
extern GLsizei _vertex_array_stride;
extern GLenum _vertex_array_type;
extern GLint _vertex_array_size;

extern GLuint* _color_array_pointer;
extern GLsizei _color_array_stride;
extern GLenum _color_array_type;
extern GLint _color_array_size;

extern GLfloat* _texcoord_array_pointer;
extern GLsizei _texcoord_array_stride;
extern GLenum _texcoord_array_type;
extern GLint _texcoord_array_size;

extern short _index_array_pointer[];
extern int _index_array_length;

extern GLenum _polygon_mode;

extern float _polygon_offset_factor;
extern float _polygon_offset_units;

inline static void qglGetIntegerv( GLenum pname, GLint* params )
{
	if ( pname == GL_MAX_TEXTURE_SIZE )
	{
		*params = 1024;
	}
	else
	{
		trace( "glGetIntegerv( pname=%d )\n", pname );
	}
}

inline static GLubyte* qglGetString( GLenum name )
{
    GLubyte* returnValue;
	
	if ( name == GL_VENDOR )
	{
		returnValue = (GLubyte*)"impulse12.com";
	}
	else if ( name == GL_RENDERER )
	{
		returnValue = (GLubyte*)"OpenGL wrapper for Adobe Flash 11";
	}
	else if ( name == GL_VERSION )
	{
		returnValue = (GLubyte*)"1.2";
	}
	else if ( name == GL_EXTENSIONS )
	{
		returnValue = (GLubyte*)"NONE";
	}
	else
	{
		trace( "glGetString( name=%lu )\n", name );
	}
	
    return returnValue;
}

inline static GLenum qglGetError()
{
    GLenum returnValue = GL_NO_ERROR;
	
    return returnValue;
}

inline static void qglBindTexture( GLenum target, GLuint texture )
{
	if ( target == GL_TEXTURE_2D )
	{
		molehill_bindTexture( (int)texture );
	}
    else
	{
		trace( "glBindTexture( target=%lu, texture=%lu )\n", target, texture );
	}
}

inline static void qglTexImage2D( GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, GLvoid *pixels )
{
 	if ( target == GL_TEXTURE_2D )
	{
		if ( internalFormat == 1 )
			molehill_uploadTexture( MOLEHILL_I8, (int)level, (int)width, (int)height, (void*)pixels );
		else if ( internalFormat == 3 )
			molehill_uploadTexture( MOLEHILL_B8G8R8, (int)level, (int)width, (int)height, (void*)pixels );
		else if ( internalFormat == 4 )
			molehill_uploadTexture( MOLEHILL_A8B8G8R8, (int)level, (int)width, (int)height, (void*)pixels );
		else if ( internalFormat == GL_RGB8 )
			molehill_uploadTexture( MOLEHILL_B8G8R8, (int)level, (int)width, (int)height, (void*)pixels );
		else
			trace( "glTexImage2D( target=%lu, level=%ld, internalFormat=%ld, width=%ld, height=%ld, border=%ld, format=%lu, type=%lu, pixels=%p )\n", target, level, internalFormat, width, height, border, format, type, pixels );
	}
    else
	{
	    trace( "glTexImage2D( target=%lu, level=%ld, internalFormat=%ld, width=%ld, height=%ld, border=%ld, format=%lu, type=%lu, pixels=%p )\n", target, level, internalFormat, width, height, border, format, type, pixels );
	}
}

inline static void qglTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels )
{
 	if ( target == GL_TEXTURE_2D )
	{
		molehill_uploadTexture( MOLEHILL_A8B8G8R8, (int)level, (int)width, (int)height, (void*)pixels );
	}
    else
	{
		trace( "glTexSubImage2D( target=%lu, level=%ld, xoffset=%ld, yoffset=%ld, width=%ld, height=%ld, format=%lu, type=%lu, pixels=%p )\n", target, level, xoffset, yoffset, width, height, format, type, pixels );
	}
}

inline static void qglTexParameterf( GLenum target, GLenum pname, GLfloat param )
{
 	if ( target == GL_TEXTURE_2D )
	{
		if ( pname == GL_TEXTURE_WRAP_S || pname == GL_TEXTURE_WRAP_T )
		{
			if ( param == GL_CLAMP )
				molehill_setTextureWrapMode( MOLEHILL_CLAMP );
			else
				molehill_setTextureWrapMode( MOLEHILL_REPEAT );
		}
	}
    else
	{
		trace( "glTexParameterf( target=%lu, pname=%lu, param=%f )\n", target, pname, param );
	}
}

inline static void qglTexParameterfv( GLenum target, GLenum pname, GLfloat *params )
{
 	if ( target == GL_TEXTURE_2D )
	{
		// do nothing
	}
    else
	{
		trace( "glTexParameterfv( target=%lu, pname=%lu, params=%p )\n", target, pname, params );
	}
}

inline static void qglClearColor( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha )
{
	_clear_red = (float)red;
	_clear_green = (float)green;
	_clear_blue = (float)blue;
	_clear_alpha = (float)alpha;
}

inline static void qglClearDepth( GLclampd depth )
{
	_clear_depth = (float)depth;
}

inline static void qglCullFace( GLenum mode )
{
 	if ( mode == GL_FRONT )
	{
		_culling_mode = MOLEHILL_BACK;
		
		if ( _culling_enabled )
			molehill_setCulling( _culling_mode );
	}
 	else if ( mode == GL_BACK )
	{
		_culling_mode = MOLEHILL_FRONT;
		
		if ( _culling_enabled )
			molehill_setCulling( _culling_mode );
	}
    else
	{
		trace( "glCullFace( mode=%lu )\n", mode );
	}
}

inline static void qglColor4f( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha )
{
    molehill_setColor( (float)red, (float)green, (float)blue, (float)alpha );
}

inline static void qglEnable( GLenum cap )
{
   	if ( cap == GL_CULL_FACE )
	{
		_culling_enabled = 1;

		molehill_setCulling( _culling_mode );
	}
 	else if ( cap == GL_TEXTURE_2D )
	{
		// do nothing
	}
 	else if ( cap == GL_SCISSOR_TEST )
	{
		// TODO : use clipping planes when molehill will integrate it
	}
 	else if ( cap == GL_ALPHA_TEST )
	{
		molehill_setAlphaTest( MOLEHILL_TRUE );
	}
 	else if ( cap == GL_POLYGON_OFFSET_FILL )
	{
		molehill_setPolygonOffset( MOLEHILL_TRUE, _polygon_offset_factor, _polygon_offset_units );
	}
 	else if ( cap == GL_BLEND )
	{
		molehill_setBlending( _blending_src_op, _blending_dst_op );
	}
 	else if ( cap == GL_DEPTH_TEST )
	{
		molehill_setDepthFunc( _depth_func );
	}
    else
	{
		trace( "glEnable( cap=%lu )\n", cap );
	}
}

inline static void qglDisable( GLenum cap )
{
  	if ( cap == GL_CULL_FACE )
	{
		_culling_enabled = 0;
		
		molehill_setCulling( MOLEHILL_NONE );
	}
 	else if ( cap == GL_STENCIL_TEST )
	{
		// do nothing
	}
 	else if ( cap == GL_ALPHA_TEST )
	{
		molehill_setAlphaTest( MOLEHILL_FALSE );
	}
 	else if ( cap == GL_POLYGON_OFFSET_FILL )
	{
		molehill_setPolygonOffset( MOLEHILL_FALSE, _polygon_offset_factor, _polygon_offset_units );
	}
 	else if ( cap == GL_BLEND )
	{
		molehill_setBlending( MOLEHILL_ONE, MOLEHILL_ZERO );
	}
 	else if ( cap == GL_DEPTH_TEST )
	{
		molehill_setDepthFunc( MOLEHILL_ALWAYS );
	}
 	else if ( cap == GL_CLIP_PLANE0 )
	{
		// do nothing
	}
    else
	{
		trace( "glDisable( cap=%lu )\n", cap );
	}
}

inline static void qglTexEnvf( GLenum target, GLenum pname, GLfloat param )
{
 	if ( target == GL_TEXTURE_ENV )
	{
		// do nothing
	}
    else
	{
		trace( "glTexEnvf( target=%lu, pname=%lu, param=%f )\n", target, pname, param );
	}
}

inline static void qglShadeModel( GLenum mode )
{
    // do nothing
}

inline static void qglDepthFunc( GLenum func )
{
 	if ( func == GL_LEQUAL )
	{
		_depth_func = MOLEHILL_LESS_OR_EQUAL;
		
		molehill_setDepthFunc( _depth_func );
	}
	else if ( func == GL_EQUAL )
	{
		_depth_func = MOLEHILL_EQUAL;
		
		molehill_setDepthFunc( _depth_func );
	}
	else if ( func == GL_ALWAYS )
	{
		_depth_func = MOLEHILL_ALWAYS;
		
		molehill_setDepthFunc( _depth_func );
	}
    else
	{
		trace( "glDepthFunc( func=%lu )\n", func );
	}
}

inline static void qglEnableClientState( GLenum cap )
{
    if ( cap == GL_VERTEX_ARRAY )
	{
		// do nothing
	}
	else if ( cap == GL_COLOR_ARRAY )
	{
		// do nothing
	}
	else if ( cap == GL_TEXTURE_COORD_ARRAY )
	{
		// do nothing
	}
    else
	{
		trace( "qglEnableClientState( cap=%lu )\n", cap );
	}
}

inline static void qglDisableClientState( GLenum cap )
{
    if ( cap == GL_VERTEX_ARRAY )
	{
		_vertex_array_pointer = 0;
		_vertex_array_stride = 0;
		_vertex_array_type = GL_FLOAT;
		_vertex_array_size = 4;
	}
	else if ( cap == GL_COLOR_ARRAY )
	{
		_color_array_pointer = 0;
		_color_array_stride = 0;
		_color_array_type = GL_FLOAT;
		_color_array_size = 4;
	}
	else if ( cap == GL_TEXTURE_COORD_ARRAY )
	{
		_texcoord_array_pointer = 0;
		_texcoord_array_stride = 0;
		_texcoord_array_type = GL_FLOAT;
		_texcoord_array_size = 4;
	}
    else
	{
		trace( "qglDisableClientState( cap=%lu )\n", cap );
	}
}

inline static void qglPolygonMode( GLenum face, GLenum mode )
{
  	if ( mode == GL_FILL )
	{
		 // do nothing
	}
    else
	{
		trace( "glPolygonMode( face=%lu, mode=%lu )\n", face, mode );
	}
}

inline static void qglDepthMask( GLboolean flag )
{
  	if ( flag == GL_TRUE )
	{
		molehill_setDepthWrite( MOLEHILL_TRUE );
	}
  	else if ( flag == GL_FALSE )
	{
		molehill_setDepthWrite( MOLEHILL_FALSE );
	}
    else
	{
		trace( "glDepthMask( flag=%u )\n", flag );
	}
}

inline static void qglViewport( GLint x, GLint y, GLsizei width, GLsizei height )
{
	molehill_setViewport( (int)x, (int)y, (int)width, (int)height );
}

inline static void qglScissor( GLint x, GLint y, GLsizei width, GLsizei height )
{
	//trace( "glScissor( x=%ld, y=%ld, width=%ld, height=%ld )\n", x, y, width, height );
}

inline static void qglMatrixMode( GLenum mode )
{
	if ( mode == GL_MODELVIEW )
	{
		molehill_setMatrixType( MOLEHILL_WORLD );
	}
	else if ( mode == GL_PROJECTION )
	{
		molehill_setMatrixType( MOLEHILL_PROJECTION );
	}
	else
	{
		trace( "glMatrixMode( mode=%lu )\n", mode );
	}
}

inline static void qglLoadIdentity()
{
	molehill_identity();
}

inline static void qglBlendFunc( GLenum sfactor, GLenum dfactor )
{
	if ( sfactor == GL_SRC_ALPHA && dfactor == GL_ONE_MINUS_SRC_ALPHA )
	{
		_blending_src_op = MOLEHILL_SRC_ALPHA;
		_blending_dst_op = MOLEHILL_ONE_MINUS_SRC_ALPHA;
		
		molehill_setBlending( _blending_src_op, _blending_dst_op );
	}
	else if ( sfactor == GL_ONE && dfactor == GL_ONE )
	{
		_blending_src_op = MOLEHILL_ONE;
		_blending_dst_op = MOLEHILL_ONE;
		
		molehill_setBlending( _blending_src_op, _blending_dst_op );
	}
	else if ( sfactor == GL_ONE && dfactor == GL_ZERO )
	{
		_blending_src_op = MOLEHILL_ONE;
		_blending_dst_op = MOLEHILL_ZERO;
		
		molehill_setBlending( _blending_src_op, _blending_dst_op );
	}
	else if ( sfactor == GL_ZERO && dfactor == GL_ONE_MINUS_SRC_COLOR )
	{
		_blending_src_op = MOLEHILL_ZERO;
		_blending_dst_op = MOLEHILL_ONE_MINUS_SRC_COLOR;
		
		molehill_setBlending( _blending_src_op, _blending_dst_op );
	}
	else if ( sfactor == GL_DST_COLOR && dfactor == GL_ZERO )
	{
		_blending_src_op = MOLEHILL_DST_COLOR;
		_blending_dst_op = MOLEHILL_ZERO;
		
		molehill_setBlending( _blending_src_op, _blending_dst_op );
	}
	else if ( sfactor == GL_DST_COLOR && dfactor == GL_SRC_COLOR )
	{
		_blending_src_op = MOLEHILL_DST_COLOR;
		_blending_dst_op = MOLEHILL_SRC_COLOR;
		
		molehill_setBlending( _blending_src_op, _blending_dst_op );
	}
	else if ( sfactor == GL_DST_COLOR && dfactor == GL_ONE_MINUS_DST_ALPHA )
	{
		_blending_src_op = MOLEHILL_DST_COLOR;
		_blending_dst_op = MOLEHILL_ONE_MINUS_DST_ALPHA;
		
		molehill_setBlending( _blending_src_op, _blending_dst_op );
	}
	else if ( sfactor == GL_DST_COLOR && dfactor == GL_ONE )
	{
		_blending_src_op = MOLEHILL_DST_COLOR;
		_blending_dst_op = MOLEHILL_ONE;
		
		molehill_setBlending( _blending_src_op, _blending_dst_op );
	}
	else if ( sfactor == GL_DST_COLOR && dfactor == GL_SRC_ALPHA )
	{
		_blending_src_op = MOLEHILL_DST_COLOR;
		_blending_dst_op = MOLEHILL_SRC_ALPHA;
		
		molehill_setBlending( _blending_src_op, _blending_dst_op );
	}
	else
	{
		trace( "glBlendFunc( sfactor=%lu, dfactor=%lu )\n", sfactor, dfactor );
	}
}

inline static void qglDrawBuffer( GLenum mode )
{
    // do nothing
}

inline static void qglFinish()
{
    // do nothing
}

inline static void qglOrtho( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val )
{
	molehill_ortho( (float)left, (float)right, (float)bottom, (float)top, (float)near_val, (float)far_val );
}

inline static void qglColor3f( GLfloat red, GLfloat green, GLfloat blue )
{
    molehill_setColor( (float)red, (float)green, (float)blue, 1.0f );
}

inline static void qglBegin( GLenum mode )
{
	_polygon_mode = mode;
	
	_index_array_length = 0;
	
	if ( mode == GL_TRIANGLES )
	{
		molehill_begin( MOLEHILL_TRIANGLE_LIST );
	}
	else if ( mode == GL_TRIANGLE_STRIP )
	{
		molehill_begin( MOLEHILL_TRIANGLE_STRIP );
	}
	else if ( mode == GL_TRIANGLE_FAN )
	{
		molehill_begin( MOLEHILL_TRIANGLE_FAN );
	}
	else if ( mode == GL_QUADS )
	{
		molehill_begin( MOLEHILL_QUAD_LIST );
	}
	else if ( mode == GL_POLYGON )
	{
		molehill_begin( MOLEHILL_POLYGON );
	}
	else
	{
		trace( "glBegin( mode=%lu )\n", mode );
	}
}

inline static void qglTexCoord2f( GLfloat s, GLfloat t )
{
    molehill_setTexcoord( (float)s, (float)t );
}

inline static void qglVertex2f( GLfloat x, GLfloat y )
{
    molehill_setVertex( (float)x, (float)y, 0.0f, 1.0f );
}

inline static GLsizei qglGetArrayStride( GLint size, GLenum type )
{
	if ( type == GL_FLOAT )
		return size * sizeof(GLfloat);
	else if ( type == GL_DOUBLE )
		return size * sizeof(GLdouble);
	else if ( type == GL_INT )
		return size * sizeof(GLint);
	else if ( type == GL_UNSIGNED_INT )
		return size * sizeof(GLuint);
	else if ( type == GL_SHORT )
		return size * sizeof(GLshort);
	else if ( type == GL_UNSIGNED_SHORT )
		return size * sizeof(GLushort);
	else if ( type == GL_BYTE )
		return size * sizeof(GLbyte);
	else if ( type == GL_UNSIGNED_BYTE )
		return size * sizeof(GLubyte);
	else
		return 0;
}

inline static float qglGetVertexArrayValue( GLint index, GLint offset, float value )
{
	char* ptr = (char*)_vertex_array_pointer;
	
	if ( offset >= _vertex_array_size )
		return value;
	else if ( _vertex_array_type == GL_FLOAT )
		return (float)*(((GLfloat*)(ptr+index*_vertex_array_stride))+offset);
	else if ( _vertex_array_type == GL_DOUBLE )
		return (float)*(((GLdouble*)(ptr+index*_vertex_array_stride))+offset);
	else if ( _vertex_array_type == GL_INT )
		return (float)*(((GLint*)(ptr+index*_vertex_array_stride))+offset);
	else if ( _vertex_array_type == GL_SHORT )
		return (float)*(((GLshort*)(ptr+index*_vertex_array_stride))+offset);
	else
		return value;
}

inline static float qglGetColorArrayValue( GLint index, GLint offset, float value )
{
	char* ptr = (char*)_color_array_pointer;

	if ( offset >= _color_array_size )
		return value;
	else if ( _color_array_type == GL_FLOAT )
		return (float)*(((GLfloat*)(ptr+index*_color_array_stride))+offset);
	else if ( _color_array_type == GL_DOUBLE )
		return (float)*(((GLdouble*)(ptr+index*_color_array_stride))+offset);
	else if ( _color_array_type == GL_INT )
		return (float)*(((GLint*)(ptr+index*_color_array_stride))+offset) / 255.0f;
	else if ( _color_array_type == GL_UNSIGNED_INT )
		return (float)*(((GLuint*)(ptr+index*_color_array_stride))+offset) / 255.0f;
	else if ( _color_array_type == GL_SHORT )
		return (float)*(((GLshort*)(ptr+index*_color_array_stride))+offset) / 255.0f;
	else if ( _color_array_type == GL_UNSIGNED_SHORT )
		return (float)*(((GLushort*)(ptr+index*_color_array_stride))+offset) / 255.0f;
	else if ( _color_array_type == GL_BYTE )
		return (float)*(((GLbyte*)(ptr+index*_color_array_stride))+offset) / 255.0f;
	else if ( _color_array_type == GL_UNSIGNED_BYTE )
		return (float)*(((GLubyte*)(ptr+index*_color_array_stride))+offset) / 255.0f;
	else
		return value;
}

inline static float qglGetTexcoordArrayValue( GLint index, GLint offset, float value )
{
	char* ptr = (char*)_texcoord_array_pointer;

	if ( offset >= _texcoord_array_size )
		return value;
	else if ( _texcoord_array_type == GL_FLOAT )
		return (float)*(((GLfloat*)(ptr+index*_texcoord_array_stride))+offset);
	else if ( _texcoord_array_type == GL_DOUBLE )
		return (float)*(((GLdouble*)(ptr+index*_texcoord_array_stride))+offset);
	else if ( _texcoord_array_type == GL_INT )
		return (float)*(((GLint*)(ptr+index*_texcoord_array_stride))+offset);
	else if ( _texcoord_array_type == GL_SHORT )
		return (float)*(((GLshort*)(ptr+index*_texcoord_array_stride))+offset);
	else
		return value;
}

inline static void qglAddArrayValue( GLint index )
{
	float x, y, z, w;
	float r, g, b, a;
	float u, v;
	
	if ( _texcoord_array_pointer )
	{
		u = qglGetTexcoordArrayValue( index, 0, 0.0f );
		v = qglGetTexcoordArrayValue( index, 1, 0.0f );
		
		molehill_setTexcoord( u, v );
	}
	
	if ( _color_array_pointer )
	{
		r = qglGetColorArrayValue( index, 0, 1.0f );
		g = qglGetColorArrayValue( index, 1, 1.0f );
		b = qglGetColorArrayValue( index, 2, 1.0f );
		a = qglGetColorArrayValue( index, 3, 1.0f );
		
		molehill_setColor( r, g, b, a );
	}
	
	if ( _vertex_array_pointer )
	{
		x = qglGetVertexArrayValue( index, 0, 0.0f );
		y = qglGetVertexArrayValue( index, 1, 0.0f );
		z = qglGetVertexArrayValue( index, 2, 0.0f );
		w = qglGetVertexArrayValue( index, 3, 1.0f );
		
		molehill_setVertex( x, y, z, w );
	}
}

inline static void qglEnd()
{
	if ( _index_array_length > 0 )
	{
		int k;
	
		for ( k = 0 ; k < _index_array_length ; k++ )
		{
			qglAddArrayValue( _index_array_pointer[k] );
		}
	}
	
    molehill_end();
}

inline static void qglFlush()
{
    molehill_flush();
}

inline static void qglTexCoordPointer( GLint size, GLenum type, GLsizei stride, GLvoid *ptr )
{
	_texcoord_array_pointer = ptr;
	_texcoord_array_stride = (stride == 0) ? qglGetArrayStride( size, type ) : stride;
	_texcoord_array_type = type;
	_texcoord_array_size = size;
}

inline static void qglColorPointer( GLint size, GLenum type, GLsizei stride, GLvoid *ptr )
{
	_color_array_pointer = ptr;
	_color_array_stride = (stride == 0) ? qglGetArrayStride( size, type ) : stride;
	_color_array_type = type;
	_color_array_size = size;
}

inline static void qglVertexPointer( GLint size, GLenum type, GLsizei stride, GLvoid *ptr )
{
	_vertex_array_pointer = ptr;
	_vertex_array_stride = (stride == 0) ? qglGetArrayStride( size, type ) : stride;
	_vertex_array_type = type;
	_vertex_array_size = size;
}

inline static void qglArrayElement( GLint i )
{
    _index_array_pointer[_index_array_length++] = (int)i;
}

inline static void qglClear( GLbitfield mask )
{
	if ( mask == GL_COLOR_BUFFER_BIT )
	{
		molehill_clear( _clear_red, _clear_green, _clear_blue, _clear_alpha, _clear_depth, 0 );
	}
	else if ( mask == GL_DEPTH_BUFFER_BIT )
	{
		molehill_clearDepthStencil( _clear_depth, 0 );
	}
	else if ( mask == GL_STENCIL_BUFFER_BIT )
	{
		molehill_clearDepthStencil( _clear_depth, 0 );
	}
	else
	{
		trace( "glClear( mask=%lu )\n", mask );
	}
}

inline static void qglLoadMatrixf( GLfloat *m )
{
	molehill_setMatrix( (float*)m ) ;
}

inline static void qglTexCoord2fv( GLfloat *v )
{
    molehill_setTexcoord( (float)v[0], (float)v[1] );
}

inline static void qglColor4ubv( GLubyte *v )
{
    molehill_setColor( (float)v[0] / 255.0f, (float)v[1] / 255.0f, (float)v[2] / 255.0f, (float)v[3] / 255.0f );
}

inline static void qglVertex3fv( GLfloat *v )
{
    molehill_setVertex( (float)v[0], (float)v[1], (float)v[2], 1.0f );
}

inline static void __qglDrawElements__( GLenum mode, GLsizei count, GLenum type, GLvoid *indices )
{
	int k;
	
	_polygon_mode = mode;
		
	if ( mode == GL_TRIANGLES )
	{
		molehill_begin( MOLEHILL_TRIANGLE_LIST );
	}
	else if ( mode == GL_TRIANGLE_STRIP )
	{
		molehill_begin( MOLEHILL_TRIANGLE_STRIP );
	}
	else if ( mode == GL_TRIANGLE_FAN )
	{
		molehill_begin( MOLEHILL_TRIANGLE_FAN );
	}
	else if ( mode == GL_QUADS )
	{
		molehill_begin( MOLEHILL_QUAD_LIST );
	}
	else if ( mode == GL_POLYGON )
	{
		molehill_begin( MOLEHILL_POLYGON );
	}
	else
	{
		trace( "glDrawElements( mode=%lu, count=%ld, type=%lu, indices=%p )\n", mode, count, type, indices );
		
		return;
	}

	if ( type == GL_UNSIGNED_INT )
	{
		GLuint* uint_data = (GLuint*)indices;
		
		for ( k = 0 ; k < count ; k++ )
		{
			qglAddArrayValue( uint_data[k] );
		}
	}
	else if ( type == GL_UNSIGNED_SHORT )
	{
		GLushort* ushort_data = (GLushort*)indices;
		
		for ( k = 0 ; k < count ; k++ )
		{
			qglAddArrayValue( ushort_data[k] );
		}
	}
	else if ( type == GL_UNSIGNED_BYTE )
	{
		GLubyte* ubyte_data = (GLubyte*)indices;
		
		for ( k = 0 ; k < count ; k++ )
		{
			qglAddArrayValue( ubyte_data[k] );
		}
	}
	else
	{
		trace( "glDrawElements( mode=%lu, count=%ld, type=%lu, indices=%p )\n", mode, count, type, indices );
		
		return;
	}
	
	molehill_end();
}

inline static void qglDrawElements( GLenum mode, GLsizei count, GLenum type, GLuint *indices )
{
#ifdef USE_MOLEHILL_CACHE
	int hash = molehill_calcHash( _vertex_array_pointer, _vertex_array_size );
	
	if ( cache_count[hash] <= 0 )
		cache_count[hash] = 3;
	else if ( cache_count[hash] < 0x3FFFFFFF )
		cache_count[hash] <<= 1;
		
	if ( cache_count[hash] < 0x007FFFFF )
	{
		// draw normally
		
		__qglDrawElements__( mode, count, type, indices );
	}
	else if ( cache_count[hash] < 0x00FFFFFE )
	{
		// upload and draw cache
		
		__qglDrawElements__( mode, count, type, indices );
		
		molehill_uploadCache( hash );
		molehill_drawCache( hash );
	}
	else
	{
		// draw cache
		
		molehill_drawCache( hash );
	}
#else
	int k;
	
	int max_index = 0;
	
	for ( k = 0 ; k < count ; k++ )
	{
		if ( indices[k] > max_index ) max_index = indices[k];
		
		_index_array_pointer[k] = (short)indices[k];
	}

	for ( k = 0 ; k <= max_index ; k++ )
	{
		//_color_array_pointer[k] = 0xffffffff;
		
		_vertex_array_pointer[4*k+3] = 1.0;
		
		//_texcoord_array_pointer[2*k+0] = 0.5;
		//_texcoord_array_pointer[2*k+1] = 0.5;

		//trace( "vertex[%d] x=%.3f y=%.3f z=%.3f w=%.3f", k, _vertex_array_pointer[4*k+0], _vertex_array_pointer[4*k+1], _vertex_array_pointer[4*k+2], _vertex_array_pointer[4*k+3] );
	}

	if ( type == GL_UNSIGNED_INT && mode == GL_TRIANGLES )
	{
		if ( _texcoord_array_pointer )
		{
			if ( _texcoord_array_size == 2 && _texcoord_array_type == GL_FLOAT && _texcoord_array_stride == 8 )
				molehill_setTexcoordPointer( _texcoord_array_pointer, max_index + 1 );
			else
				trace( "texcoords : size=%d type=%d stride=%d", _texcoord_array_size, _texcoord_array_type, _texcoord_array_stride );
		}
		
		if ( _color_array_pointer )
		{
			if ( _color_array_size == 4 && _color_array_type == GL_UNSIGNED_BYTE && _color_array_stride == 4 )
				molehill_setColorPointer( _color_array_pointer, max_index + 1 );
			else
				trace( "colors : size=%d type=%d stride=%d", _color_array_size, _color_array_type, _color_array_stride );
		}

		if ( _vertex_array_pointer )
		{
			if ( _vertex_array_size == 3 && _vertex_array_type == GL_FLOAT && _vertex_array_stride == 16 )
				molehill_setVertexPointer( _vertex_array_pointer, max_index + 1 );
			else
				trace( "vertices : size=%d type=%d stride=%d", _vertex_array_size, _vertex_array_type, _vertex_array_stride );
		}
		
		molehill_setIndexPointer( _index_array_pointer, count );
		
		molehill_drawFast();
	}
	else
	{
		__qglDrawElements__( mode, count, type, indices );
		
		trace( "indices : count=%d type=%d GL_UNSIGNED_INT=%d mode=%d GL_TRIANGLES=%d", count, type, GL_UNSIGNED_INT, mode, GL_TRIANGLES );
	}
#endif
}

inline static void qglAlphaFunc( GLenum func, GLclampf ref )
{
	if ( func == GL_GEQUAL )
	{
		molehill_setAlphaFunc( MOLEHILL_GREATER_OR_EQUAL, (float)ref );
	}
	else
	{
		trace( "glAlphaFunc( func=%lu, ref=%f )\n", func, ref );
	}
}

inline static void qglDepthRange( GLclampd near_val, GLclampd far_val )
{
	molehill_setDepthRange( MOLEHILL_TRUE, near_val, far_val );
	//trace( "glDepthRange( near_val=%f, far_val=%f )\n", near_val, far_val );
}

inline static void qglPolygonOffset( GLfloat factor, GLfloat units )
{
	_polygon_offset_factor = factor;
	_polygon_offset_units = units;
}

#endif