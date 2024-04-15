#include "renderer.h"
#include <engine/scene/scene.h>
#include <engine/object/objects.h>

cRenderer::cRenderer()
{

}

cRenderer::~cRenderer()
{

}

void cRenderer::CollectViewObjects(cScene* scene)
{
	objects_in_view = 0;

	scene->ObjectList.reset_enumerator();
	while(cObjectBase* object_base = scene->ObjectList.next())
	{
		object_base->is_in_frustum = false;
		if(object_base->UpdateFrustumViewBox(scene->pMainCamera))
		{
			object_base->is_in_frustum = true;

			TmpStackSortingItems.Push(object_base);

			++objects_in_view;
		}
	}
}

float Distance(const vec3& vPoint1, const vec3& vPoint2)
{
	return sqrt((vPoint2.x - vPoint1.x) * (vPoint2.x - vPoint1.x) + (vPoint2.y - vPoint1.y) * (vPoint2.y - vPoint1.y) + (vPoint2.z - vPoint1.z) * (vPoint2.z - vPoint1.z));
}

cObjectBase* cRenderer::PopItemByDistance(cCamera* pCamera)
{
	if(!TmpStackSortingItems.Root)return 0;

	cStack<cObjectBase*>::_item* cur = TmpStackSortingItems.Root;
	float max_dist = 0.0f;
	cStack<cObjectBase*>::_item* max_item = 0;

	//printf("%p %p\n", TmpStackSortingItems.Root, TmpStackSortingItems.Last);

	while(cur)
	{
		cObjectBase* object_base = cur->v;

		float dist = distance(vec3(object_base->WorldMx[3]), pCamera->eye);

		/*float dist = Distance(
			vec3(
				object_base->WorldMx[3][0],
				object_base->WorldMx[3][1],
				object_base->WorldMx[3][2]
			),
			vec3(pCamera->eye.x, pCamera->eye.y, pCamera->eye.z)
		);*/

		//printf("%s %f\n", object_base->name, dist);

		if(dist > max_dist){
			max_dist = dist;
			max_item = cur;
		}

		cur = cur->next;
	}

	if(!max_item)return 0;

	cObjectBase* ret_object = max_item->v;

	TmpStackSortingItems.Free(max_item);

	//printf("return %s\n", ret_object->name);

	return ret_object;
}

int cRenderer::RenderShadowsDepth(cScene* scene)
{
	//printf("-------------------------------------    cRenderer::RenderShadowsDepth    ------------------------------------\n");

	scene->SceneLights.UpdateLightSpec();

	glFrontFace(GL_CW);

	for(int li = 0; li < scene->SceneLights.count; ++li)
	{
		scene->SceneLights.DepthBuffersBindFBO(li);

		scene->SceneLights.DepthBuffersSetView();

		/*scene->ObjectList.reset_enumerator();
		while(cObjectBase* object_base = scene->ObjectList.next())
		{
			object_base->is_in_frustum = false;

			if(object_base->UpdateFrustumViewBox(scene->pMainCamera))
			{
				object_base->is_in_frustum = true;

				if(object_base->is_rendered && object_base->is_cast_shadow)
				{
					cObjectModel* pObjectModel = (cObjectModel*)object_base;
					//printf(">>> Render->GathereShadowDepth:%s %d\n", pObjectModel->name, li);

					pObjectModel->GathereShadowDepth(&scene->SceneLights.LightSpaceMatrces[li]);
				}
			}
		}*/

		cStack<cObjectBase*>::_item* cur = TmpStackSortingItems.Root;
		while(cur)
		{
			cObjectBase* object_base = cur->v;

			if(object_base->is_rendered && object_base->is_cast_shadow)
			{
				cObjectModel* pObjectModel = (cObjectModel*)object_base;

				pObjectModel->GathereShadowDepth(&scene->SceneLights.LightSpaceMatrces[li]);
			}

			cur = cur->next;
		}

		scene->SceneLights.DepthBuffersUnbindFBO();
	}

	glFrontFace(GL_CCW);

	return 0;
}

int cRenderer::Render(cScene* scene)
{
	//printf("-------------------------------------    cRenderer::Render    ------------------------------------\n");

	scene->LandscapeList.reset_enumerator();
	while(cObjectBase* object_base = scene->LandscapeList.next())
	{
		if(object_base->is_rendered)object_base->Draw(scene->pMainCamera);
	}

	/*for(int zri = scene->CurrentViewZonRow - 4; zri <= scene->CurrentViewZonRow + 4; ++zri)
	{
		for(int zci = scene->CurrentViewZonCell - 4; zci <= scene->CurrentViewZonCell + 4; ++zci)
		{
			int tzri = zri;
			int tzci = zci;

			int pos_x = 0;
			int pos_z = 0;

			if(tzri < 0){
				while(tzri < 0)
				{
					tzri += scene->SceneMap->Navigation.MapRows;
				}

				pos_z = zri * (scene->SceneMap->ZoneRows - 1);
			}
			else if(tzri >= scene->SceneMap->Navigation.MapRows)
			{
				tzri = scene->SceneMap->Navigation.MapRows - tzri;
			}
			else{
				pos_z = zri * (scene->SceneMap->ZoneRows - 1);
			}

			if(tzci < 0){
				while(tzci < 0)
				{
					tzci += scene->SceneMap->Navigation.MapCells;
				}

				pos_x = zci * (scene->SceneMap->ZoneCells - 1);
			}
			else if(tzci >= scene->SceneMap->Navigation.MapCells)
			{
				tzci = scene->SceneMap->Navigation.MapCells - tzci;
			}
			else{
				pos_x = zci * (scene->SceneMap->ZoneCells - 1);
			}

			scene->SceneMap->ViewZones[tzri][tzci]->LandscapeObject->SetPosition(pos_x, 0.0f, pos_z);
			scene->SceneMap->ViewZones[tzri][tzci]->LandscapeObject->Draw(scene->pMainCamera);
		}
	}*/

	cObjectBase* object_base = 0;
	while(object_base = PopItemByDistance(scene->pMainCamera))
	{
		object_base->Draw(scene->pMainCamera);
	}

	TmpStackSortingItems.Swap();

	return 0;
}