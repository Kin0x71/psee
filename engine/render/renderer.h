#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <engine/scene/scene.h>
#include <engine/common/stack.h>

class cRenderer {
public:

	cStack<cObjectBase*> TmpStackSortingItems;
	int objects_in_view = 0;

	cRenderer();
	~cRenderer();

	void CollectViewObjects(cScene* scene);
	cObjectBase* PopItemByDistance(cCamera* pCamera);
	int RenderShadowsDepth(cScene* scene);
	int Render(cScene* scene);
};

#endif