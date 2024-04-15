#include "bt_debug_drawer.h"
#include <engine/common/gl_head.h>

#include <stdio.h>

void BTDebugDrawer::SetCameraRef(cCamera* camera)
{
	pCamera = camera;
}

void BTDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf((float*)&pCamera->ProjMx);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf((float*)&pCamera->ViewMx);

	glDisable(GL_TEXTURE_2D);

	glLineWidth(1);

	glBegin(GL_LINES);

	glColor3d(color.x(), color.y(), color.z());
	glVertex3f(from.x(), from.y(), from.z());
	glColor3d(color.x(), color.y(), color.z());
	glVertex3f(to.x(), to.y(), to.z());

	glEnd();
	glEnable(GL_TEXTURE_2D);
}

void BTDebugDrawer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
}

void BTDebugDrawer::reportErrorWarning(const char* warningString)
{
}

void BTDebugDrawer::draw3dText(const btVector3& location, const char* textString)
{
}

void BTDebugDrawer::setDebugMode(int debugMode)
{
	debug_mode = debugMode;
}
