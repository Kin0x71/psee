#ifndef _DEBUG_OBJECT_H_
#define _DEBUG_OBJECT_H_

#include "objects.h"
#include <engine/camera.h>

class cObjectDebug:public cObjectBase {
public:

#define LINE_WIDTH 5.0f

	enum class _OBJECT_TYPE{
		NONE,
		CROSS,
		SQUARE,
		CIRCLE
	};

	_OBJECT_TYPE object_type = _OBJECT_TYPE::NONE;
	float size = 0.0f;
	vec3 velocity;

	cObjectDebug(_OBJECT_TYPE ObjectType, float Size);
	~cObjectDebug();

	void Update();
	void Draw(cCamera* pCamera, mat4* pworldmx);

	void DrawCross(cCamera* pCamera);
	void DrawSquare(cCamera* pCamera);
	void DrawCircle(cCamera* pCamera);
};

#endif