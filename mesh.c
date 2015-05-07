#include "mesh.h"

#define PT_ZERO(pt)					{pt.x=pt.y=pt.z=0.f;}
#define PT_ADDC(pt1,pt2)			{ {pt1.x+=pt2.x;}	{pt1.y+=pt2.y;}		{pt1.z+=pt2.z;} }
#define PT_SUBC(pt1,pt2)			{ {pt1.x-=pt2.x;}	{pt1.y-=pt2.y;}		{pt1.z-=pt2.z;} }
#define PT_SCALE(pt,n)				{ {pt.x/=n;}		{pt.y/=n;}			{pt.z/=n;}		}
#define PT_NORM(pt)					sqrtf( powf(pt.x,2.f)+powf(pt.y,2.f)+powf(pt.z,2.f) )
#define FGET_STR(fh,out,n)			{ fread((void*)out,  1UL,n,fh); }
#define MESH_HAS_COLOR(attr)		(attr&(0x1<<15UL))

void fget_u16( FILE** fh, uint16_t* out )
{
	union { uint8_t b[2UL]; uint16_t out; } U08_U16;

	fread((void*)U08_U16.b,1U,2UL,*fh);

	*out	= U08_U16.out;
}

void fget_u32( FILE** fh, uint32_t* out )
{
	union { uint8_t b[4UL]; uint32_t out; } U08_U32;

	fread((void*)U08_U32.b,1U,4UL,*fh);

	*out	= U08_U32.out;
}

void fget_f32( FILE** fh, float* out )
{
	union { uint8_t b[4UL]; float out; } U08_F32;

	fread((void*)U08_F32.b,1U,4UL,*fh);

	*out	= U08_F32.out;
}


void mesh_read_bin_header( FILE** fh, header_t* header )
{
	/// Get header string
	FGET_STR((*fh),header->data,80UL);
		
	/// Get n-triangles
	fget_u32(fh,&header->tri_count);	
}



void mesh_read_bin_pt( FILE** fh, pt_t* pt )
{
	/// Read x-coord
	fget_f32(fh,&pt->x);
	/// Read y-coord
	fget_f32(fh,&pt->y);
	/// Read z-coord
	fget_f32(fh,&pt->z);	
}


void mesh_read_bin_tri( FILE** fh, tri_t* tri )
{
	uint32_t vtr;

	/// Read Normal
	mesh_read_bin_pt(fh,&tri->normal);
		
	/// Read Vetex 1,2,3
	for( vtr = 0U; vtr < 3U; vtr++ )
		mesh_read_bin_pt(fh,tri->v+vtr);

	/// Get Attribute Bits
	fget_u16(fh,&tri->attribute);	

	/// Get Color, if available
	if(MESH_HAS_COLOR(tri->attribute))
	{
		tri->color.x = ((tri->attribute&0xF000)<< 0UL)/31.f;
		tri->color.y = ((tri->attribute&0x0F00)<< 8UL)/31.f;
		tri->color.z = ((tri->attribute&0x00F0)<<16UL)/31.f;
	}
}



uint8_t mesh_read_bin( const char* filename, stl_t* mesh )
{
	FILE* fh;
	uint32_t itr;

	fh = fopen(filename,"rb");
	if(fh!=NULL)
	{
		mesh_read_bin_header(&fh,&mesh->header);
	
		mesh->tri_data = (tri_t*)malloc( sizeof(tri_t) * mesh->header.tri_count );
			
		for( itr=0UL; itr < mesh->header.tri_count; itr++ )
		{
			if(ferror(fh)||feof(fh))
			{
				#if _DEBUG && MESH_WITH_WARNINGS
				printf("%s : %s \n","MESH","Bad file. Ended before all triangles collected.",itr);
				#endif
				return 0U;
			}
			else
			{
				mesh_read_bin_tri(&fh, &mesh->tri_data[itr] );
			}
		}
		fclose(fh);
		return 1UL;
	}
	return 0UL;
}



void mesh_init( stl_t* mesh )
{
	mesh->header.tri_count 	= 0UL;
	mesh->tri_data 			= NULL;	
}



stl_t* mesh_create()
{
	stl_t* mesh = (stl_t*)malloc(sizeof(stl_t));
		
	if(mesh!=NULL)
		mesh_init(mesh);

	return mesh;
}



void mesh_destroy( stl_t* mesh )
{
	if(mesh->tri_data!=NULL)
	{	
		free(mesh->tri_data);
		mesh->tri_data = NULL;
	}
}


void mesh_autocenter( stl_t* mesh )
{
	pt_t center;
	uint32_t itr, vtr;

	PT_ZERO(center);
	for( itr=0UL; itr < mesh->header.tri_count; itr++ )
		for( vtr=0UL; vtr < 3U; vtr++ )
			PT_ADDC(center,mesh->tri_data[itr].v[vtr]);

	PT_SCALE(center,(mesh->header.tri_count*3.f));
	for( itr=0UL; itr < mesh->header.tri_count; itr++ )
		for( vtr=0UL; vtr < 3U; vtr++ )
			PT_SUBC(mesh->tri_data[itr].v[vtr],center);
}


void mesh_normalize( stl_t* mesh, float by )
{
	uint32_t itr,vtr;

	mesh_autocenter( mesh );
	for( itr=0UL; itr < mesh->header.tri_count; itr++ )
		for( vtr=0UL; vtr < 3U; vtr++ )
			PT_SCALE(mesh->tri_data[itr].v[vtr],by);
}



void mesh_rotate( stl_t* mesh, float x, float y, float z )
{
	uint32_t itr,vtr;

	for( itr=0UL; itr < mesh->header.tri_count; itr++ )
	{
		for( vtr=0UL; vtr < 3U; vtr++ )
		{
			float Cx = cosf(x);
			float Sx = sinf(x);
			float Cy = cosf(y);
			float Sy = sinf(y);
			float Cz = cosf(z);
			float Sz = sinf(z);
			float vv[3U];

			vv[0U] = mesh->tri_data[itr].v[vtr].z*(Sx*Sz + Cx*Cz*Sy) - mesh->tri_data[itr].v[vtr].y*(Cx*Sz - Cz*Sx*Sy) + mesh->tri_data[itr].v[vtr].x*Cy*Cz;
			vv[1U] = mesh->tri_data[itr].v[vtr].y*(Cx*Cz + Sx*Sy*Sz) - mesh->tri_data[itr].v[vtr].z*(Cz*Sx - Cx*Sy*Sz) + mesh->tri_data[itr].v[vtr].x*Cy*Sz;
			vv[2U] = mesh->tri_data[itr].v[vtr].z*Cx*Cy - mesh->tri_data[itr].v[vtr].x*Sy + mesh->tri_data[itr].v[vtr].y*Cy*Sx;

			mesh->tri_data[itr].v[vtr].x = vv[0U];
			mesh->tri_data[itr].v[vtr].y = vv[1U];
			mesh->tri_data[itr].v[vtr].z = vv[2U];
		}
	}
}




#if(MESH_WITH_OPENGL)
void	mesh_draw( 
	stl_t*			mesh, 
	const float 	px,
	const float 	py,
	const float 	pz,
	const float 	rx,
	const float 	ry,
	const float 	rz,
	const float 	r,
	const float 	g,
	const float 	b,
	const float 	a
){
	uint32_t itr,vtr;

#if _DEBUG && MESH_WITH_WARNINGS
	if(mesh==NULL)
		printf("%s : %s\n","MESH","bad mesh object.");
#endif

	glPushMatrix();
	glTranslatef(px,py,pz);
	glRotatef(rx,1.f,0.f,0.f);
	glRotatef(ry,0.f,1.f,0.f);
	glRotatef(rz,0.f,0.f,1.f);
	for( itr = 0UL; itr < mesh->header.tri_count; itr++ )
	{
		glBegin(GL_TRIANGLES);

        if(MESH_HAS_COLOR(mesh->tri_data[itr].attribute))
			glColor4f(mesh->tri_data[itr].color.z,mesh->tri_data[itr].color.y,mesh->tri_data[itr].color.x,0.5f);
		else
			glColor4f(r,g,b,a);

		glNormal3f(
			mesh->tri_data[itr].normal.x,
			mesh->tri_data[itr].normal.y,
			mesh->tri_data[itr].normal.z
		);
		for( vtr = 0U; vtr < 3U; vtr++ )
		{
			glVertex3f(
				mesh->tri_data[itr].v[vtr].x,
				mesh->tri_data[itr].v[vtr].y,
				mesh->tri_data[itr].v[vtr].z
			);
		}
		glEnd();
	}

	glPopMatrix();
}
#endif