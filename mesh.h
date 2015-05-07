/// @file 	mesh.h
///	@brief 	A small C/CXX binary STL-IO library
#ifndef MESH_H
#define MESH_H 	MESH_VER

#include <stdlib.h>
#include <stdint.h>

#if __cplusplus
namespace Mesh
{
#endif

	typedef struct PT_T
	{
		float x;
		float y;
		float z;
	} pt_t;

	typedef struct TRI_T
	{
		pt_t normal;
		pt_t v1;
		pt_t v2;
		pt_t v3;
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


	#if __cplusplus
	uint8_t read_bin( FILE* fh, stl_t* mesh );
	uint8_t init( stl_t* mesh );
	stl_t* 	create();
	void	destroy( stl_t* mesh );
	#else
	uint8_t mesh_read_bin( FILE* fh, stl_t* mesh );
	uint8_t mesh_init( stl_t* mesh );
	stl_t* 	mesh_create();
	void	mesh_destroy( stl_t* mesh );
	#endif


#if __cplusplus
}
#endif

#endif