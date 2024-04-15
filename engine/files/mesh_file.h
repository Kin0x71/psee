
#ifndef _MESH_FILE_H_
#define _MESH_FILE_H_

#include <engine/common/list.h>
#include <engine/mesh/model.h>
#include <engine/object/animation/skeleton_animation.h>
#include <engine/object/animation/animation.h>
//#include <object/render/render_object3d.h>
//#include "../object/animation/animation.h"

struct _model_file_container{
	struct _collide_mesh{
		int vertices_count = 0;
		int indexes_count = 0;
		vec3* vertices=0;
		word* indexes = 0;
	};

	cModel* model = 0;
	_collide_mesh* collide_mesh = 0;
};

bool LoadMeshlVss(const char* fname, cList<cMesh*>* pret_mesh_list);
_model_file_container* LoadModelVss(const char* fname, int Wrapp = GL_REPEAT, int Filter = GL_LINEAR);
cSkeleton* LoadSkeletonKbh(const char* fname);
cAnimation *LoadAnimationAkf(const char* fname);
cAnimation* LoadAnimationAbc(const char* fname);

#endif