#include "character.h"
#include <engine/common/timer.h>
#include <engine/common/exception.h>
#include <engine/debug/primitives.h>
#include <engine/map/map.h>
#include "unit.h"

#include <BulletCollision/CollisionShapes/btCapsuleShape.h>
#include <LinearMath/btDefaultMotionState.h>

const char* cCharacter::action_names[] = {
	"NONE",
	"IDLE",
	"WALK",
	"RUN",
	"ATTACK",
	"ACTIONS_COUNT"
};

extern cTimer Timer;

void cCharacter::release()
{
}

static bool CollideLandscapePerFrame = false;
static int CollideLandscapePerFrameContactsCount = 0;
static vec3 CollideLandscapePerFrameContactPoints[24];

float angle_point(vec2 a, vec2 b, vec2 c)
{
	vec2 A = normalize(a - c);
	vec2 B = normalize(b - c);
	return acos(dot(A, B));
}

void cCharacter::CollideCallback(const btRigidBody* SelfRigidBody, const vec3& contact, const btRigidBody* pCollisionRigidBody)
{
	/*btTransform rb_mx = RigidBodys[0]->getWorldTransform();
	vec3 centre = vec3(rb_mx.getOrigin().x(), rb_mx.getOrigin().y(), rb_mx.getOrigin().z());
	vec3 forward = GetForwardVector();
	vec3 contact_top = vec3(contact.x, centre.y, contact.z);
	//DrawCross(centre, 0.3f, "\xff\x00\x00");

	char* h_color = (char*)"\x00\x00\xff";
	float h_angle = angle_point(vec2(contact.x, contact.z), vec2(forward.x, forward.z), vec2(centre.x, centre.z));
	if(h_angle > 0.0f && h_angle < 1.0f && (centre.y - contact.y) < 0.4f){
		h_color = (char*)"\xff\x00\x00";
	}

	//DrawCross(forward, 0.3f, "\x00\x00\xff");
	DrawCross(contact, 0.3f, h_color);
	//DrawCross(vec3(centre.x, contact.y, centre.z), 0.3f, v_color);

	printf("%f\n", centre.y - contact.y);*/
	//printf("cCharacter::CollideCallback()\n");
	
	//cCollidedObject::CollideCallback(SelfRigidBody, pCollisionRigidBody);
	//printf("collide time:%f\n", Timer.ProgrammTime);

	const void* userptr = pCollisionRigidBody->getUserPointer();

	if(!userptr){
		return;
	}

	cObjectModel* pcObjectModel = (cObjectModel*)userptr;
	cCollidedObject* CollidedObject = (cCollidedObject*)userptr;

	//printf("CollideCallback: %s->%s %f\n", name, pcObjectModel->name, Timer.ProgrammTime);

	COLLISION_GROUPS group = COLLISION_GROUP_NOTHING;
	for(int i = 0; i < CollidedObject->RigidBodysCount; ++i)
	{
		//printf(" %p==%p\n", CollidedObject->RigidBodys[i], pCollisionRigidBody);
		if(CollidedObject->RigidBodys[i] == pCollisionRigidBody){
			group = CollidedObject->CollideGroups[i];
			break;
		}
	}

	if(group == COLLISION_GROUP_LANDSCAPE || group == COLLISION_GROUP_WALLS)
	{
		CollideLandscapePerFrame = true;

		if(CollideLandscapePerFrameContactsCount < 24){
			CollideLandscapePerFrameContactPoints[CollideLandscapePerFrameContactsCount++] = contact;
		}

		if(JumpStat == JUMP_STAT::LOST_CONTACT_LANDSCEPE)
		{
			printf("collide landscape time:%f\n", Timer.ProgrammTime - dbg_JumpFlyStartTime);
			JumpEnd();
		}/*else if(JumpStat == JUMP_STAT::FIRST_COLL_LANDSCEPE)
		{
			btVector3 vel = RigidBodys[0]->getLinearVelocity();
			printf("collide landscape (%f %f %f) time:%f\n", vel.x(), floor(vel.y() * 100.0f) / 100.0f, vel.z(), Timer.ProgrammTime - dbg_JumpFlyStartTime);

			//if(floor(vel.y() * 100.0f) / 100.0f <= 0.2f)
			{
				JumpEnd();
			}
		}*/
	}
}

void cCharacter::UpdateTransAnimPart(BODY_PART part)
{
	if(CurAnimIdPart[part] == ANIMATION_ID::TRANS && NextAnimIdPart[part] != ANIMATION_ID::NONE && !AnimTabParts[part][CurAnimIdPart[part]]->PlayFlag){
		//printf("end trans set next:%s\n", AnimTabParts[part][NextAnimIdPart[part]]->name);
		SetAnimAction(NextAnimIdPart[part], part);
	}
}

void cCharacter::UpdateSkeletonPart(BODY_PART part)
{
	AnimTabParts[part][CurAnimIdPart[part]]->Update();

	//printf("Torso BonesFramesCount:%d\n", AnimTorso[CurrentActionId]->BonesFramesCount);
	for(int i = 0; i < AnimTabParts[part][CurAnimIdPart[part]]->BonesFramesCount; ++i)
	{
		//printf("bone:%d\n", TorsoIds[i]);
		pSkeleton->Bones[PartBoneIds[part][i]].KeyState.position = AnimTabParts[part][CurAnimIdPart[part]]->BonesFrames[i].KeyState.position;
		pSkeleton->Bones[PartBoneIds[part][i]].KeyState.rotation = AnimTabParts[part][CurAnimIdPart[part]]->BonesFrames[i].KeyState.rotation;
		pSkeleton->Bones[PartBoneIds[part][i]].KeyState.scale = AnimTabParts[part][CurAnimIdPart[part]]->BonesFrames[i].KeyState.scale;
	}
}

void cCharacter::UpdateAttachments()
{
	for(map<int, _attach_item>::iterator it = AttachedMatrices.begin(); it != AttachedMatrices.end(); ++it)
	{
		int character_bone_id = it->first;

		cSkeleton* ObjectSkeleton = it->second.pSkeleton;
		int object_bone_id = it->second.bone_id;

		mat4 object_bone =
			translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f)) *
			mat4_cast(angleAxis(pi<float>() / 4.0f, vec3(0.0f, 1.0f, 0.0f))) * mat4_cast(angleAxis(pi<float>() / 2.0f, vec3(1.0f, 0.0f, 0.0f))) *
			scale(mat4(1.0f), vec3(1.0f, 1.0f, 1.0f));

		mat4 character_bone = WorldMx * bones_transforms[character_bone_id];

		*it->second.pmx_world = character_bone * object_bone;

		//vec3 cross_vec((character_bone * object_bone)[3]);

		//DrawCross(pCamera, cross_vec, 0.1f, "\xff\x00\x00");
		//DrawLine(pCamera, vec3(character_bone[3]), vec3(character_bone[3]) + normalize(vec3(character_bone[0])), "\xff\x00\x00");
	}
}

void calculate_bones_r(cBone* bones, mat4* result_buff, int break_bone, cBone* cur_bone, const mat4& ParentMatrix)
{
	int id = cur_bone->Id;

	quat qr = bones[id].KeyState.rotation;
	vec3 vp = bones[id].KeyState.position;
	vec3 vs = bones[id].KeyState.scale;

	mat4 mx_anim =
		translate(mat4(1.0f), vp) *
		mat4_cast(qr) *
		scale(mat4(1.0f), vs);

	result_buff[id] = ParentMatrix * mx_anim;

	if(id == break_bone)return;

	if(cur_bone->pSibling)calculate_bones_r(bones, result_buff, break_bone, cur_bone->pSibling, ParentMatrix);
	if(cur_bone->pChildren)calculate_bones_r(bones, result_buff, break_bone, cur_bone->pChildren, result_buff[id]);
}

void cCharacter::Update()
{
	if(JumpStat >= JUMP_STAT::STARTING)
	{
		if(JumpStat < JUMP_STAT::FLY)
		{
			if(CurAnimIdPart[BODY_PART::LEGS] == ANIMATION_ID::JUMP &&  AnimTabParts[BODY_PART::LEGS][ANIMATION_ID::JUMP]->TimeCurrent >= 5.0f)
			{
				bool break_jump = false;
				for(int i = 0; i < CollideLandscapePerFrameContactsCount; ++i)
				{
					btTransform rb_mx = RigidBodys[0]->getWorldTransform();
					btVector3 rb_pos = rb_mx.getOrigin();
					vec3 centre = vec3(rb_pos.x(), rb_pos.y(), rb_pos.z());
					vec3 forward = GetForwardVector();

					float h_angle = angle_point(vec2(CollideLandscapePerFrameContactPoints[i].x, CollideLandscapePerFrameContactPoints[i].z), vec2(forward.x, forward.z), vec2(centre.x, centre.z));

					if(h_angle > 0.0f && h_angle < 1.0f && (centre.y - CollideLandscapePerFrameContactPoints[i].y) < 0.4f){
						break_jump = true;
						printf("break jump %f %f\n", h_angle, (centre.y - CollideLandscapePerFrameContactPoints[i].y));
						break;
					}
				}
				
				if(break_jump)
				{
					JumpEnd();
				}
				else
				{
					JumpStat = JUMP_STAT::FLY;
					float f = MovingRunFlag ? 50.0f / Timer.fps : 100.0f / Timer.fps;

					printf("fly JumpStat:%d (%f %f) t:%f\n", JumpStat, btMovingVelocity.x() * f, btMovingVelocity.z() * f, Timer.ProgrammTime);

					RigidBodys[0]->setLinearVelocity(btVector3(btMovingVelocity.x() * f, 25.0f, btMovingVelocity.z() * f));

					dbg_StartJumpPos = RigidBodys[0]->getWorldTransform().getOrigin();
					dbg_JumpFlyStartTime = Timer.ProgrammTime;
				}
			}
		}
		else if(JumpStat == JUMP_STAT::FLY && CollideLandscapePerFrame){
			if(Timer.ProgrammTime - dbg_JumpFlyStartTime > 0.1f)
			{
				printf("stop jump time limit\n");
				JumpEnd();
			}
		}
	}

	CollideLandscapePerFrame = false;
	CollideLandscapePerFrameContactsCount = 0;

	/*else if(MovingForwardFlag)
	{
		btVector3 linear_velocity = RigidBody->getLinearVelocity();
		printf("lv:%f %f %f mv:%f %f %f\n", linear_velocity.x(), linear_velocity.y(), linear_velocity.z(), btMovingVelocity.x(), btMovingVelocity.y(), btMovingVelocity.z());
	}*/

	UpdateSkeletonPart(BODY_PART::TORSO);
	UpdateSkeletonPart(BODY_PART::LEGS);

	//выравнивание торса под легс
	calculate_bones_r(pSkeleton->Bones, bones_transforms, PartBoneIds[BODY_PART::TORSO][0], &pSkeleton->Bones[0], mat4(1.0f));

	quat bone_bq = angleAxis(dbg_dir.y, vec3(0.0f, 1.0f, 0.0f)) * quat(0.5f, -0.5f, -0.3f, -0.3f);

	pSkeleton->Bones[attach_parts_bones[BODY_PART::TORSO]].KeyState.rotation = 
		pSkeleton->Bones[attach_parts_bones[BODY_PART::TORSO]].rotation * 
		quat_cast(inverse(bones_transforms[attach_parts_bones[BODY_PART::LEGS]])) * 
		bone_bq;

	pSkeleton->Bones[attach_parts_bones[BODY_PART::TORSO]].KeyState.position = vec3();

	pSkeleton->Update();

	if(CurAnimIdPart[BODY_PART::TORSO] == ANIMATION_ID::ATTACK && !AnimTabParts[BODY_PART::TORSO][CurAnimIdPart[BODY_PART::TORSO]]->PlayFlag)
	{
		//printf("end:%s\n", AnimTabParts[BODY_PART::TORSO][CurAnimIdPart[BODY_PART::TORSO]]->name);
		//printf("trans to last:%s\n", AnimTabParts[BODY_PART::TORSO][LastAnimIdPart[BODY_PART::TORSO]]->name);
		_setup_trans_animation(
			AnimTabParts[BODY_PART::TORSO][ANIMATION_ID::TRANS],
			AnimTabParts[BODY_PART::TORSO][CurAnimIdPart[BODY_PART::TORSO]],
			AnimTabParts[BODY_PART::TORSO][LastAnimIdPart[BODY_PART::TORSO]],
			char_trans_length
		);

		NextAnimIdPart[BODY_PART::TORSO] = LastAnimIdPart[BODY_PART::TORSO];

		SetAnimAction(ANIMATION_ID::TRANS, BODY_PART::TORSO);
	}
	else{
		UpdateTransAnimPart(BODY_PART::TORSO);
	}

	UpdateTransAnimPart(BODY_PART::LEGS);

	//AnimTabParts[BODY_PART::LEGS][CurAnimIdPart[BODY_PART::LEGS]]
	//cSkeletonObject::Update();
	cUnitBase::Update();
}

void cCharacter::UpdateAfterTransforms()
{
	cSkeletonObject::UpdateAfterTransforms();

	if(!CollideLandscapePerFrame){
		if(JumpStat == JUMP_STAT::FLY){
			JumpStat = JUMP_STAT::LOST_CONTACT_LANDSCEPE;
		}
	}
}

void cCharacter::Draw(cCamera* pCamera)
{
	calculate_bones_r(pSkeleton->Bones, bones_transforms, pSkeleton->BonesCount, &pSkeleton->Bones[0], mat4(1.0f));

	UpdateAttachments();

	/*mat4 bone_amx = bones_transforms[3];
	mat4 bone_bmx = bones_transforms[4];*/

	//DrawCross(pCamera, vec3((worldmx * bone_amx)[3]), 0.1f, "\xff\x00\x00");
	/*DrawLine(
		vec3((WorldMx * bones_transforms[attach_parts_bones[BODY_PART::LEGS]])[3]),
		vec3((WorldMx * bones_transforms[attach_parts_bones[BODY_PART::LEGS]])[3]) + normalize(-vec3(bones_transforms[attach_parts_bones[BODY_PART::LEGS]][2])) * 0.3f,
		"\xff\x00\x00"
	);
	
	DrawLine(
		vec3((WorldMx * bones_transforms[attach_parts_bones[BODY_PART::TORSO]])[3]),
		vec3((WorldMx * bones_transforms[attach_parts_bones[BODY_PART::TORSO]])[3]) + normalize(-vec3(bones_transforms[attach_parts_bones[BODY_PART::TORSO]][2])) * 1.0f,
		"\x00\x00\xff"
	);*/

	/*quat tqa = quat_cast(bones_transforms[3]);
	quat tqb = quat_cast(bones_transforms[4]);
	printf("%f %f %f %f\n", tqa.x, tqa.y, tqa.z, tqa.w);
	printf("%f %f %f %f\n\n", tqb.x, tqb.y, tqb.z, tqb.w);*/

	//DrawCross(pCamera, vec3((worldmx* bone_bmx)[3]), 0.1f, "\xff\x00\x00");
	//DrawLine(pCamera, vec3((worldmx* bone_bmx)[3]), vec3((worldmx* bone_bmx)[3]) + normalize(-vec3(bone_amx[2])) * 0.3f, "\x00\x00\xff");

	//DrawCross(pCamera, vec3((worldmx* bones_transforms[3])[3]), 0.1f, "\x00\xff\x00");
	//DrawCross(pCamera, vec3((worldmx* bones_transforms[4])[3]), 0.1f, "\x00\xff\x00");

	cSkeletonObject::Draw(pCamera);

	//cObjectModel::DBG_DrawNormals(pCamera);
}

void cCharacter::AddAnimationPart(ANIMATION_ID ActionId, BODY_PART part, cAnimation* Animation)
{
	if(AnimTabParts[part][ActionId]){
		delete AnimTabParts[part][ActionId];
	}

	AnimTabParts[part][ActionId] = new cAnimation();

	AnimTabParts[part][ActionId]->set_name(Animation->name);

	AnimTabParts[part][ActionId]->BonesFramesCount = PartBoneIds[part].count;
	AnimTabParts[part][ActionId]->BonesFrames = new _bone_frame[AnimTabParts[part][ActionId]->BonesFramesCount];
	AnimTabParts[part][ActionId]->TimeEnd = Animation->TimeEnd;

	for(int i = 0; i < AnimTabParts[part][ActionId]->BonesFramesCount; ++i)
	{
		Animation->BonesFrames[PartBoneIds[part][i]].copy_keys(&AnimTabParts[part][ActionId]->BonesFrames[i]);
	}
	AnimTabParts[part][ActionId]->_setup_default_frame();
}

void cCharacter::AddAnimation(ANIMATION_ID ActionId, cAnimation* Animation)
{
	AddAnimationPart(ActionId, BODY_PART::LEGS, Animation);
	AddAnimationPart(ActionId, BODY_PART::TORSO, Animation);
}

bool cCharacter::SetAnimAction(ANIMATION_ID ActionId, BODY_PART part)
{
	if(part != BODY_PART::ALL)
	{

		if(!AnimTabParts[part][ActionId])return false;

		CurAnimIdPart[part] = ActionId;

		AnimTabParts[part][CurAnimIdPart[part]]->Play();
	}
	else{
		if(!AnimTabParts[BODY_PART::TORSO][ActionId] || !AnimTabParts[BODY_PART::LEGS][ActionId])return false;

		CurAnimIdPart[BODY_PART::TORSO] = ActionId;
		CurAnimIdPart[BODY_PART::LEGS] = ActionId;

		AnimTabParts[BODY_PART::TORSO][CurAnimIdPart[BODY_PART::TORSO]]->Play();
		AnimTabParts[BODY_PART::LEGS][CurAnimIdPart[BODY_PART::LEGS]]->Play();
	}
	
	//printf("SetAnimAction(%d:%s)\n", part, AnimTabParts[part][CurAnimIdPart[part]]->name);
	//system("pause");
	//Sleep(1000);
	//dbg_pause = true;
	return true;
}

bool cCharacter::IsPlayAttack()
{
	return CurAnimIdPart[BODY_PART::TORSO] == ANIMATION_ID::ATTACK || NextAnimIdPart[BODY_PART::TORSO] == ANIMATION_ID::ATTACK;
}

void cCharacter::MoveForward(bool run)
{
	//printf("MoveForward %d\n",run);
	MovingForwardFlag = true;
	MovingBackwardFlag = false;
	MovingRunFlag = false;

	vec3 direction = GetDirection();

	float ms = MovingSpeed;
	ANIMATION_ID ft = ANIMATION_ID::WALK;

	if(run)
	{
		MovingRunFlag = true;
		ms = MovingSpeed * 2.0f;
		ft = ANIMATION_ID::RUN;
	}

	btMovingVelocity = (btVector3(direction.x, direction.y, direction.z) * ms);

	RigidBodys[0]->setLinearVelocity(btMovingVelocity);

	if(CurAnimIdPart[BODY_PART::LEGS] != ft && NextAnimIdPart[BODY_PART::LEGS] != ft)
	{
		if(AnimTabParts[BODY_PART::LEGS][CurAnimIdPart[BODY_PART::LEGS]])
		{
			_setup_trans_animation(
				AnimTabParts[BODY_PART::LEGS][ANIMATION_ID::TRANS],
				AnimTabParts[BODY_PART::LEGS][CurAnimIdPart[BODY_PART::LEGS]],
				AnimTabParts[BODY_PART::LEGS][ft],
				char_trans_length
			);

			NextAnimIdPart[BODY_PART::LEGS] = ft;
			//printf("\tSetAnimAction %d\n", ANIMATION_ID::TRANS);
			SetAnimAction(ANIMATION_ID::TRANS, BODY_PART::LEGS);
		}
		else{
			//printf("\tSetAnimAction %d\n", ft);
			SetAnimAction(ft, BODY_PART::LEGS);
		}
	}

	//AnimTabParts[BODY_PART::LEGS][CurAnimIdPart[BODY_PART::LEGS]]->DebugMod = true;
}

void cCharacter::MoveBackward()
{
	//printf("MoveBackward\n");
	MovingForwardFlag = false;
	MovingBackwardFlag = true;

	vec3 direction = GetDirection();
	btMovingVelocity = btVector3(direction.x, direction.y, direction.z) * -MovingSpeed;

	RigidBodys[0]->setLinearVelocity(btMovingVelocity);

	if(CurAnimIdPart[BODY_PART::LEGS] != ANIMATION_ID::BACKWARD && NextAnimIdPart[BODY_PART::LEGS] != ANIMATION_ID::BACKWARD)
	{
		if(AnimTabParts[BODY_PART::LEGS][CurAnimIdPart[BODY_PART::LEGS]])
		{
			_setup_trans_animation(
				AnimTabParts[BODY_PART::LEGS][ANIMATION_ID::TRANS],
				AnimTabParts[BODY_PART::LEGS][CurAnimIdPart[BODY_PART::LEGS]],
				AnimTabParts[BODY_PART::LEGS][ANIMATION_ID::BACKWARD],
				char_trans_length
			);

			NextAnimIdPart[BODY_PART::LEGS] = ANIMATION_ID::BACKWARD;

			SetAnimAction(ANIMATION_ID::TRANS, BODY_PART::LEGS);
		}
		else{
			SetAnimAction(ANIMATION_ID::BACKWARD, BODY_PART::LEGS);
		}
	}
}

void cCharacter::MoveStop()
{
	MovingForwardFlag = false;
	MovingBackwardFlag = false;
	MovingRunFlag = false;

	btMovingVelocity = btVector3(0.0f, 0.0f, 0.0f);
	RigidBodys[0]->setLinearVelocity(btMovingVelocity);

	if(CurAnimIdPart[BODY_PART::LEGS] != ANIMATION_ID::IDLE)
	{
		//float trans_duration = AnimTabParts[BODY_PART::TORSO][CurAnimIdPart[BODY_PART::TORSO]]->TimeEnd - AnimTabParts[BODY_PART::TORSO][CurAnimIdPart[BODY_PART::TORSO]]->TimeCurrent;
		//printf("%f\n", trans_duration);

		if(AnimTabParts[BODY_PART::LEGS][CurAnimIdPart[BODY_PART::LEGS]])
		{
			_setup_trans_animation(
				AnimTabParts[BODY_PART::LEGS][ANIMATION_ID::TRANS],
				AnimTabParts[BODY_PART::LEGS][CurAnimIdPart[BODY_PART::LEGS]],
				AnimTabParts[BODY_PART::LEGS][ANIMATION_ID::IDLE],
				char_trans_length
			);

			NextAnimIdPart[BODY_PART::LEGS] = ANIMATION_ID::IDLE;

			SetAnimAction(ANIMATION_ID::TRANS, BODY_PART::LEGS);
		}
		else{
			SetAnimAction(ANIMATION_ID::IDLE, BODY_PART::LEGS);
		}
	}
}

void cCharacter::Jump()
{
	dbg_JumpStartTime = Timer.ProgrammTime;
	
	//printf("Jump()\n");
	JumpingFlag = true;
	JumpingFlyFlag = false;

	JumpStat = JUMP_STAT::STARTING;

	//RigidBody->setLinearVelocity(btVector3(btMovingVelocity.x(), 5.0f, btMovingVelocity.z()));

	if(CurAnimIdPart[BODY_PART::LEGS] != ANIMATION_ID::JUMP && NextAnimIdPart[BODY_PART::LEGS] != ANIMATION_ID::JUMP)
	{
		if(AnimTabParts[BODY_PART::LEGS][CurAnimIdPart[BODY_PART::LEGS]])
		{
			//LastAnimIdPart[BODY_PART::TORSO] = CurAnimIdPart[BODY_PART::TORSO];
			//LastAnimIdPart[BODY_PART::LEGS] = CurAnimIdPart[BODY_PART::LEGS];

			/*_setup_trans_animation(
				AnimTabParts[BODY_PART::TORSO][ANIMATION_ID::TRANS],
				AnimTabParts[BODY_PART::TORSO][CurAnimIdPart[BODY_PART::TORSO]],
				AnimTabParts[BODY_PART::TORSO][ANIMATION_ID::JUMP],
				char_trans_length
			);*/

			_setup_trans_animation(
				AnimTabParts[BODY_PART::LEGS][ANIMATION_ID::TRANS],
				AnimTabParts[BODY_PART::LEGS][CurAnimIdPart[BODY_PART::LEGS]],
				AnimTabParts[BODY_PART::LEGS][ANIMATION_ID::JUMP],
				char_trans_length
			);

			//NextAnimIdPart[BODY_PART::TORSO] = ANIMATION_ID::JUMP;
			NextAnimIdPart[BODY_PART::LEGS] = ANIMATION_ID::JUMP;

			SetAnimAction(ANIMATION_ID::TRANS, BODY_PART::LEGS);
		}
		else{
			SetAnimAction(ANIMATION_ID::JUMP, BODY_PART::LEGS);
		}

		//AnimTabParts[BODY_PART::LEGS][ANIMATION_ID::JUMP]->TimeCurrent = 0.0f;
		//printf("TimeCurrent:%f\n", AnimTabParts[BODY_PART::LEGS][ANIMATION_ID::JUMP]->TimeCurrent);
	}
}

void cCharacter::JumpEnd()
{
	dbg_EndJumpPos = RigidBodys[0]->getWorldTransform().getOrigin();

	printf("JumpEnd(%f)\n", dbg_StartJumpPos.distance(dbg_EndJumpPos));

	//vec3 sp(dbg_StartJumpPos.x(), dbg_StartJumpPos.y(), dbg_StartJumpPos.z());
	//vec3 se(dbg_EndJumpPos.x(), dbg_EndJumpPos.y(), dbg_EndJumpPos.z());
	//printf("       (%f %f %f)->(%f %f %f)\n", dbg_StartJumpPos.x(), dbg_StartJumpPos.y(), dbg_StartJumpPos.z(), dbg_EndJumpPos.x(), dbg_EndJumpPos.y(), dbg_EndJumpPos.z());
	JumpStat = JUMP_STAT::NONE;

	JumpingFlag = false;
	JumpingFlyFlag = false;

	if(MovingForwardFlag){

		float ms = MovingSpeed;
		ANIMATION_ID ft = ANIMATION_ID::WALK;

		if(MovingRunFlag)
		{
			MovingRunFlag = true;
			ms = MovingSpeed * 2.0f;
			ft = ANIMATION_ID::RUN;
		}

		vec3 direction = GetDirection();
		btMovingVelocity = btVector3(direction.x, direction.y, direction.z) * ms;

		RigidBodys[0]->setLinearVelocity(btMovingVelocity);

		NextAnimIdPart[BODY_PART::LEGS] = ft;

	}else if(MovingBackwardFlag){
		NextAnimIdPart[BODY_PART::LEGS] = ANIMATION_ID::BACKWARD;
	}
	else{
		NextAnimIdPart[BODY_PART::LEGS] = ANIMATION_ID::IDLE;
	}

	/*_setup_trans_animation(
		AnimTabParts[BODY_PART::TORSO][ANIMATION_ID::TRANS],
		AnimTabParts[BODY_PART::TORSO][CurAnimIdPart[BODY_PART::TORSO]],
		AnimTabParts[BODY_PART::TORSO][NextAnimIdPart[BODY_PART::TORSO]],
		char_trans_length
	);*/

	_setup_trans_animation(
		AnimTabParts[BODY_PART::LEGS][ANIMATION_ID::TRANS],
		AnimTabParts[BODY_PART::LEGS][CurAnimIdPart[BODY_PART::LEGS]],
		AnimTabParts[BODY_PART::LEGS][NextAnimIdPart[BODY_PART::LEGS]],
		char_trans_length
	);

	//NextAnimIdPart[BODY_PART::TORSO] = ANIMATION_ID::IDLE;

	//printf("next:%s\n", AnimTabParts[BODY_PART::LEGS][NextAnimIdPart[BODY_PART::LEGS]]->name);

	SetAnimAction(ANIMATION_ID::TRANS, BODY_PART::LEGS);
}

void cCharacter::Attack()
{
	if(CurAnimIdPart[BODY_PART::TORSO] != ANIMATION_ID::ATTACK && NextAnimIdPart[BODY_PART::TORSO] != ANIMATION_ID::ATTACK)
	{
		if(AnimTabParts[BODY_PART::TORSO][CurAnimIdPart[BODY_PART::TORSO]])
		{
			LastAnimIdPart[BODY_PART::TORSO] = CurAnimIdPart[BODY_PART::TORSO];

			_setup_trans_animation(
				AnimTabParts[BODY_PART::TORSO][ANIMATION_ID::TRANS],
				AnimTabParts[BODY_PART::TORSO][CurAnimIdPart[BODY_PART::TORSO]],
				AnimTabParts[BODY_PART::TORSO][ANIMATION_ID::ATTACK],
				char_trans_length
			);

			NextAnimIdPart[BODY_PART::TORSO] = ANIMATION_ID::ATTACK;

			SetAnimAction(ANIMATION_ID::TRANS, BODY_PART::TORSO);
		}
		else{
			SetAnimAction(ANIMATION_ID::ATTACK, BODY_PART::TORSO);
		}
	}
}

void cCharacter::AttachSkeletonObject(cSkeletonObject* pSkeletonObject, const char* attached_bone, const char* attaching_bone)
{
	int pattached_id = 0;
	for(int i = 0; i < pSkeletonObject->pSkeleton->BonesCount; ++i)
	{
		if(!strcmp(pSkeletonObject->pSkeleton->Bones[i].Name, attached_bone))
		{
			pattached_id = i;
		}
	}

	int attaching_id = 0;
	for(int i = 0; i < pSkeleton->BonesCount; ++i)
	{
		if(!strcmp(pSkeleton->Bones[i].Name, attaching_bone))
		{
			attaching_id = i;
		}
	}

	AttachedMatrices[attaching_id] = _attach_item(pSkeletonObject->pSkeleton, pattached_id, &pSkeletonObject->WorldMx);
}