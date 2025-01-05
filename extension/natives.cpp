#include <IForwardSys.h>
#include <sp_typeutil.h>
#include <sp_vm_types.h>
#include <mathlib/vector.h>
#include <basehandle.h>
#include <iservernetworkable.h>
#include <iserverunknown.h>
#include <safetyhook/common.hpp>
#include "left4dhooks.h"


class ZombieManagerExt
{
public:
	CBaseEntity *SpawnTank(const Vector &vecPos, const QAngle &vecAng);
	CBaseEntity *SpawnWitch(const Vector &vecPos, const QAngle &vecAng);
	CBaseEntity *SpawnSpecial(int zombieClass, const Vector &vecPos, const QAngle &vecAng);
	void SpawnMob(int amount);
};


template <typename R>
inline R GetVfuncAddress(void *obj, int index)
{
	uintptr_t *vtable = *(uintptr_t**)obj;
	return (R)vtable[index];
}

static ZombieManagerExt *TheZombieManager;
static void *TheDirector;

// CBaseEntity* ZombieManager::SpawnTank(Vector const&, QAngle const&)
// native int L4D2_SpawnTank(const float vecPos[3], const float vecAng[3]);
static cell_t L4D2_SpawnTank(IPluginContext *pContext, const cell_t *params)
{
	cell_t *vec, *ang;
	pContext->LocalToPhysAddr(params[1], &vec);
	pContext->LocalToPhysAddr(params[2], &ang);

	// A reference is never nullptr.
	// When a plugin passes NULL_VECTOR, it actually passes {0.0, 0.0, 0.0}, 
	// which is the default initialization value of `NULL_VECTOR[3]` in core.inc, if no one modifies it.
	Vector vecPos(sp_ctof(vec[0]), sp_ctof(vec[1]), sp_ctof(vec[2]));
	QAngle vecAng(sp_ctof(ang[0]), sp_ctof(ang[1]), sp_ctof(ang[2]));

	// Call the hook function directly.
	CBaseEntity *pEntity = TheZombieManager->SpawnTank(vecPos, vecAng);
	return gamehelpers->EntityToBCompatRef(pEntity);
}

// CBaseEntity* ZombieManager::SpawnWitch(Vector const&, QAngle const&)
// native int L4D2_SpawnWitch(const float vecPos[3], const float vecAng[3]);
static cell_t L4D2_SpawnWitch(IPluginContext *pContext, const cell_t *params)
{
	cell_t *vec, *ang;
	pContext->LocalToPhysAddr(params[1], &vec);
	pContext->LocalToPhysAddr(params[2], &ang);

	Vector vecPos(sp_ctof(vec[0]), sp_ctof(vec[1]), sp_ctof(vec[2]));
	QAngle vecAng(sp_ctof(ang[0]), sp_ctof(ang[1]), sp_ctof(ang[2]));

	CBaseEntity *pEntity = TheZombieManager->SpawnWitch(vecPos, vecAng);
	return gamehelpers->EntityToBCompatRef(pEntity);
}

// CBaseEntity* ZombieManager::SpawnSpecial(ZombieClassType, Vector const&, QAngle const&)
// native int L4D2_SpawnSpecial(int zombieClass, const float vecPos[3], const float vecAng[3]);
static cell_t L4D2_SpawnSpecial(IPluginContext *pContext, const cell_t *params)
{
	cell_t *vec, *ang;
	pContext->LocalToPhysAddr(params[2], &vec);
	pContext->LocalToPhysAddr(params[3], &ang);

	Vector vecPos(sp_ctof(vec[0]), sp_ctof(vec[1]), sp_ctof(vec[2]));
	QAngle vecAng(sp_ctof(ang[0]), sp_ctof(ang[1]), sp_ctof(ang[2]));

	CBaseEntity *pEntity = TheZombieManager->SpawnSpecial(params[1], vecPos, vecAng);
	return gamehelpers->EntityToBCompatRef(pEntity);
}


using fnGetRandomPZSpawnPosition = bool (SAFETYHOOK_THISCALL *)(ZombieManagerExt*, int, int, CBaseEntity*, Vector*);
static fnGetRandomPZSpawnPosition GetRandomPZSpawnPosition;
// bool ZombieManager::GetRandomPZSpawnPosition(ZombieClassType, int, CTerrorPlayer*, Vector*)
// native bool L4D_GetRandomPZSpawnPosition(int client, int zombieClass, int attempts, float vecPos[3]);
static cell_t L4D_GetRandomPZSpawnPosition(IPluginContext *pContext, const cell_t *params)
{
	CBaseEntity *pEntity = gamehelpers->ReferenceToEntity(params[1]); // TO DO: Study the impact of -1(nullptr), 0(CWord)
	int zombieClass = params[2];
	int attempts = params[3];
	Vector vecPos;

	if (GetRandomPZSpawnPosition(TheZombieManager, zombieClass, attempts, pEntity, &vecPos))
	{
		cell_t *vec;
		pContext->LocalToPhysAddr(params[4], &vec);
		vec[0] = sp_ftoc(vecPos.x);
		vec[1] = sp_ftoc(vecPos.y);
		vec[2] = sp_ftoc(vecPos.z);
		return 1;
	}
	return 0;
}



using fnCTerrorPlayerIsStaggering = bool (SAFETYHOOK_THISCALL *)(CBaseEntity*);
static fnCTerrorPlayerIsStaggering CTerrorPlayerIsStaggering;
// bool CTerrorPlayer::IsStaggering()
// native bool L4D_IsPlayerStaggering(int client)
static cell_t L4D_IsPlayerStaggering(IPluginContext *pContext, const cell_t *params)
{
	return CTerrorPlayerIsStaggering(gamehelpers->ReferenceToEntity(params[1]));
}



using fnMusicPlay = void (SAFETYHOOK_THISCALL *)(void*, const char*, int, float, bool, bool);
static fnMusicPlay MusicPlay;
static int g_PlayerMusic_offset;
// void Music::Play(char const*, int, float, bool, bool)
// native void L4D_PlayMusic(int client, const char[] music_str, int source_ent = 0, float one_float, bool one_bool, bool two_bool);
static cell_t L4D_PlayMusic(IPluginContext *pContext, const cell_t *params)
{
	CBaseEntity *pPlayer = gamehelpers->ReferenceToEntity(params[1]);
	void *pMusic = (char*)pPlayer + g_PlayerMusic_offset;

	char *music_str;
	pContext->LocalToString(params[2], &music_str);

	int source_ent = params[3];
	float fUnknown = sp_ctof(params[4]);
	bool bUnknown1 = params[5];
	bool bUnknown2 = params[6];
	MusicPlay(pMusic, music_str, source_ent, fUnknown, bUnknown1, bUnknown2);
	return 0;
}


using fnMusicStopPlaying = void (SAFETYHOOK_THISCALL *)(void*, const char*, float, bool);
static fnMusicStopPlaying MusicStopPlaying;
// void Music::StopPlaying(char const*, float, bool)
// native void L4D_StopMusic(int client, const char[] music_str, float one_float = 0.0, bool one_bool = false);
static cell_t L4D_StopMusic(IPluginContext *pContext, const cell_t *params)
{
	CBaseEntity *pPlayer = gamehelpers->ReferenceToEntity(params[1]);
	void *pMusic = (char*)pPlayer + g_PlayerMusic_offset;

	char *music_str;
	pContext->LocalToString(params[2], &music_str);

	float fUnknown = sp_ctof(params[3]);
	bool bUnknown1 = params[4];
	MusicStopPlaying(pMusic, music_str, fUnknown, bUnknown1);
	return 0;
}

using fnIsGenericCooperativeMode = bool (SAFETYHOOK_CCALL *)();
static fnIsGenericCooperativeMode IsGenericCooperativeMode;
// bool CTerrorGameRules::IsGenericCooperativeMode()
// native bool L4D2_IsGenericCooperativeMode();
static cell_t L4D2_IsGenericCooperativeMode(IPluginContext *pContext, const cell_t *params)
{
	return IsGenericCooperativeMode();
}


using fnCBaseEntitySetAbsOrigin = void (SAFETYHOOK_THISCALL *)(CBaseEntity*, const Vector&);
static fnCBaseEntitySetAbsOrigin CBaseEntitySetAbsOrigin;
// The hl2sdk-l4d2 has implementation, but for correctness...
// void CBaseEntity::SetAbsOrigin( const Vector& absOrigin )
// native void SetAbsOrigin(int entity, const float vec[3])
static cell_t SetAbsOrigin(IPluginContext *pContext, const cell_t *params)
{
	CBaseEntity *pEntity = gamehelpers->ReferenceToEntity(params[1]);

	cell_t *vec;
	pContext->LocalToPhysAddr(params[2], &vec);
	Vector absOrigin(sp_ctof(vec[0]), sp_ctof(vec[1]), sp_ctof(vec[2]));

	CBaseEntitySetAbsOrigin(pEntity, absOrigin);
	return 0;
}


using fnCBaseEntitySetAbsAngles = void (SAFETYHOOK_THISCALL *)(CBaseEntity*, const QAngle&);
static fnCBaseEntitySetAbsAngles CBaseEntitySetAbsAngles;
// void CBaseEntity::SetAbsAngles( const QAngle& absAngles )
// native void SetAbsAngles(int entity, const float vec[3])
static cell_t SetAbsAngles(IPluginContext *pContext, const cell_t *params)
{
	CBaseEntity *pEntity = gamehelpers->ReferenceToEntity(params[1]);

	cell_t *vec;
	pContext->LocalToPhysAddr(params[2], &vec);
	QAngle absAngles(sp_ctof(vec[0]), sp_ctof(vec[1]), sp_ctof(vec[2]));

	CBaseEntitySetAbsAngles(pEntity, absAngles);
	return 0;
}


using fnCBaseEntitySetAbsVelocity = void (SAFETYHOOK_THISCALL *)(CBaseEntity*, const Vector&);
static fnCBaseEntitySetAbsVelocity CBaseEntitySetAbsVelocity;
// void CBaseEntity::SetAbsVelocity( const Vector& vecAbsVelocity )
// native void SetAbsVelocity(int entity, const float vec[3])
static cell_t SetAbsVelocity(IPluginContext *pContext, const cell_t *params)
{
	CBaseEntity *pEntity = gamehelpers->ReferenceToEntity(params[1]);

	cell_t *vec;
	pContext->LocalToPhysAddr(params[2], &vec);
	Vector absVelocity(sp_ctof(vec[0]), sp_ctof(vec[1]), sp_ctof(vec[2]));

	CBaseEntitySetAbsVelocity(pEntity, absVelocity);
	return 0;
}


using fnSurvivorBotSetHumanSpectator = bool (SAFETYHOOK_THISCALL *)(CBaseEntity*, CBaseEntity*);
static fnSurvivorBotSetHumanSpectator SurvivorBotSetHumanSpectator;
// bool SurvivorBot::SetHumanSpectator(CTerrorPlayer *)
// native bool L4D_SetHumanSpec(int bot, int client);
static cell_t L4D_SetHumanSpec(IPluginContext *pContext, const cell_t *params)
{
	CBaseEntity *pBot = gamehelpers->ReferenceToEntity(params[1]);
	CBaseEntity *pClient = gamehelpers->ReferenceToEntity(params[2]);
	return SurvivorBotSetHumanSpectator(pBot, pClient);
}


using fnCTerrorPlayerTakeOverBot = bool (SAFETYHOOK_THISCALL *)(CBaseEntity*, bool);
static fnCTerrorPlayerTakeOverBot CTerrorPlayerTakeOverBot;
// bool CTerrorPlayer::TakeOverBot(bool bIgnoreTeam)
// native bool L4D_TakeOverBot(int client);
static cell_t L4D_TakeOverBot(IPluginContext *pContext, const cell_t *params)
{
	CBaseEntity *pEntity = gamehelpers->ReferenceToEntity(params[1]);
	return CTerrorPlayerTakeOverBot(pEntity, true);
}


using fnCCSPlayerStateTransition = void (SAFETYHOOK_THISCALL *)(CBaseEntity*, int);
static fnCCSPlayerStateTransition CCSPlayerStateTransition;
// void CCSPlayer::State_Transition(CSPlayerState)
// native void L4D_State_Transition(int client, any state);
static cell_t L4D_State_Transition(IPluginContext *pContext, const cell_t *params)
{
	CBaseEntity *pEntity = gamehelpers->ReferenceToEntity(params[1]);
	CCSPlayerStateTransition(pEntity, params[2]);
	return 0;
}



//CBaseCombatWeapon *CBaseCombatCharacter::Weapon_GetSlot( int slot );
using fnWeaponGetSlot = CBaseEntity* (SAFETYHOOK_THISCALL *)(CBaseEntity*, int);
static fnWeaponGetSlot WeaponGetSlot;
static int g_WeaponGetSlot_vtableidx;

// bool CBasePlayer::RemovePlayerItem( CBaseCombatWeapon *pItem )
using fnRemovePlayerItem = bool (SAFETYHOOK_THISCALL *)(CBaseEntity*, CBaseEntity*);
static fnRemovePlayerItem RemovePlayerItem;
static int g_RemovePlayerItem_vtableidx;

//void UTIL_Remove( CBaseEntity *oldObj );
using fnUTIL_Remove = void (SAFETYHOOK_CCALL *)(CBaseEntity*);
static fnUTIL_Remove UTIL_Remove;
static int g_customAbility_offset;

// void CTerrorPlayer::SetClass(ZombieClassType)
// native void L4D_SetClass(int client, int zombieClass);
using fnCTerrorPlayerSetClass = void (SAFETYHOOK_THISCALL *)(CBaseEntity*, int);
static fnCTerrorPlayerSetClass CTerrorPlayerSetClass;

// CBaseEntity* CBaseAbility::CreateForPlayer(CTerrorPlayer*)
// On Windows, "CTerrorPlayer*" is unused, and the "this" parameter degenerates to cdecl pushed on the stack
using fnCBaseAbilityCreateForPlayer = CBaseEntity* (SAFETYHOOK_CCALL *)(CBaseEntity*);
static fnCBaseAbilityCreateForPlayer CBaseAbilityCreateForPlayer;

static void SetClass(CBaseEntity *pClient, int zombieClass)
{
	WeaponGetSlot = GetVfuncAddress<fnWeaponGetSlot>(pClient, g_WeaponGetSlot_vtableidx);
	RemovePlayerItem = GetVfuncAddress<fnRemovePlayerItem>(pClient, g_RemovePlayerItem_vtableidx);
	
	CBaseEntity *pWeapon = WeaponGetSlot(pClient, 0);
	if (pWeapon)
	{
		RemovePlayerItem(pClient, pWeapon);
		UTIL_Remove(pWeapon);
	}
	
	CBaseHandle &hndl = *(CBaseHandle*)((char*)pClient + g_customAbility_offset);
	CBaseEntity *pAbility = gamehelpers->ReferenceToEntity(hndl.GetEntryIndex());
	UTIL_Remove(pAbility);

	CTerrorPlayerSetClass(pClient, zombieClass);

	CBaseEntity *pNewAbility = CBaseAbilityCreateForPlayer(pClient);
	hndl.Set((IHandleEntity*)pNewAbility);
	
	edict_t *pEdict = ((IServerUnknown*)pClient)->GetNetworkable()->GetEdict();
	gamehelpers->SetEdictStateChanged(pEdict, g_customAbility_offset);
}


// void CTerrorPlayer::SetClass(ZombieClassType)
// native void L4D_SetClass(int client, int zombieClass);
static cell_t L4D_SetClass(IPluginContext *pContext, const cell_t *params)
{
	CBaseEntity *pEntity = gamehelpers->ReferenceToEntity(params[1]);
	SetClass(pEntity, params[2]);
	return 0;
}


using fnCTerrorPlayerTakeOverZombieBot = void (SAFETYHOOK_THISCALL *)(CBaseEntity*, CBaseEntity*);
static fnCTerrorPlayerTakeOverZombieBot CTerrorPlayerTakeOverZombieBot;
static int g_zombieClass_offset;
// void CTerrorPlayer::TakeOverZombieBot(CTerrorPlayer*)
// native void L4D_TakeOverZombieBot(int client, int target);
static cell_t L4D_TakeOverZombieBot(IPluginContext *pContext, const cell_t *params)
{
	CBaseEntity *pClient = gamehelpers->ReferenceToEntity(params[1]);
	CBaseEntity *pTarget = gamehelpers->ReferenceToEntity(params[2]);
	uint8_t zombieClass = *(uint8_t*)((char*)pTarget + g_zombieClass_offset);

	CTerrorPlayerTakeOverZombieBot(pClient, pTarget);
	SetClass(pClient, zombieClass);
	return 0;
}

using fnCDirectorIsFirstMapInScenario = bool (SAFETYHOOK_THISCALL *)(void*);
static fnCDirectorIsFirstMapInScenario CDirectorIsFirstMapInScenario;
// bool CDirector::IsFirstMapInScenario()
// native bool L4D_IsFirstMapInScenario();
static cell_t L4D_IsFirstMapInScenario(IPluginContext *pContext, const cell_t *params)
{
	return CDirectorIsFirstMapInScenario(TheDirector);
}

using fnCTerrorGameRulesIsMissionFinalMap = bool (SAFETYHOOK_CCALL *)();
static fnCTerrorGameRulesIsMissionFinalMap CTerrorGameRulesIsMissionFinalMap;
// bool CTerrorGameRules::IsMissionFinalMap()
// native bool L4D_IsMissionFinalMap();
static cell_t L4D_IsMissionFinalMap(IPluginContext *pContext, const cell_t *params)
{
	return CTerrorGameRulesIsMissionFinalMap();
}

using fnScriptGetMaxFlowDistance = float (SAFETYHOOK_CCALL *)();
static fnScriptGetMaxFlowDistance ScriptGetMaxFlowDistance;
// Script_GetMaxFlowDistance
// native float L4D2Direct_GetMapMaxFlowDistance();
static cell_t L4D2Direct_GetMapMaxFlowDistance(IPluginContext *pContext, const cell_t *params)
{
	return sp_ftoc(ScriptGetMaxFlowDistance());
}

using fnCTerrorPlayerGetFlowDistance = float (SAFETYHOOK_THISCALL *)(CBaseEntity*, int);
static fnCTerrorPlayerGetFlowDistance CTerrorPlayerGetFlowDistance;
// float CTerrorPlayer::GetFlowDistance(TerrorNavArea::FlowType)
// native float L4D2Direct_GetFlowDistance(int client);
static cell_t L4D2Direct_GetFlowDistance(IPluginContext *pContext, const cell_t *params)
{
	float flow = CTerrorPlayerGetFlowDistance(gamehelpers->ReferenceToEntity(params[1]), 0);
	if (flow == -9999.0f)
		return sp_ftoc(0.0f);
	return sp_ftoc(flow);
}

using fnGetHighestFlowSurvivor = CBaseEntity* (SAFETYHOOK_THISCALL *)(void*, int);
static fnGetHighestFlowSurvivor GetHighestFlowSurvivor;
// CBaseEntity *CDirectorTacticalServices::GetHighestFlowSurvivor(TerrorNavArea::FlowType)
// native int L4D_GetHighestFlowSurvivor();
static cell_t L4D_GetHighestFlowSurvivor(IPluginContext *pContext, const cell_t *params)
{
	CBaseEntity *pPlayer = GetHighestFlowSurvivor(nullptr, 0); //"this" parameter is not used
	return gamehelpers->EntityToBCompatRef(pPlayer);
}


using fnGetFurthestSurvivorFlow = float (SAFETYHOOK_THISCALL *)(void*);
static fnGetFurthestSurvivorFlow GetFurthestSurvivorFlow;
// float CDirector::GetFurthestSurvivorFlow()
// native float L4D2_GetFurthestSurvivorFlow();
static cell_t L4D2_GetFurthestSurvivorFlow(IPluginContext *pContext, const cell_t *params)
{
	return sp_ftoc(GetFurthestSurvivorFlow(TheDirector));
}



// native any L4D2_GetCurrentFinaleStage();
static int *g_pCurrentFinaleStage;
static cell_t L4D2_GetCurrentFinaleStage(IPluginContext *pContext, const cell_t *params)
{
	return *g_pCurrentFinaleStage;
}




// native void L4D2Direct_SetPendingMobCount(int count);
static int *g_pPendingMobCount;
static cell_t L4D2Direct_SetPendingMobCount(IPluginContext *pContext, const cell_t *params)
{
	*g_pPendingMobCount = params[1];
	return 0;
}


using fnScriptReviveByDefibrillator = void (SAFETYHOOK_THISCALL *)(CBaseEntity*);
static fnScriptReviveByDefibrillator ScriptReviveByDefibrillator;
// void CTerrorPlayer::ScriptReviveByDefibrillator()
//native void L4D2_VScriptWrapper_ReviveByDefib(int client);
static cell_t L4D2_VScriptWrapper_ReviveByDefib(IPluginContext *pContext, const cell_t *params)
{
	CBaseEntity *pEntity = gamehelpers->ReferenceToEntity(params[1]);
	ScriptReviveByDefibrillator(pEntity);
	return 0;
}


using fnCDirectorGetScriptValueInt = int (SAFETYHOOK_THISCALL *)(void*, const char*, int);
static fnCDirectorGetScriptValueInt CDirectorGetScriptValueInt;
// int CDirector::GetScriptValue(char const*, int)
// native int L4D2_GetScriptValueInt(const char[] key, int value);
static cell_t L4D2_GetScriptValueInt(IPluginContext *pContext, const cell_t *params)
{
	char *key;
	pContext->LocalToString(params[1], &key);
	int defValue = params[2];
	return CDirectorGetScriptValueInt(TheDirector, key, defValue);
}

using fnCDirectorGetScriptValueFloat = float (SAFETYHOOK_THISCALL *)(void*, const char*, float);
static fnCDirectorGetScriptValueFloat CDirectorGetScriptValueFloat;
// float CDirector::GetScriptValue(char const*, float)
// native float L4D2_GetScriptValueFloat(const char[] key, float value);
static cell_t L4D2_GetScriptValueFloat(IPluginContext *pContext, const cell_t *params)
{
	char *key;
	pContext->LocalToString(params[1], &key);
	float defValue = sp_ctof(params[2]);
	return sp_ftoc(CDirectorGetScriptValueFloat(TheDirector, key, defValue));
}


static const sp_nativeinfo_t MyNatives[] = 
{
	{"L4D2_SpawnTank",						L4D2_SpawnTank},
	{"L4D2_SpawnWitch",						L4D2_SpawnWitch},
	{"L4D2_SpawnSpecial",					L4D2_SpawnSpecial},
	{"L4D_GetRandomPZSpawnPosition",		L4D_GetRandomPZSpawnPosition},
	{"L4D_IsPlayerStaggering",				L4D_IsPlayerStaggering},
	{"L4D_PlayMusic",						L4D_PlayMusic},
	{"L4D_StopMusic",						L4D_StopMusic},
	{"L4D2_IsGenericCooperativeMode",		L4D2_IsGenericCooperativeMode},
	{"SetAbsOrigin",						SetAbsOrigin},
	{"SetAbsAngles",						SetAbsAngles},
	{"SetAbsVelocity",						SetAbsVelocity},
	{"L4D_SetHumanSpec",					L4D_SetHumanSpec},
	{"L4D_TakeOverBot",						L4D_TakeOverBot},
	{"L4D_State_Transition",				L4D_State_Transition},
	{"L4D_SetClass",						L4D_SetClass},
	{"L4D_TakeOverZombieBot",				L4D_TakeOverZombieBot},
	{"L4D_IsFirstMapInScenario",			L4D_IsFirstMapInScenario},
	{"L4D_IsMissionFinalMap",				L4D_IsMissionFinalMap},
	{"L4D2Direct_GetMapMaxFlowDistance",	L4D2Direct_GetMapMaxFlowDistance},
	{"L4D2Direct_GetFlowDistance",			L4D2Direct_GetFlowDistance},
	{"L4D_GetHighestFlowSurvivor",			L4D_GetHighestFlowSurvivor},
	{"L4D2_GetFurthestSurvivorFlow",		L4D2_GetFurthestSurvivorFlow},
	{"L4D2_GetCurrentFinaleStage",			L4D2_GetCurrentFinaleStage},
	{"L4D2Direct_SetPendingMobCount",		L4D2Direct_SetPendingMobCount},
	{"L4D2_VScriptWrapper_ReviveByDefib",	L4D2_VScriptWrapper_ReviveByDefib},
	{"L4D2_GetScriptValueInt",				L4D2_GetScriptValueInt},
	{"L4D2_GetScriptValueFloat",			L4D2_GetScriptValueFloat},
	{NULL,	NULL},
};

bool left4dhooks::PrepNatives(IGameConfig *gamedata, char *error, size_t maxlength)
{
	const char *buffer = nullptr;

	// ---- GetAddress ----
	buffer = "TheZombieManager";
	if (!gamedata->GetAddress(buffer, (void**)&TheZombieManager)) 
	{
		snprintf(error, maxlength, "Failed to GetAddress: %s", buffer);
		return false;
	}

	buffer = "TheDirector";
	if (!gamedata->GetAddress(buffer, (void**)&TheDirector)) 
	{
		snprintf(error, maxlength, "Failed to GetAddress: %s", buffer);
		return false;
	}

	buffer = "CurrentFinaleStage";
	if (!gamedata->GetAddress(buffer, (void**)&g_pCurrentFinaleStage)) 
	{
		snprintf(error, maxlength, "Failed to GetAddress: %s", buffer);
		return false;
	}

	buffer = "PendingMobCount";
	if (!gamedata->GetAddress(buffer, (void**)&g_pPendingMobCount)) 
	{
		snprintf(error, maxlength, "Failed to GetAddress: %s", buffer);
		return false;
	}




	// ---- GetOffset ----
	buffer = "CBaseCombatCharacter::Weapon_GetSlot";
	if (!gamedata->GetOffset(buffer, &g_WeaponGetSlot_vtableidx)) 
	{
		snprintf(error, maxlength, "Failed to GetOffset: %s", buffer);
		return false;
	}

	buffer = "CBasePlayer::RemovePlayerItem";
	if (!gamedata->GetOffset(buffer, &g_RemovePlayerItem_vtableidx)) 
	{
		snprintf(error, maxlength, "Failed to GetOffset: %s", buffer);
		return false;
	}

	buffer = "m_music";
	if (!gamedata->GetOffset(buffer, &g_PlayerMusic_offset)) 
	{
		snprintf(error, maxlength, "Failed to GetOffset: %s", buffer);
		return false;
	}

	buffer = "m_customAbility";
	if (!gamedata->GetOffset(buffer, &g_customAbility_offset)) 
	{
		snprintf(error, maxlength, "Failed to GetOffset: %s", buffer);
		return false;
	}

	buffer = "m_zombieClass";
	if (!gamedata->GetOffset(buffer, &g_zombieClass_offset)) 
	{
		snprintf(error, maxlength, "Failed to GetOffset: %s", buffer);
		return false;
	}

	/*
	printf("g_WeaponGetSlot_vtableidx = %i\n", g_WeaponGetSlot_vtableidx);
	printf("g_RemovePlayerItem_vtableidx = %i\n", g_RemovePlayerItem_vtableidx);
	printf("g_PlayerMusic_offset = %i\n", g_PlayerMusic_offset);
	printf("g_customAbility_offset = %i\n", g_customAbility_offset);
	printf("g_zombieClass_offset = %i\n", g_zombieClass_offset);
	*/


	// ---- GetMemSig ----
	buffer = "ZombieManager::GetRandomPZSpawnPosition";
	GetRandomPZSpawnPosition = nullptr;
	if (!gamedata->GetMemSig(buffer, (void**)&GetRandomPZSpawnPosition) || !GetRandomPZSpawnPosition)
	{
		snprintf(error, maxlength, "Failed to GetMemSig: %s", buffer);
		return false;
	}

	buffer = "CTerrorPlayer::IsStaggering";
	CTerrorPlayerIsStaggering = nullptr;
	if (!gamedata->GetMemSig(buffer, (void**)&CTerrorPlayerIsStaggering) || !CTerrorPlayerIsStaggering)
	{
		snprintf(error, maxlength, "Failed to GetMemSig: %s", buffer);
		return false;
	}

	buffer = "Music::Play";
	MusicPlay = nullptr;
	if (!gamedata->GetMemSig(buffer, (void**)&MusicPlay) || !MusicPlay)
	{
		snprintf(error, maxlength, "Failed to GetMemSig: %s", buffer);
		return false;
	}

	buffer = "Music::StopPlaying";
	MusicStopPlaying = nullptr;
	if (!gamedata->GetMemSig(buffer, (void**)&MusicStopPlaying) || !MusicStopPlaying)
	{
		snprintf(error, maxlength, "Failed to GetMemSig: %s", buffer);
		return false;
	}

	buffer = "CTerrorGameRules::IsGenericCooperativeMode";
	IsGenericCooperativeMode = nullptr;
	if (!gamedata->GetMemSig(buffer, (void**)&IsGenericCooperativeMode) || !IsGenericCooperativeMode)
	{
		snprintf(error, maxlength, "Failed to GetMemSig: %s", buffer);
		return false;
	}

	buffer = "CBaseEntity::SetAbsOrigin";
	CBaseEntitySetAbsOrigin = nullptr;
	if (!gamedata->GetMemSig(buffer, (void**)&CBaseEntitySetAbsOrigin) || !CBaseEntitySetAbsOrigin)
	{
		snprintf(error, maxlength, "Failed to GetMemSig: %s", buffer);
		return false;
	}

	buffer = "CBaseEntity::SetAbsAngles";
	CBaseEntitySetAbsAngles = nullptr;
	if (!gamedata->GetMemSig(buffer, (void**)&CBaseEntitySetAbsAngles) || !CBaseEntitySetAbsAngles)
	{
		snprintf(error, maxlength, "Failed to GetMemSig: %s", buffer);
		return false;
	}

	buffer = "CBaseEntity::SetAbsVelocity";
	CBaseEntitySetAbsVelocity = nullptr;
	if (!gamedata->GetMemSig(buffer, (void**)&CBaseEntitySetAbsVelocity) || !CBaseEntitySetAbsVelocity)
	{
		snprintf(error, maxlength, "Failed to GetMemSig: %s", buffer);
		return false;
	}

	buffer = "CTerrorPlayer::TakeOverBot";
	CTerrorPlayerTakeOverBot = nullptr;
	if (!gamedata->GetMemSig(buffer, (void**)&CTerrorPlayerTakeOverBot) || !CTerrorPlayerTakeOverBot)
	{
		snprintf(error, maxlength, "Failed to GetMemSig: %s", buffer);
		return false;
	}

	buffer = "SurvivorBot::SetHumanSpectator";
	SurvivorBotSetHumanSpectator = nullptr;
	if (!gamedata->GetMemSig(buffer, (void**)&SurvivorBotSetHumanSpectator) || !SurvivorBotSetHumanSpectator)
	{
		snprintf(error, maxlength, "Failed to GetMemSig: %s", buffer);
		return false;
	}

	buffer = "CCSPlayer::State_Transition";
	CCSPlayerStateTransition = nullptr;
	if (!gamedata->GetMemSig(buffer, (void**)&CCSPlayerStateTransition) || !CCSPlayerStateTransition)
	{
		snprintf(error, maxlength, "Failed to GetMemSig: %s", buffer);
		return false;
	}

	buffer = "UTIL_Remove";
	UTIL_Remove = nullptr;
	if (!gamedata->GetMemSig(buffer, (void**)&UTIL_Remove) || !UTIL_Remove)
	{
		snprintf(error, maxlength, "Failed to GetMemSig: %s", buffer);
		return false;
	}

	buffer = "CTerrorPlayer::SetClass";
	CTerrorPlayerSetClass = nullptr;
	if (!gamedata->GetMemSig(buffer, (void**)&CTerrorPlayerSetClass) || !CTerrorPlayerSetClass)
	{
		snprintf(error, maxlength, "Failed to GetMemSig: %s", buffer);
		return false;
	}

	buffer = "CBaseAbility::CreateForPlayer";
	CBaseAbilityCreateForPlayer = nullptr;
	if (!gamedata->GetMemSig(buffer, (void**)&CBaseAbilityCreateForPlayer) || !CBaseAbilityCreateForPlayer) 
	{
		snprintf(error, maxlength, "Failed to GetMemSig: %s", buffer);
		return false;
	}

	buffer = "CTerrorPlayer::TakeOverZombieBot";
	CTerrorPlayerTakeOverZombieBot = nullptr;
	if (!gamedata->GetMemSig(buffer, (void**)&CTerrorPlayerTakeOverZombieBot) || !CTerrorPlayerTakeOverZombieBot)
	{
		snprintf(error, maxlength, "Failed to GetMemSig: %s", buffer);
		return false;
	}

	buffer = "CDirector::IsFirstMapInScenario";
	CDirectorIsFirstMapInScenario = nullptr;
	if (!gamedata->GetMemSig(buffer, (void**)&CDirectorIsFirstMapInScenario) || !CDirectorIsFirstMapInScenario)
	{
		snprintf(error, maxlength, "Failed to GetMemSig: %s", buffer);
		return false;
	}

	buffer = "CTerrorGameRules::IsMissionFinalMap";
	CTerrorGameRulesIsMissionFinalMap = nullptr;
	if (!gamedata->GetMemSig(buffer, (void**)&CTerrorGameRulesIsMissionFinalMap) || !CTerrorGameRulesIsMissionFinalMap)
	{
		snprintf(error, maxlength, "Failed to GetMemSig: %s", buffer);
		return false;
	}

	buffer = "Script_GetMaxFlowDistance";
	ScriptGetMaxFlowDistance = nullptr;
	if (!gamedata->GetMemSig(buffer, (void**)&ScriptGetMaxFlowDistance) || !ScriptGetMaxFlowDistance)
	{
		snprintf(error, maxlength, "Failed to GetMemSig: %s", buffer);
		return false;
	}

	buffer = "CTerrorPlayer::GetFlowDistance";
	CTerrorPlayerGetFlowDistance = nullptr;
	if (!gamedata->GetMemSig(buffer, (void**)&CTerrorPlayerGetFlowDistance) || !CTerrorPlayerGetFlowDistance)
	{
		snprintf(error, maxlength, "Failed to GetMemSig: %s", buffer);
		return false;
	}

	buffer = "CDirectorTacticalServices::GetHighestFlowSurvivor";
	GetHighestFlowSurvivor = nullptr;
	if (!gamedata->GetMemSig(buffer, (void**)&GetHighestFlowSurvivor) || !GetHighestFlowSurvivor)
	{
		snprintf(error, maxlength, "Failed to GetMemSig: %s", buffer);
		return false;
	}

	buffer = "CDirector::GetFurthestSurvivorFlow";
	GetFurthestSurvivorFlow = nullptr;
	if (!gamedata->GetMemSig(buffer, (void**)&GetFurthestSurvivorFlow) || !GetFurthestSurvivorFlow)
	{
		snprintf(error, maxlength, "Failed to GetMemSig: %s", buffer);
		return false;
	}

	buffer = "CTerrorPlayer::ScriptReviveByDefibrillator";
	ScriptReviveByDefibrillator = nullptr;
	if (!gamedata->GetMemSig(buffer, (void**)&ScriptReviveByDefibrillator) || !ScriptReviveByDefibrillator)
	{
		snprintf(error, maxlength, "Failed to GetMemSig: %s", buffer);
		return false;
	}

	buffer = "CDirector::GetScriptValueInt";
	CDirectorGetScriptValueInt = nullptr;
	if (!gamedata->GetMemSig(buffer, (void**)&CDirectorGetScriptValueInt) || !CDirectorGetScriptValueInt)
	{
		snprintf(error, maxlength, "Failed to GetMemSig: %s", buffer);
		return false;
	}

	buffer = "CDirector::GetScriptValueFloat";
	CDirectorGetScriptValueFloat = nullptr;
	if (!gamedata->GetMemSig(buffer, (void**)&CDirectorGetScriptValueFloat) || !CDirectorGetScriptValueFloat)
	{
		snprintf(error, maxlength, "Failed to GetMemSig: %s", buffer);
		return false;
	}

	sharesys->AddNatives(myself, MyNatives);
	return true;
}

void left4dhooks::ClearNatives()
{
	
}

