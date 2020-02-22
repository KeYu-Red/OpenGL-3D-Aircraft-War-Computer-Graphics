#ifndef _OBJWRITER_H
#define _OBJWRITER_H
#include"GeometricSolid.h"

void ExportCube()
{
	FILE *file = fopen("export/cube.obj", "w");
	for (int i = 0; i < cube_vnum; i++)
	{
		fprintf(file, "v %lf %lf %lf \n", cube_v[i].x, cube_v[i].y, cube_v[i].z);
	}
	for (int i = 0; i < cube_vtnum; i++)
	{
		fprintf(file, "vt %lf %lf \n", cube_vt[i].u, cube_vt[i].v);
	}
	for (int i = 0; i < cube_vnnum; i++)
	{
		fprintf(file, "vn %lf %lf %lf \n", cube_vn[i].x, cube_vn[i].y, cube_vn[i].z);
	}
	for (int i = 0; i < cube_fnum * 3; i+=3)
	{
		fprintf(file, "f %d/%d/%d %d/%d/%d %d/%d/%d \n", 
			cube_index[i].pointindex, cube_index[i].uvindex, cube_index[i].normalindex,
			cube_index[i+1].pointindex, cube_index[i+1].uvindex, cube_index[i+1].normalindex,
			cube_index[i+2].pointindex, cube_index[i+2].uvindex, cube_index[i+2].normalindex);
	}
	fclose(file);

}void ExportSphere()
{
	FILE *file = fopen("export/sphere.obj", "w");
	for (int i = 0; i < sphere_vnum; i++)
	{
		fprintf(file, "v %lf %lf %lf \n", sphere_v[i].x, sphere_v[i].y, sphere_v[i].z);
	}
	for (int i = 0; i < sphere_vtnum; i++)
	{
		fprintf(file, "vt %lf %lf \n", sphere_vt[i].u, sphere_vt[i].v);
	}
	for (int i = 0; i < sphere_vnnum; i++)
	{
		fprintf(file, "vn %lf %lf %lf \n", sphere_vn[i].x, sphere_vn[i].y, sphere_vn[i].z);
	}
	for (int i = 0; i < sphere_fnum * 3; i += 3)
	{
		fprintf(file, "f %d/%d/%d %d/%d/%d %d/%d/%d \n",
			sphere_index[i].pointindex, sphere_index[i].uvindex, sphere_index[i].normalindex,
			sphere_index[i + 1].pointindex, sphere_index[i + 1].uvindex, sphere_index[i + 1].normalindex,
			sphere_index[i + 2].pointindex, sphere_index[i + 2].uvindex, sphere_index[i + 2].normalindex);
	}
	fclose(file);
}

void ExportCone()
{
	FILE *file = fopen("export/cone.obj", "w");
	for (int i = 0; i < cone_vnum; i++)
	{
		fprintf(file, "v %lf %lf %lf \n", cone_v[i].x, cone_v[i].y, cone_v[i].z);
	}
	for (int i = 0; i < cone_vtnum; i++)
	{
		fprintf(file, "vt %lf %lf \n", cone_vt[i].u, cone_vt[i].v);
	}
	for (int i = 0; i < cone_vnnum; i++)
	{
		fprintf(file, "vn %lf %lf %lf \n", cone_vn[i].x, cone_vn[i].y, cone_vn[i].z);
	}
	for (int i = 0; i < cone_fnum * 3; i+=3)
	{
		fprintf(file, "f %d/%d/%d %d/%d/%d %d/%d/%d \n", 
			cone_index[i].pointindex, cone_index[i].uvindex, cone_index[i].normalindex,
			cone_index[i+1].pointindex, cone_index[i+1].uvindex, cone_index[i+1].normalindex,
			cone_index[i+2].pointindex, cone_index[i+2].uvindex, cone_index[i+2].normalindex);
	}
	fclose(file);
}

void ExportCylinder()
{
	FILE *file = fopen("export/cylinder.obj", "w");
	for (int i = 0; i < cylinder_vnum; i++)
	{
		fprintf(file, "v %lf %lf %lf \n", cylinder_v[i].x, cylinder_v[i].y, cylinder_v[i].z);
	}
	for (int i = 0; i < cylinder_vtnum; i++)
	{
		fprintf(file, "vt %lf %lf \n", cylinder_vt[i].u, cylinder_vt[i].v);
	}
	for (int i = 0; i < cylinder_vnnum; i++)
	{
		fprintf(file, "vn %lf %lf %lf \n", cylinder_vn[i].x, cylinder_vn[i].y, cylinder_vn[i].z);
	}
	for (int i = 0; i < cylinder_fnum * 3; i+=3)
	{
		fprintf(file, "f %d/%d/%d %d/%d/%d %d/%d/%d \n", 
			cylinder_index[i].pointindex, cylinder_index[i].uvindex, cylinder_index[i].normalindex,
			cylinder_index[i+1].pointindex, cylinder_index[i+1].uvindex, cylinder_index[i+1].normalindex,
			cylinder_index[i+2].pointindex, cylinder_index[i+2].uvindex, cylinder_index[i+2].normalindex);
	}
	fclose(file);
}


#endif // !_OBJWRITER_H
