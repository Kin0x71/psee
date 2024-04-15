
#include "astar.h"

cAsSortList::_asitem::_asitem(_asnode_base* innode, _asnode_base* inparent, uint insteps, float inrating)
{
	node = innode;
	steps = insteps;
	rating = inrating;
	parent = inparent;
	back = 0;
	next = 0;
}

cAsSortList::_asitem::~_asitem()
{
	if(next)delete next;
}

void cAsSortList::_asitem::operator = (_asitem* i){
	node = i->node;
	steps = i->steps;
	parent = i->parent;
}

cAsSortList::cAsSortList()
{
	Root = _create_list(_COLLISION_SORT_INIT_SIZE);
}

cAsSortList::~cAsSortList()
{
	delete Root;
}

cAsSortList::_asitem* cAsSortList::_create_list(uint count)
{
	_asitem* root = new _asitem();
	_asitem* cur = root;

	for(int i = 0; i < count; ++i)
	{
		cur->next = new _asitem();
		cur->next->back = cur;
		cur = cur->next;
	}
	Last = cur;

	return root;
}

void cAsSortList::_reset_list()
{
	_asitem* cur = Root;

	for(int i = 0; cur->next; ++i)
	{
		cur->node = 0;
		cur->steps = -1;
		cur->parent = 0;
		cur = cur->next;
	}
}

cAsSortList::_asitem* cAsSortList::GetItem(_asnode_base* node)
{
	_asitem* cur = Root;
	while(cur->steps != -1)
	{
		if(cur->node == node)return cur;
		cur = cur->next;
	};

	return 0;
}

void cAsSortList::PushItem(_asitem* initem)
{
	_asitem* item = Last;
	Last = Last->back;
	Last->steps = -1;
	Last->next = 0;
	item->back = 0;
	item->next = Root;
	(*item) = initem;
	Root->back = item;
	Root = item;
}

void cAsSortList::PushItem(_asnode_base* node, _asnode_base* parent_node, uint steps)
{
	_asitem* item = Last;
	Last = Last->back;
	Last->steps = -1;
	Last->next = 0;
	item->back = 0;
	item->next = Root;

	item->node = node;
	item->parent = parent_node;
	item->steps = steps;

	Root->back = item;
	Root = item;
}

bool cAsSortList::PopMinItem(_asitem* pitem)
{
	_asitem* cur = Root;
	_asitem* ret = 0;
	uint minsteps = 999;

	while(cur->steps != -1)
	{
		if(cur->steps < minsteps)
		{
			minsteps = cur->steps;
			ret = cur;
		}
		cur = cur->next;
	};

	if(ret){
		(*pitem) = ret;

		DeleteItem(ret);

		return true;
	}

	return false;
}

void cAsSortList::DeleteItem(_asitem* item)
{
	if(item->back && item->next)
	{
		item->back->next = item->next;
		item->next->back = item->back;
		item->back = Last;
		item->next = 0;
		Last = item;
		Last->steps = -1;
	}
	else if(item->back == 0){
		//printf("		DEL:%x<-%d->%x\n",item->back,item->pid,item->next);
		item->back = Last;
		Last = item;
		Last->steps = -1;
		Root = Root->next;
		Root->back = 0;
		Last->next = 0;
	}
	else if(item->next == 0){
		item->steps = -1;
	}
}

_PATH_TRACK::_PATH_TRACK()
{
	num = 0;
	points = 0;
}

_PATH_TRACK::_PATH_TRACK(uint count)
{
	//printf("new %x\n",this);
	num = count;
	points = new _ASPOINT[num + 1];
}

_PATH_TRACK::~_PATH_TRACK()
{
	//printf("delete %x\n",this);
	if(points)delete[]points;
}

cAStar::cAStar()
{
	Open = new cAsSortList();
	Close = new cAsSortList();
}

cAStar::~cAStar()
{
	delete Open;
	delete Close;
}

_PATH_TRACK* cAStar::build_path()
{
	cAsSortList::_asitem* cur = Close->Root;

	_PATH_TRACK* pPath = new _PATH_TRACK(cur->steps);
	pPath->points[0].x = 0;
	pPath->points[0].y = 0;

	_ASPOINT vneed(cur->node->c, cur->node->r, cur->node->id);

	if(pPath->num > 0)
	{
		uint idec = pPath->num;

		while(cur && cur->node)
		{
			if(cur->node->r == vneed.y && cur->node->c == vneed.x)
			{
				pPath->points[idec].as_id = cur->node->id;
				pPath->points[idec].x = cur->node->c;
				pPath->points[idec].y = cur->node->r;
				//printf("%dx%d ",cur->node->c,cur->node->r);
				--idec;

				if(idec == -1)return pPath;

				vneed.x = cur->parent->c;
				vneed.y = cur->parent->r;
			}
			cur = cur->next;
		}
	}
	//printf("\n");
	return pPath;
}

_PATH_TRACK* cAStar::FindPath(uint startX, uint startY, uint targetX, uint targetY, _asnode_base*** rootnode)
{
	if(startX == targetX && startY == targetY)return 0;
	//printf("######################\n");
	//printf("startX:%d startY:%d -> targetX:%d targetY:%d\n", startX, startY, targetX, targetY);
	//printf("START  %dx%d (%dx%d)\n", startX, startY, rootnode[startY][startX]->c, rootnode[startY][startX]->r);
	//printf("TARGET %dx%d (%dx%d)\n", targetX, targetY, rootnode[targetY][targetX]->c, rootnode[targetY][targetX]->r);

	Open->_reset_list();
	Close->_reset_list();
	cAsSortList::_asitem startitem(rootnode[startY][startX], 0, 0, 0);

	int flag_root_node = rootnode[startY][startX]->flag;
	rootnode[startY][startX]->flag = 0;

	Open->PushItem(&startitem);

	_asnode_base* node = (_asnode_base*)-1;

	int steps = 0;

	do
	{
		cAsSortList::_asitem parent;

		if(!Open->PopMinItem(&parent)){
			//printf("EOF\n");
			rootnode[startY][startX]->flag = flag_root_node;
			return 0;
		}
		node = parent.node;

		if(node->flag != -1)
		{
			//node->check=parent.steps;
			//printf("%d,%d %d\n",node->y,node->x,node->flag);
			if((node->r == targetY && node->c == targetX))// || ((node->x==targetX-1 || node->x==targetX+1) && (node->y==targetY-1 || node->y==targetY+1)))
			{
				Close->PushItem(parent.node, parent.parent, parent.steps);
				rootnode[startY][startX]->flag = flag_root_node;

				//printf("FOUND %dx%d (%dx%d)\n", node->c, node->r, rootnode[node->r][node->c]->c, rootnode[node->r][node->c]->r);
				return build_path();
			}

			//_LINK dirpart[4] = { _LINK::NS, _LINK::EW, _LINK::SN, _LINK::WE };

			for(int lid = 0; lid < node->links_count; ++lid)
			{
				_asnode_base* child = node->link_nodes[lid];

				if(child->flag != -1)
				{
					cAsSortList::_asitem* openitem = Open->GetItem(child);
					cAsSortList::_asitem* closeitem = Close->GetItem(child);

					if(!closeitem)
					{
						if(!openitem){
							Open->PushItem(child, parent.node, parent.steps + 1);
						}
						else{
							if(openitem->steps > parent.steps){
								openitem->steps = parent.steps + 1;
								openitem->parent = parent.node;
							}
						}
					}
					else{
						if(closeitem->steps > parent.steps){
							Open->PushItem(closeitem->node, parent.node, parent.steps + 1);
							Close->DeleteItem(closeitem);
						}
					}
				}
			}//if not out size
		}
		else{
			printf("FLAG==-1\n");
		}

		Close->PushItem(parent.node, parent.parent, parent.steps);

		++steps;
	} while(node != (_asnode_base*)-1);

	rootnode[startY][startX]->flag = flag_root_node;
	return 0;
}

_PATH_TRACK* cAStar::FindPath(_asnode_base* start_node, _asnode_base* target_node)
{
	if(start_node == target_node)return 0;
	//printf("######################\n");
	//printf("startX:%d startY:%d -> targetX:%d targetY:%d\n", startX, startY, targetX, targetY);
	//printf("START  %dx%d (%dx%d)\n", startX, startY, rootnode[startY][startX]->c, rootnode[startY][startX]->r);
	//printf("TARGET %dx%d (%dx%d)\n", targetX, targetY, rootnode[targetY][targetX]->c, rootnode[targetY][targetX]->r);

	Open->_reset_list();
	Close->_reset_list();
	cAsSortList::_asitem startitem(start_node, 0, 0, 0);

	int flag_root_node = start_node->flag;
	start_node->flag = 0;

	Open->PushItem(&startitem);

	_asnode_base* node = (_asnode_base*)-1;

	int steps = 0;

	do
	{
		cAsSortList::_asitem parent;

		if(!Open->PopMinItem(&parent)){
			//printf("EOF\n");
			start_node->flag = flag_root_node;
			return 0;
		}
		node = parent.node;

		if(node->flag != -1)
		{
			//node->check=parent.steps;
			//printf("%d,%d %d\n",node->y,node->x,node->flag);
			if((node == target_node))// || ((node->x==targetX-1 || node->x==targetX+1) && (node->y==targetY-1 || node->y==targetY+1)))
			{
				Close->PushItem(parent.node, parent.parent, parent.steps);
				start_node->flag = flag_root_node;

				//printf("FOUND %dx%d (%dx%d)\n", node->c, node->r, rootnode[node->r][node->c]->c, rootnode[node->r][node->c]->r);
				return build_path();
			}

			//_LINK dirpart[4] = { _LINK::NS, _LINK::EW, _LINK::SN, _LINK::WE };

			for(int lid = 0; lid < node->links_count; ++lid)
			{
				_asnode_base* child = node->link_nodes[lid];

				if(child->flag != -1)
				{
					cAsSortList::_asitem* openitem = Open->GetItem(child);
					cAsSortList::_asitem* closeitem = Close->GetItem(child);

					if(!closeitem)
					{
						if(!openitem){
							Open->PushItem(child, parent.node, parent.steps + 1);
						}
						else{
							if(openitem->steps > parent.steps){
								openitem->steps = parent.steps + 1;
								openitem->parent = parent.node;
							}
						}
					}
					else{
						if(closeitem->steps > parent.steps){
							Open->PushItem(closeitem->node, parent.node, parent.steps + 1);
							Close->DeleteItem(closeitem);
						}
					}
				}
			}//if not out size
		}
		else{
			printf("FLAG==-1\n");
		}

		Close->PushItem(parent.node, parent.parent, parent.steps);

		++steps;
	} while(node != (_asnode_base*)-1);

	start_node->flag = flag_root_node;
	return 0;
}