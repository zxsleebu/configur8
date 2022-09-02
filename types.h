#pragma once
#include <cstdint>
#include "memory.h"

struct Vector
{
	float x, y, z;
};

enum CommandButtons : int
{
	IN_ATTACK = (1 << 0),
	IN_JUMP = (1 << 1),
	IN_DUCK = (1 << 2),
	IN_FORWARD = (1 << 3),
	IN_BACK = (1 << 4),
	IN_USE = (1 << 5),
	IN_CANCEL = (1 << 6),
	IN_LEFT = (1 << 7),
	IN_RIGHT = (1 << 8),
	IN_MOVELEFT = (1 << 9),
	IN_MOVERIGHT = (1 << 10),
	IN_SECOND_ATTACK = (1 << 11),
	IN_RUN = (1 << 12),
	IN_RELOAD = (1 << 13),
	IN_LEFT_ALT = (1 << 14),
	IN_RIGHT_ALT = (1 << 15),
	IN_SCORE = (1 << 16),
	IN_SPEED = (1 << 17),
	IN_WALK = (1 << 18),
	IN_ZOOM = (1 << 19),
	IN_FIRST_WEAPON = (1 << 20),
	IN_SECOND_WEAPON = (1 << 21),
	IN_BULLRUSH = (1 << 22),
	IN_FIRST_GRENADE = (1 << 23),
	IN_SECOND_GRENADE = (1 << 24),
	IN_MIDDLE_ATTACK = (1 << 25)
};

struct UserCmd
{
	void* vmt;
	int commandNumber;
	int tickCount;
	Vector viewPoint;
	Vector aimDirection;
	float forwardMove;
	float sideMove;
	float upMove;
	int buttons;
	char impulse;
	int weaponSelect;
	int weaponSubType;
	int randomSeed;
	short mouseDeltaX;
	short mouseDeltaY;
	bool hasBeenPredicted;
	Vector headAngles;
	Vector headOffset;
};

class NetworkChannel {
public:
	BYTE pad[44];
	int chokedPackets;
};

class AnimationLayer {
public:
	char  pad_0000[20];
	// These should also be present in the padding, don't see the use for it though
	//float	m_flLayerAnimtime;
	//float	m_flLayerFadeOuttime;
	unsigned int m_nOrder; //0x0014
	unsigned int m_nSequence; //0x0018
	float m_flPrevCycle; //0x001C
	float m_flWeight; //0x0020
	float m_flWeightDeltaRate; //0x0024
	float m_flPlaybackRate; //0x0028
	float m_flCycle; //0x002C
	void* m_pOwner; //0x0030 // player's thisptr
	char  pad_0038[4]; //0x0034
};

enum AnimationLayerIndex {
	AIMMATRIX = 0,
	WEAPON_ACTION,
	WEAPON_ACTION_RECROUCH,
	ADJUST,
	MOVEMENT_JUMP_OR_FALL,
	MOVEMENT_LAND_OR_CLIMB,
	MOVEMENT_MOVE,
	MOVEMENT_STRAFECHANGE,
	WHOLE_BODY,
	FLASHED,
	FLINCH,
	ALIVELOOP,
	LEAN
};

class CEntity
{
public:
	enum EFlags : std::int32_t
	{
		FL_ONGROUND = (1 << 0),
		FL_DUCKING = (1 << 1),
		FL_ANIMDUCKING = (1 << 2),
		FL_WATERJUMP = (1 << 3),
		FL_ONTRAIN = (1 << 4),
		FL_INRAIN = (1 << 5),
		FL_FROZEN = (1 << 6),
		FL_ATCONTROLS = (1 << 7),
		FL_CLIENT = (1 << 8),
		FL_FAKECLIENT = (1 << 9),
		FL_INWATER = (1 << 10),
		FL_FLY = (1 << 11),
		FL_SWIM = (1 << 12),
		FL_CONVEYOR = (1 << 13),
		FL_NPC = (1 << 14),
		FL_GODMODE = (1 << 15),
		FL_NOTARGET = (1 << 16),
		FL_AIMTARGET = (1 << 17),
		FL_PARTIALGROUND = (1 << 18),
		FL_STATICPROP = (1 << 19),
		FL_GRAPHED = (1 << 20),
		FL_GRENADE = (1 << 21),
		FL_STEPMOVEMENT = (1 << 22),
		FL_DONTTOUCH = (1 << 23),
		FL_BASEVELOCITY = (1 << 24),
		FL_WORLDBRUSH = (1 << 25),
		FL_OBJECT = (1 << 26),
		FL_KILLME = (1 << 27),
		FL_ONFIRE = (1 << 28),
		FL_DISSOLVING = (1 << 29),
		FL_TRANSRAGDOLL = (1 << 30),
		FL_UNBLOCKABLE_BY_PLAYER = (1 << 31)
	};
	constexpr std::int32_t GetIndex() noexcept
	{
		return Memory::Call<std::int32_t>(this + 0x8, 10);
	}
	constexpr bool IsAlive() noexcept
	{
		return Memory::Call<bool>(this, 155);
	}
	constexpr AnimationLayer* GetAnimLayers() noexcept
	{
		return *(AnimationLayer**)((DWORD)this + 0x2980);
	}
};

class CColor
{
public:
	BYTE r;
	BYTE g;
	BYTE b;
	BYTE a;
	bool operator==(CColor color) const { return r == color.r && g == color.g && b == color.b && a == color.a; }

	CColor(BYTE r, BYTE g, BYTE b, BYTE a = 255)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}
	const char* str() {
		static char buffer[25];
		sprintf(buffer, "(%d, %d, %d, %d)", r, g, b, a);
		return buffer;
	}
};