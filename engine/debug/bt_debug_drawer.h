#ifndef _BT_DEBUG_DRAWER_H
#define _BT_DEBUG_DRAWER_H

#include <LinearMath/btIDebugDraw.h>
#include <engine/camera.h>

class BTDebugDrawer : public btIDebugDraw
{
	int debug_mode = 0;
	cCamera* pCamera = 0;
public:
	BTDebugDrawer(){}
	~BTDebugDrawer(){}

	void SetCameraRef(cCamera* camera);
	void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);
	void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color);
	void reportErrorWarning(const char* warningString);
	void draw3dText(const btVector3& location, const char* textString);
	void setDebugMode(int debugMode);
	virtual int getDebugMode() const { return debug_mode; }

};

#endif