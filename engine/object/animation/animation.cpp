#include "animation.h"
#include <engine/common/timer.h>
#include <glm/gtx/compatibility.hpp>
#include <stdio.h>

#ifdef _DEBUG
#include "engine/debug/sizeof_memory.hpp"
#endif

extern cTimer Timer;

//extern float DBG_TestAnimTime;

void cKey::reset()
{
	time = 0;
	position = vec3(0.0f, 0.0f, 0.0f);
	scale = vec3(1.0f, 1.0f, 1.0f);
	rotation = quat(0.0f, 0.0f, 0.0f, 1.0f);
	AnimMod = word(_ANIM_MOD::NO);
}
cKey::cKey()
{
	reset();
}
cKey::~cKey()
{
}

cKeyB::cKeyB()
{
	time = 0;
}

float cKeyB::get_time()
{
	return time;
}

cKeyP::cKeyP()
{
	reset();
}
cKeyP::~cKeyP()
{
}
void cKeyP::reset()
{
	v = 0;
}

cKeyR::cKeyR()
{
	reset();
}
cKeyR::~cKeyR()
{
}
void cKeyR::reset()
{
	v = quat(0.0f, 0.0f, 0.0f, 1.0f);
}

cKeyS::cKeyS()
{
	reset();
}
cKeyS::~cKeyS()
{
}
void cKeyS::reset()
{
	v = vec3(1.0f, 1.0f, 1.0f);
}

void _bone_frame::KEY_TRANSFORM_STATE::reset()
{
	PrevId = 0;
	NextId = 0;
	StageFlag = 0;
	NextTime = 0.0;
}

_bone_frame::KEY_TRANSFORM_STATE::KEY_TRANSFORM_STATE()
{
	reset();
}

_bone_frame::_bone_frame()
{
	Id = -1;

	KeysX = 0;
	KeysY = 0;
	KeysZ = 0;
	KeysR = 0;
	KeysS = 0;
	KeysXNum = 0;
	KeysYNum = 0;
	KeysZNum = 0;
	KeysRNum = 0;
	KeysSNum = 0;
}
_bone_frame::~_bone_frame()
{
	if(KeysX){
		delete[]KeysX;
	}
	if(KeysY){
		delete[]KeysY;
	}
	if(KeysZ){
		delete[]KeysZ;
	}
	if(KeysR){
		delete[]KeysR;
	}
	if(KeysS){
		delete[]KeysS;
	}
}

void _bone_frame::copy_keys(_bone_frame* dest)
{
	dest->Id = Id;
	dest->KeysXNum = KeysXNum;
	dest->KeysYNum = KeysYNum;
	dest->KeysZNum = KeysZNum;
	dest->KeysRNum = KeysRNum;
	dest->KeysSNum = KeysSNum;

	if(KeysX){
		dest->KeysX = new cKeyP[KeysXNum];
		memcpy(dest->KeysX, KeysX, sizeof(cKeyP) * KeysXNum);
	}

	if(KeysY){
		dest->KeysY = new cKeyP[KeysYNum];
		memcpy(dest->KeysY, KeysY, sizeof(cKeyP) * KeysYNum);
	}

	if(KeysZ){
		dest->KeysZ = new cKeyP[KeysZNum];
		memcpy(dest->KeysZ, KeysZ, sizeof(cKeyP) * KeysZNum);
	}

	if(KeysR){
		dest->KeysR = new cKeyR[KeysRNum];
		memcpy(dest->KeysR, KeysR, sizeof(cKeyR) * KeysRNum);
	}

	if(KeysS){
		dest->KeysS = new cKeyS[KeysSNum];
		memcpy(dest->KeysS, KeysS, sizeof(cKeyP) * KeysSNum);
	}
}

void _bone_frame::set_first()
{
	if(KeysX){
		KeyState.position.x = KeysX[0].v;
	}
	else{
		KeyState.position.x = 0.0f;
	}

	if(KeysY){
		KeyState.position.y = KeysY[0].v;
	}
	else{
		KeyState.position.y = 0.0f;
	}

	if(KeysZ){
		KeyState.position.z = KeysZ[0].v;
	}
	else{
		KeyState.position.z = 0.0f;
	}

	if(KeysR){
		KeyState.rotation = KeysR[0].v;
	}
	else{
		KeyState.rotation = quat();
	}

	if(KeysS){
		KeyState.scale = KeysS[0].v;
	}
	else{
		KeyState.scale = vec3(1.0f, 1.0f, 1.0f);
	}
}

void _bone_frame::set_last()
{
	if(KeysXNum > 0){
		KeyState.position.x = KeysX[KeysXNum - 1].v;
	}
	else{
		KeyState.position.x = 0.0f;
	}

	if(KeysYNum > 0){
		KeyState.position.y = KeysY[KeysYNum - 1].v;
	}
	else{
		KeyState.position.y = 0.0f;
	}

	if(KeysZNum > 0){
		KeyState.position.z = KeysZ[KeysZNum - 1].v;
	}
	else{
		KeyState.position.z = 0.0f;
	}

	if(KeysRNum > 0){
		KeyState.rotation = KeysR[KeysRNum - 1].v;
	}
	else{
		KeyState.rotation = quat();
	}

	if(KeysSNum > 0){
		KeyState.scale = KeysS[KeysSNum - 1].v;
	}
	else{
		KeyState.scale = vec3(1.0f, 1.0f, 1.0f);
	}
}

cAnimation::cAnimation()
{
	BonesFramesCount = 0;
	BonesFrames = 0;
	TimeStart = 0.0f;
	TimeEnd = 0.0f;
}

cAnimation::cAnimation(int frames_count, float start_time, float end_time)
{
	BonesFramesCount = frames_count;
	BonesFrames = new _bone_frame[BonesFramesCount];

	TimeStart = start_time;
	TimeEnd = end_time;
}

cAnimation::~cAnimation()
{
	if(BonesFrames){
		delete[] BonesFrames;
	}

	if(name){
		delete[] name;
	}
}

void cAnimation::set_name(char* n)
{
	int nlen = strlen(n);
	name = new char[nlen + 1];
	memcpy(name, n, nlen);
	name[nlen] = 0;
}

template <typename T>
void _get_next_time_scalar(float cur_time, T* keys, int keys_count, int* pret_timeid, float* pret_scalar)
{
	int NextId = 0;

	for(int ti = 0; ti < keys_count; ++ti)
	{
		if(keys[ti].time >= cur_time)
		{
			NextId = ti;
			break;
		}
	}

	*pret_timeid = NextId;
	*pret_scalar = ((cur_time - keys[NextId - 1].time) / (keys[NextId].time - keys[NextId - 1].time));
}

void cAnimation::_setup_default_frame()
{
	for(int bi = 0; bi < BonesFramesCount; ++bi)
	{
		BonesFrames[bi].set_first();
	}
}

bool _is_bone_excluded(int bid)
{
	int excluded_ids[] = { 0, 10, 22, 26, 109 };

	for(int i = 0; i < sizeof(excluded_ids) / 4; ++i)
	{
		if(bid == excluded_ids[i])return true;
	}

	return false;
}

void cAnimation::_frames_update(_bone_frame* bone_frame)
{
	/*bone_frame->KeyState.position.x = bone_frame->KeysX[0].v;
	bone_frame->KeyState.position.y = bone_frame->KeysY[0].v;
	bone_frame->KeyState.position.z = bone_frame->KeysZ[0].v;
	bone_frame->KeyState.rotation = bone_frame->KeysR[0].v;
	bone_frame->KeyState.scale = bone_frame->KeysS[0].v;*/

	float scalar = 0.0f;
	int NextId = 0;
	
	if(bone_frame->KeysXNum > 1)
	{
		_get_next_time_scalar(TimeCurrent, bone_frame->KeysX, bone_frame->KeysXNum, &NextId, &scalar);

		if(NextId > 0){
			bone_frame->KeyState.position.x = lerp(bone_frame->KeysX[NextId - 1].v, bone_frame->KeysX[NextId].v, scalar);
			//if(bid == 2)printf("X %f (%f)\n", bone_frame->KeyState.position.x, scalar);
		}
		else{
			//if(bid == 2)printf("END X %f\n", TimeCurrent);
			//bone_frame->KeyState.position.x = lerp(bone_frame->KeysX[bone_frame->KeysXNum-1].v, bone_frame->KeysX[0].v, scalar);
			//if(bid == 58)printf("END X %d\n", bid);
		}
	}
	else if(bone_frame->KeysYNum > 0){
		bone_frame->KeyState.position.x = bone_frame->KeysX[0].v;
	}

	if(bone_frame->KeysYNum > 1)
	{
		_get_next_time_scalar(TimeCurrent, bone_frame->KeysY, bone_frame->KeysYNum, &NextId, &scalar);

		if(NextId > 0){
			//if(bid == 2)printf("Y [%d]%f:%f (%f)\n", NextId, bone_frame->KeysY[NextId - 1].v, bone_frame->KeysY[NextId].v, scalar);
			bone_frame->KeyState.position.y = lerp(bone_frame->KeysY[NextId - 1].v, bone_frame->KeysY[NextId].v, scalar);
		}
		else{
			//if(bid==2)printf("END Y %f\n", TimeCurrent);
			//bone_frame->KeyState.position.y = lerp(bone_frame->KeysY[bone_frame->KeysYNum-1].v, bone_frame->KeysY[0].v, scalar);
		}
	}
	else if(bone_frame->KeysYNum > 0){
		bone_frame->KeyState.position.y = bone_frame->KeysY[0].v;
	}

	if(bone_frame->KeysZNum > 1)
	{
		_get_next_time_scalar(TimeCurrent, bone_frame->KeysZ, bone_frame->KeysZNum, &NextId, &scalar);

		if(NextId > 0){
			bone_frame->KeyState.position.z = lerp(bone_frame->KeysZ[NextId - 1].v, bone_frame->KeysZ[NextId].v, scalar);
			//if(bid == 2)printf("Z %f (%f)\n", bone_frame->KeyState.position.z, scalar);
		}
		else{
			//if(bid == 2)printf("END Z %f\n", TimeCurrent);
			//bone_frame->KeyState.position.z = lerp(bone_frame->KeysZ[bone_frame->KeysZNum-1].v, bone_frame->KeysZ[0].v, scalar);
		}
	}
	else if(bone_frame->KeysZNum > 0){
		bone_frame->KeyState.position.z = bone_frame->KeysZ[0].v;
	}
	
	if(bone_frame->KeysRNum > 1)
	{
		_get_next_time_scalar(TimeCurrent, bone_frame->KeysR, bone_frame->KeysRNum, &NextId, &scalar);

		if(NextId > 0){
			bone_frame->KeyState.rotation=slerp(bone_frame->KeysR[NextId - 1].v, bone_frame->KeysR[NextId].v, scalar);
		}
		else{
			//if(bid == 58)printf("END R %f %d\n", TimeCurrent, bone_frame->KeysRNum);
			//scalar = TimeEnd - TimeCurrent;
			//printf("(%f-%f)=%f\n", TimeCurrent, TimeEnd, scalar);
			//bone_frame->KeyState.rotation = slerp(bone_frame->KeysR[1].v, bone_frame->KeysR[bone_frame->KeysRNum - 1].v, scalar);
			//bone_frame->KeyState.rotation = bone_frame->KeysR[bone_frame->KeysRNum-1].v;

			/*printf(
				"%d %f\n%f %f %f %f\n%f %f %f %f\n",
				bid, bone_frame->KeysR[bone_frame->KeysRNum - 1].time,
				bone_frame->KeysR[bone_frame->KeysRNum - 1].v.x, bone_frame->KeysR[bone_frame->KeysRNum - 1].v.y, bone_frame->KeysR[bone_frame->KeysRNum - 1].v.z, bone_frame->KeysR[bone_frame->KeysRNum - 1].v.w,
				bone_frame->KeysR[1].v.x, bone_frame->KeysR[1].v.y, bone_frame->KeysR[1].v.z, bone_frame->KeysR[1].v.w
			);*/
		}
	}
	else if(bone_frame->KeysRNum > 0){
		bone_frame->KeyState.rotation = bone_frame->KeysR[0].v;
	}

	if(bone_frame->KeysS){
		_get_next_time_scalar(TimeCurrent, bone_frame->KeysS, bone_frame->KeysSNum, &NextId, &scalar);

		if(NextId > 0){
			bone_frame->KeyState.scale = lerp(bone_frame->KeysS[NextId - 1].v, bone_frame->KeysS[NextId].v, scalar);
		}
	}
	else if(bone_frame->KeysSNum > 0){
		bone_frame->KeyState.scale = bone_frame->KeysS[0].v;
	}
}

void cAnimation::Play()
{
	PlayStartTime = Timer.ProgrammTime;
	TimeCurrent = 0.0f;
	PlayFlag = true;
}

void cAnimation::Stop()
{
	PlayFlag = false;
}

int cAnimation::Update()
{
	int ret = 1;

	if(PlayFlag || DebugMod)
	{
		//if(!DebugMod){
		TimeCurrent = (Timer.ProgrammTime - PlayStartTime) * TimeSpeedFactor;
		//}

		//DebugMod

		if(DebugMod){
			printf("cAnimation::Update (%f : %f)\n", TimeCurrent, TimeEnd);
		}

		if(TimeCurrent >= TimeEnd)
		{
			//printf("END(%f-%f)\n",TimeCurrent,TimeEnd);
			PlayStartTime = Timer.ProgrammTime;
			TimeCurrent = 0.0f;

			for(int bi = 0; bi < BonesFramesCount; ++bi)
			{
				BonesFrames[bi].set_last();
			}
			
			if(PlayType == _ANIM_PLAY_TYPE::ONCE){
				PlayFlag = false;
				ret = 0;
			}
		}
		else{

			for(int bi = 0; bi < BonesFramesCount; ++bi)
			{
				_frames_update(&BonesFrames[bi]);
			}
		}

		return ret;
	}

	return ret;
}