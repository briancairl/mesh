/// @file 	mesh.h
///	@brief 	A small C/CXX binary STL-IO library
#ifndef MESH_H
#define MESH_H 	MESH_VER

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#ifndef MESH_WITH_OPENGL
#define MESH_WITH_OPENGL	1
#endif

#ifndef MESH_WITH_WARNINGS
#define MESH_WITH_WARNINGS	1
#endif

#ifndef MESH_WITH_MESSAGES
#define MESH_WITH_MESSAGES	0
#endif

#if(MESH_WITH_OPENGL)
#include "GL/freeglut.h"
#endif

#if __cplusplus
extern "C"
{
#endif
	
	
	typedef uint16_t attr_t;
	
	
	typedef struct PT_T
	{
		float x;
		float y;
		float z;
	} pt_t;


	typedef void(*animate_fn)(pt_t*,float*,uint32_t*);


	typedef struct TRI_T
	{
		pt_t		normal;
		pt_t		v[3U];
		pt_t		color;
		attr_t		attribute;
	} tri_t;


	typedef struct HEADER_T
	{
		uint8_t		data[80UL];
		uint32_t	tri_count;
	} header_t;

	typedef struct STL_T
	{
		header_t 	header;
		tri_t* 		tri_data;
	} stl_t;


	uint8_t mesh_read_bin( const char* filename, stl_t* mesh );
	void	mesh_init( stl_t* mesh );
	stl_t* 	mesh_create();
	void	mesh_destroy( stl_t* mesh );
	void	mesh_autocenter( stl_t* mesh );
	void	mesh_normalize( stl_t* mesh, float by );
	void	mesh_rotate( stl_t* mesh, float x, float y, float z );
	void	mesh_animate( stl_t* mesh, animate_fn anifn, float* params );

#if(MESH_WITH_OPENGL)
	void	mesh_draw( 
		stl_t*		mesh, 
		const float	px,
		const float	py,
		const float	pz,
		const float	rx,
		const float	ry,
		const float	rz,
		const float r,
		const float g,
		const float b,
		const float a
	);
#endif

#if __cplusplus
}
#endif

#endif