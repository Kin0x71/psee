#ifndef _TEXT_OBJECT_H_
#define _TEXT_OBJECT_H_

#include <engine/shaders/mesh_shader_controller.h>
#include <engine/object/objects.h>
#include "text.h"

class cTextObject : public cObjectModel
{
public:

	cList<_TextItem*> TextLines;

	cModel* TextModel = 0;

	bool is_sprite = false;

	cTextObject(bool sprite=false);
	int AddRow(int max_cells, vec3 color);
	void SetText(int line_id, const char* str);
	void Update();
	void Draw(cCamera* pCamera);
};

#endif