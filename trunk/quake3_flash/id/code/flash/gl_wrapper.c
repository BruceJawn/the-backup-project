
#include <math.h>
#include <string.h>

#include <GL/gl.h>

#include "molehill.h"

float _clear_depth = 0.0f;
float _clear_red = 0.0f;
float _clear_green = 0.0f;
float _clear_blue = 0.0f;
float _clear_alpha = 0.0f;

int _culling_mode = MOLEHILL_BACK;
int _culling_enabled = 1;

int _blending_src_op = MOLEHILL_ONE;
int _blending_dst_op = MOLEHILL_ZERO;

int _depth_func = MOLEHILL_LESS_OR_EQUAL;

GLfloat* _vertex_array_pointer = 0;
GLsizei _vertex_array_stride = 0;
GLenum _vertex_array_type = GL_FLOAT;
GLint _vertex_array_size = 4;

GLuint* _color_array_pointer = 0;
GLsizei _color_array_stride = 0;
GLenum _color_array_type = GL_FLOAT;
GLint _color_array_size = 4;

GLfloat* _texcoord_array_pointer = 0;
GLsizei _texcoord_array_stride = 0;
GLenum _texcoord_array_type = GL_FLOAT;
GLint _texcoord_array_size = 4;

int _index_array_pointer[4096];
int _index_array_length = 0;

GLenum _polygon_mode = GL_TRIANGLES;

float _polygon_offset_factor;
float _polygon_offset_units;
