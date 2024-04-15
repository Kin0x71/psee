#ifndef _ANIMATION_H_
#define _ANIMATION_H_

#include <engine/common/base_types.h>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>

using namespace glm;

enum class _ANIM_MOD : unsigned short int
{
	NO = 0,
	X = 1 << 1,
	Y = 1 << 2,
	Z = 1 << 3,
	R = 1 << 4,
	S = 1 << 5
};

class cKey
{
public:
	float time;
	vec3 position;
	vec3 scale;
	quat rotation;
	unsigned short AnimMod;

	cKey();
	~cKey();
	void reset();
};

class cKeyB
{
public:
	float time;

	cKeyB();

	float get_time();
};

class cKeyP : public cKeyB
{
public:
	float v;
	void reset();

	cKeyP();
	~cKeyP();
};

class cKeyR : public cKeyB
{
public:
	quat v;

	void reset();
	cKeyR();
	~cKeyR();
};

class cKeyS : public cKeyB
{
public:
	vec3 v;

	void reset();
	cKeyS();
	~cKeyS();
};

struct _bone_frame
{
public:
	int Id;

	cKeyP* KeysX;
	cKeyP* KeysY;
	cKeyP* KeysZ;
	cKeyR* KeysR;
	cKeyS* KeysS;
	uint KeysXNum;
	uint KeysYNum;
	uint KeysZNum;
	uint KeysRNum;
	uint KeysSNum;

	cKey KeyState;

	struct KEY_TRANSFORM_STATE
	{
		int PrevId;
		int NextId;
		int StageFlag;
		float Time;
		float NextTime;

		KEY_TRANSFORM_STATE();
		void reset();
	};

	KEY_TRANSFORM_STATE ktsX;
	KEY_TRANSFORM_STATE ktsY;
	KEY_TRANSFORM_STATE ktsZ;
	KEY_TRANSFORM_STATE ktsR;
	KEY_TRANSFORM_STATE ktsS;

public:
	_bone_frame();
	~_bone_frame();

	void copy_keys(_bone_frame* dest);
	void set_first();
	void set_last();

	/*_bone_frame operator = (const _bone_frame& src){
		return _bone_frame(src);
	}*/
};

enum class _KEY_TYPE
{
	P,
	R,
	S
};

enum class _ANIM_PLAY_TYPE
{
	LOOP,
	ONCE,
	SUB_RET,
	NEXT
};

class cAnimation{
public:

	_bone_frame* BonesFrames = 0;
	char* name = 0;

	int BonesFramesCount = 0;

	float TimeStart = 0.0f;
	float TimeEnd = 0.0f;
	float PlayStartTime = 0.0f;
	float TimeCurrent = 0.0f;
	float TimeSpeedFactor = 30.0f;
	bool PlayFlag = false;

	bool DebugMod = false;

	_ANIM_PLAY_TYPE PlayType = _ANIM_PLAY_TYPE::LOOP;

	cAnimation();
	cAnimation(int frames_count,float start_time, float end_time);
	~cAnimation();

	void set_name(char* n);

	void _setup_default_frame();
	
	void _frames_update(_bone_frame* bone_frame);

	void Play();
	void Stop();
	int Update();
};
#endif