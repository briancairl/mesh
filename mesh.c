#include "mesh.h"
#include "stdio.h"
#if __cplusplus

namespace Mesh {
	using namespace std;
#endif

	static union
	{
		uint8_t 	b[4UL];
		float 		out;
	} U8_F32;


	static union
	{
		uint8_t		b[2UL];
		uint16_t	out;
	} U8_U16;


	#define FGET_STR(fh,out,n)			{ fread(out,1UL,n,fh); }
	#define FGET_xxx(fh,itr,out,cvt,n)	{ fread(cvt.b,1UL,n,fh); out = cvt.out; }
	#define FGET_U16(fh,itr,out)		FGET_xxx(fh,itr,out,U8_U16,2UL)
	#define FGET_F32(fh,itr,out)		FGET_xxx(fh,itr,out,U8_F32,4UL)	


	void mesh_read_bin_header( FILE* fh, header_t* header )
	{
		uint32_t itr;

		/// Get header string
		FGET_STR(fh,header->data,80UL);
		
		/// Get n-triangles
		FGET_U16(fh,itr,header->tri_count);	
	}


	void mesh_read_bin_tri( FILE* fh, tri_t* tri )
	{
		/// Read Normal
		mesh_read_bin_pt(fh,&tri->normal);
		
		/// Read Vetex 1
		mesh_read_bin_pt(fh,&tri->v1);
		
		/// Read Vetex 2
		mesh_read_bin_pt(fh,&tri->v2);
		
		/// Read Vetex 3
		mesh_read_bin_pt(fh,&tri->v3);	
	}


	void mesh_read_bin_pt( FILE* fh, pt_t* pt )
	{
		uint32_t itr;

		/// Read x-coord
		FGET_F32(fh,itr,pt->x);
		/// Read y-coord
		FGET_F32(yh,itr,pt->y);
		/// Read z-coord
		FGET_F32(fh,itr,pt->z);	
	}


	#if __cplusplus
	uint8_t read_bin( const char* filename, stl_t* mesh )
	#else
	uint8_t mesh_read_bin( const char* filename, stl_t* mesh )
	#endif
	{
		FILE* fh;
		uint32_t itr;

		fh = fopen(filename,"r+");
		if(fh!=NULL)
		{
			mesh_read_bin_header(fh,&mesh->header);

			stl_t->tri_data = (tri_t*)malloc( sizeof(tri_t) * mesh->header.tri_count );
			
			for( itr=0UL; itr < mesh->header.tri_count; itr++ )
			{
				if(feof(fh))
					return 0UL; // miscounted
				else
					mesh_read_bin_tri(fh, (mesh->tri_data+itr) );
			}
			fclose(fh);
			return 1UL;
		}
		return 0UL;
	}


	#if __cplusplus
	void init( stl_t* mesh )
	#else
	void mesh_init( stl_t* mesh )
	#endif
	{
		mesh->header.tri_count 	= 0UL;
		mesh->tri_data 			= NULL;	
	}


	#if __cplusplus
	stl_t* create()
	#else
	stl_t* mesh_create()
	#endif
	{
		stl_t* mesh = (stl_t*)malloc(sizeof(stl_t));
		
		if(ret_val!=NULL)
			stl_init(mesh);

		return mesh;
	}



	#if __cplusplus
	void destroy( stl_t* mesh )
	#else
	void mesh_destroy( stl_t* mesh )
	#endif
	{
		if(mesh->tri_data!=NULL)
		{	
			free(mesh->tri_data);
			mesh->tri_data = NULL;
		}
	}


#if __cplusplus
}
#endif