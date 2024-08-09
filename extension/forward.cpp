#include "IForwardSys.h"
#include "extension.h"
#include "safetyhook.hpp"
#include "sp_typeutil.h"
#include "vector.h"

static SafetyHookInline g_hook_SpawnTank{};
static SafetyHookInline g_hook_SpawnWitch{};
static SafetyHookInline g_hook_SpawnSpecial{};
static SafetyHookInline g_hook_SpawnMob{};
static SafetyHookInline g_hook_SelectWeightedSequence{};
static SafetyHookInline g_hook_CTankRock_OnRelease{};
static SafetyHookInline g_hook_ChooseVictim{};
static SafetyHookInline g_hook_ChangeFinaleStage_Pre{};

static IForward *g_fwd_OnSpawnTank_Pre;
static IForward *g_fwd_OnSpawnTank_Post;
static IForward *g_fwd_OnSpawnWitch_Pre;
static IForward *g_fwd_OnSpawnWitch_Post;
static IForward *g_fwd_OnSpawnSpecial_Pre;
static IForward *g_fwd_OnSpawnSpecial_Post;
static IForward *g_fwd_OnSpawnMob_Pre;
static IForward *g_fwd_OnSelectWeightedSequence_Post;
static IForward *g_fwd_CTankRock_OnRelease_Pre;
static IForward *g_fwd_OnChooseVictim_Post;
static IForward *g_fwd_OnChangeFinaleStage_Pre;


class ZombieManagerExt
{
public:
	CBaseEntity *SpawnTank(const Vector &vecPos, const QAngle &vecAng);
	CBaseEntity *SpawnWitch(const Vector &vecPos, const QAngle &vecAng);
	CBaseEntity *SpawnSpecial(int zombieClass, const Vector &vecPos, const QAngle &vecAng);
	void SpawnMob(int amount);
};


class CTerrorPlayerExt
{
public:
	int SelectWeightedSequence(int activity);
};


class CTankRockExt
{
public:
	void OnRelease(Vector &vecPos, QAngle &vecAng, Vector &vecVel, Vector &vecRot);
};

class BossZombiePlayerBotExt
{
public:
	CBaseEntity *ChooseVictim(CBaseEntity *pOldTarget, int flags, CBaseEntity *pSurvivorScan);
};


class CDirectorScriptedEventManagerExt
{
public:
	void ChangeFinaleStage(int stage, char const *str);
};


template <typename R, typename T, typename... Args>
inline void *GetMemberFuncAddr(R (T::*memberFunc)(Args...))
{
	return *(void **)&memberFunc;
}


CBaseEntity *ZombieManagerExt::SpawnTank(const Vector &vecPos, const QAngle &vecAng)
{
	cell_t vec[3] = {sp_ftoc(vecPos.x), sp_ftoc(vecPos.y), sp_ftoc(vecPos.z)};
	cell_t ang[3] = {sp_ftoc(vecAng.x), sp_ftoc(vecAng.y), sp_ftoc(vecAng.z)};

	cell_t result = Pl_Continue;
	g_fwd_OnSpawnTank_Pre->PushArray(vec, 3);
	g_fwd_OnSpawnTank_Pre->PushArray(ang, 3);
	g_fwd_OnSpawnTank_Pre->Execute(&result);

	if (result == Pl_Handled)
		return nullptr;
	
	CBaseEntity *pEntity = g_hook_SpawnTank.thiscall<CBaseEntity*>(this, &vecPos, &vecAng);
	int entity = gamehelpers->EntityToBCompatRef(pEntity);
	
	g_fwd_OnSpawnTank_Post->PushCell(entity);
	g_fwd_OnSpawnTank_Post->PushArray(vec, 3);
	g_fwd_OnSpawnTank_Post->PushArray(ang, 3);
	g_fwd_OnSpawnTank_Post->Execute();
	return pEntity;
}

CBaseEntity *ZombieManagerExt::SpawnWitch(const Vector &vecPos, const QAngle &vecAng)
{
	cell_t vec[3] = {sp_ftoc(vecPos.x), sp_ftoc(vecPos.y), sp_ftoc(vecPos.z)};
	cell_t ang[3] = {sp_ftoc(vecAng.x), sp_ftoc(vecAng.y), sp_ftoc(vecAng.z)};

	cell_t result = Pl_Continue;
	g_fwd_OnSpawnWitch_Pre->PushArray(vec, 3);
	g_fwd_OnSpawnWitch_Pre->PushArray(ang, 3);
	g_fwd_OnSpawnWitch_Pre->Execute(&result);

	if (result == Pl_Handled)
		return nullptr;
	
	CBaseEntity *pEntity = g_hook_SpawnWitch.thiscall<CBaseEntity*>(this, &vecPos, &vecAng);
	int entity = gamehelpers->EntityToBCompatRef(pEntity);
	
	g_fwd_OnSpawnWitch_Post->PushCell(entity);
	g_fwd_OnSpawnWitch_Post->PushArray(vec, 3);
	g_fwd_OnSpawnWitch_Post->PushArray(ang, 3);
	g_fwd_OnSpawnWitch_Post->Execute();
	return pEntity;
}

CBaseEntity *ZombieManagerExt::SpawnSpecial(int zombieClass, const Vector &vecPos, const QAngle &vecAng)
{
	cell_t vec[3] = {sp_ftoc(vecPos.x), sp_ftoc(vecPos.y), sp_ftoc(vecPos.z)};
	cell_t ang[3] = {sp_ftoc(vecAng.x), sp_ftoc(vecAng.y), sp_ftoc(vecAng.z)};

	cell_t result = Pl_Continue;
	int new_zombieClass = zombieClass;
	g_fwd_OnSpawnSpecial_Pre->PushCellByRef(&new_zombieClass);
	g_fwd_OnSpawnSpecial_Pre->PushArray(vec, 3);
	g_fwd_OnSpawnSpecial_Pre->PushArray(ang, 3);
	g_fwd_OnSpawnSpecial_Pre->Execute(&result);

	if (result == Pl_Handled)
		return nullptr;

	if (result == Pl_Changed)
		zombieClass = new_zombieClass;
	
	CBaseEntity *pEntity = g_hook_SpawnSpecial.thiscall<CBaseEntity*>(this, zombieClass, &vecPos, &vecAng);
	int entity = gamehelpers->EntityToBCompatRef(pEntity);
	
	g_fwd_OnSpawnSpecial_Post->PushCell(entity);
	g_fwd_OnSpawnSpecial_Post->PushCell(zombieClass);
	g_fwd_OnSpawnSpecial_Post->PushArray(vec, 3);
	g_fwd_OnSpawnSpecial_Post->PushArray(ang, 3);
	g_fwd_OnSpawnSpecial_Post->Execute();
	return pEntity;
}

void ZombieManagerExt::SpawnMob(int amount)
{
	cell_t result = Pl_Continue;
	int new_amount = amount;
	g_fwd_OnSpawnMob_Pre->PushCellByRef(&new_amount);
	g_fwd_OnSpawnMob_Pre->Execute(&result);

	if (result == Pl_Handled)
		return;

	if (result == Pl_Changed)
		amount = new_amount;
	
	g_hook_SpawnMob.thiscall<void>(this, amount);
}


// int CTerrorPlayer::SelectWeightedSequence(Activity activity)
// forward Action L4D2_OnSelectWeightedSequence_Post(int client, int &sequence);
int CTerrorPlayerExt::SelectWeightedSequence(int activity)
{
	int sequence = g_hook_SelectWeightedSequence.thiscall<int>(this, activity);
	int client = gamehelpers->EntityToBCompatRef((CBaseEntity*)this);

	int new_sequence = sequence;
	cell_t result = Pl_Continue;
	g_fwd_OnSelectWeightedSequence_Post->PushCell(client);
	g_fwd_OnSelectWeightedSequence_Post->PushCellByRef(&new_sequence);
	g_fwd_OnSelectWeightedSequence_Post->Execute(&result);

	if (result != Pl_Continue)
		return new_sequence;
	return sequence;
}

static int g_hThrower_offset;
// void CTankRock::OnRelease(Vector const&, QAngle const&, Vector const&, Vector const&)
// forward Action L4D_TankRock_OnRelease(int tank, int rock, float vecPos[3], float vecAng[3], float vecVel[3], float vecRot[3]);
void CTankRockExt::OnRelease(Vector &vecPos, QAngle &vecAng, Vector &vecVel, Vector &vecRot)
{
	edict_t *pThrower = gamehelpers->GetHandleEntity(*(CBaseHandle*)((char*)this + g_hThrower_offset));
	assert(pThrower);

	int tank = gamehelpers->IndexOfEdict(pThrower);
	int rock = gamehelpers->EntityToBCompatRef((CBaseEntity*)this);

	cell_t pos[3] = {sp_ftoc(vecPos.x), sp_ftoc(vecPos.y), sp_ftoc(vecPos.z)};
	cell_t ang[3] = {sp_ftoc(vecAng.x), sp_ftoc(vecAng.y), sp_ftoc(vecAng.z)};
	cell_t vel[3] = {sp_ftoc(vecVel.x), sp_ftoc(vecVel.y), sp_ftoc(vecVel.z)};
	cell_t rot[3] = {sp_ftoc(vecRot.x), sp_ftoc(vecRot.y), sp_ftoc(vecRot.z)};
	
	cell_t result = Pl_Continue;
	g_fwd_CTankRock_OnRelease_Pre->PushCell(tank);
	g_fwd_CTankRock_OnRelease_Pre->PushCell(rock);
	g_fwd_CTankRock_OnRelease_Pre->PushArray(pos, 3, SM_PARAM_COPYBACK);
	g_fwd_CTankRock_OnRelease_Pre->PushArray(ang, 3, SM_PARAM_COPYBACK);
	g_fwd_CTankRock_OnRelease_Pre->PushArray(vel, 3, SM_PARAM_COPYBACK);
	g_fwd_CTankRock_OnRelease_Pre->PushArray(rot, 3, SM_PARAM_COPYBACK);
	g_fwd_CTankRock_OnRelease_Pre->Execute(&result);

	if (result == Pl_Changed)
	{
		vecPos.Init(sp_ctof(pos[0]), sp_ctof(pos[1]), sp_ctof(pos[2]));
		vecAng.Init(sp_ctof(ang[0]), sp_ctof(ang[1]), sp_ctof(ang[2]));
		vecVel.Init(sp_ctof(vel[0]), sp_ctof(vel[1]), sp_ctof(vel[2]));
		vecRot.Init(sp_ctof(rot[0]), sp_ctof(rot[1]), sp_ctof(rot[2]));
	}
	
	g_hook_CTankRock_OnRelease.thiscall<void>(this, &vecPos, &vecAng, &vecVel, &vecRot);
}

// CTerrorPlayer* BossZombiePlayerBot::ChooseVictim(CTerrorPlayer*, int, CBaseCombatCharacter*)
// forward Action L4D2_OnChooseVictim(int specialInfected, int &curTarget);
CBaseEntity *BossZombiePlayerBotExt::ChooseVictim(CBaseEntity *pOldTarget, int flags, CBaseEntity *pSurvivorScan)
{
	CBaseEntity *pTarget = g_hook_ChooseVictim.thiscall<CBaseEntity*>(this, pOldTarget, flags, pSurvivorScan);
	int specialInfected = gamehelpers->EntityToBCompatRef((CBaseEntity*)this);
	int curTarget = gamehelpers->EntityToBCompatRef(pTarget);

	cell_t result = Pl_Continue;
	g_fwd_OnChooseVictim_Post->PushCell(specialInfected);
	g_fwd_OnChooseVictim_Post->PushCellByRef(&curTarget);
	g_fwd_OnChooseVictim_Post->Execute(&result);

	if (result == Pl_Handled)
		return (CBaseEntity*)this;

	if (result == Pl_Changed)
		return gamehelpers->ReferenceToEntity(curTarget);

	return pTarget;
}

// void CDirectorScriptedEventManager::ChangeFinaleStage(ScriptedEventStage, char const*)
// forward Action L4D2_OnChangeFinaleStage(int &finaleType, const char[] arg);
void CDirectorScriptedEventManagerExt::ChangeFinaleStage(int stage, char const *str)
{
	cell_t result = Pl_Continue;
	int new_stage = stage;
	g_fwd_OnChangeFinaleStage_Pre->PushCellByRef(&new_stage);
	g_fwd_OnChangeFinaleStage_Pre->PushString(str);
	g_fwd_OnChangeFinaleStage_Pre->Execute(&result);

	if (result == Pl_Handled)
		return;

	if (result == Pl_Changed)
		stage = new_stage;

	g_hook_ChangeFinaleStage_Pre.thiscall<void>(this, stage, str);
}


bool left4dhooks::PrepForward(IGameConfig *gamedata, char *error, size_t maxlength)
{
	// CBaseEntity* ZombieManager::SpawnTank(Vector const&, QAngle const&)
	// forward Action L4D_OnSpawnTank(const float vecPos[3], const float vecAng[3]);
	// forward void L4D_OnSpawnTank_Post(int client, const float vecPos[3], const float vecAng[3]);
	g_fwd_OnSpawnTank_Pre = forwards->CreateForward("L4D_OnSpawnTank", ET_Event, 2, nullptr, Param_Array, Param_Array);
	g_fwd_OnSpawnTank_Post = forwards->CreateForward("L4D_OnSpawnTank_Post", ET_Ignore, 3, nullptr, Param_Cell, Param_Array, Param_Array);

	// CBaseEntity* ZombieManager::SpawnWitch(Vector const&, QAngle const&)
	// forward Action L4D_OnSpawnWitch(const float vecPos[3], const float vecAng[3]);
	// forward void L4D_OnSpawnWitch_Post(int entity, const float vecPos[3], const float vecAng[3]);
	g_fwd_OnSpawnWitch_Pre = forwards->CreateForward("L4D_OnSpawnWitch", ET_Event, 2, nullptr, Param_Array, Param_Array);
	g_fwd_OnSpawnWitch_Post = forwards->CreateForward("L4D_OnSpawnWitch_Post", ET_Ignore, 3, nullptr, Param_Cell, Param_Array, Param_Array);


	// CBaseEntity* ZombieManager::SpawnSpecial(ZombieClassType, Vector const&, QAngle const&)
	// forward Action L4D_OnSpawnSpecial(int &zombieClass, const float vecPos[3], const float vecAng[3]);
	// forward void L4D_OnSpawnSpecial_Post(int client, int zombieClass, const float vecPos[3], const float vecAng[3]);
	g_fwd_OnSpawnSpecial_Pre = forwards->CreateForward("L4D_OnSpawnSpecial", ET_Event, 3, nullptr, Param_CellByRef, Param_Array, Param_Array);
	g_fwd_OnSpawnSpecial_Post = forwards->CreateForward("L4D_OnSpawnSpecial_Post", ET_Ignore, 4, nullptr, Param_Cell, Param_Cell, Param_Array, Param_Array);

	// void ZombieManager::SpawnMob(int)
	// forward Action L4D_OnSpawnMob(int &amount);
	g_fwd_OnSpawnMob_Pre = forwards->CreateForward("L4D_OnSpawnMob", ET_Event, 1, nullptr, Param_CellByRef);

	// int CTerrorPlayer::SelectWeightedSequence(Activity activity)
	// forward Action L4D2_OnSelectWeightedSequence_Post(int client, int &sequence);
	g_fwd_OnSelectWeightedSequence_Post = forwards->CreateForward("L4D2_OnSelectWeightedSequence_Post", ET_Event, 2, nullptr, Param_Cell, Param_CellByRef);

	// void CTankRock::OnRelease(Vector const&, QAngle const&, Vector const&, Vector const&)
	// forward Action L4D_TankRock_OnRelease(int tank, int rock, float vecPos[3], float vecAng[3], float vecVel[3], float vecRot[3]);
	g_fwd_CTankRock_OnRelease_Pre = forwards->CreateForward("L4D_TankRock_OnRelease", ET_Event, 6, nullptr, Param_Cell, Param_Cell, Param_Array, Param_Array, Param_Array, Param_Array);

	// CTerrorPlayer* BossZombiePlayerBot::ChooseVictim(CTerrorPlayer*, int, CBaseCombatCharacter*)
	// forward Action L4D2_OnChooseVictim(int specialInfected, int &curTarget);
	g_fwd_OnChooseVictim_Post = forwards->CreateForward("L4D2_OnChooseVictim", ET_Event, 2, nullptr, Param_Cell, Param_CellByRef);

	// void CDirectorScriptedEventManager::ChangeFinaleStage(ScriptedEventStage, char const*)
	// forward Action L4D2_OnChangeFinaleStage(int &finaleType, const char[] arg);
	g_fwd_OnChangeFinaleStage_Pre = forwards->CreateForward("L4D2_OnChangeFinaleStage", ET_Event, 2, nullptr, Param_CellByRef, Param_String);


	const char *buffer = nullptr;
	void *addr = nullptr;

	buffer = "m_hThrower";
	if (!gamedata->GetOffset(buffer, &g_hThrower_offset)) 
	{
		snprintf(error, maxlength, "Failed to GetOffset: %s", buffer);
		return false;
	}

	buffer = "ZombieManager::SpawnTank";
	addr = nullptr;
	if (!gamedata->GetMemSig(buffer, &addr) || !addr) 
	{
		snprintf(error, maxlength, "Failed to GetMemSig: %s", buffer);
		return false;
	}
	g_hook_SpawnTank = safetyhook::create_inline(addr, GetMemberFuncAddr(&ZombieManagerExt::SpawnTank));
	if (!g_hook_SpawnTank.enabled())
	{
		snprintf(error, maxlength, "Failed to create_inline: %s", buffer);
		return false;
	}

	buffer = "ZombieManager::SpawnWitch";
	addr = nullptr;
	if (!gamedata->GetMemSig(buffer, &addr) || !addr) 
	{
		snprintf(error, maxlength, "Failed to GetMemSig: %s", buffer);
		return false;
	}
	g_hook_SpawnWitch = safetyhook::create_inline(addr, GetMemberFuncAddr(&ZombieManagerExt::SpawnWitch));
	if (!g_hook_SpawnWitch.enabled())
	{
		snprintf(error, maxlength, "Failed to create_inline: %s", buffer);
		return false;
	}

	buffer = "ZombieManager::SpawnSpecial";
	addr = nullptr;
	if (!gamedata->GetMemSig(buffer, &addr) || !addr) 
	{
		snprintf(error, maxlength, "Failed to GetMemSig: %s", buffer);
		return false;
	}
	g_hook_SpawnSpecial = safetyhook::create_inline(addr, GetMemberFuncAddr(&ZombieManagerExt::SpawnSpecial));
	if (!g_hook_SpawnSpecial.enabled())
	{
		snprintf(error, maxlength, "Failed to create_inline: %s", buffer);
		return false;
	}

	buffer = "ZombieManager::SpawnMob";
	addr = nullptr;
	if (!gamedata->GetMemSig(buffer, &addr) || !addr) 
	{
		snprintf(error, maxlength, "Failed to GetMemSig: %s", buffer);
		return false;
	}
	g_hook_SpawnMob = safetyhook::create_inline(addr, GetMemberFuncAddr(&ZombieManagerExt::SpawnMob));
	if (!g_hook_SpawnMob.enabled())
	{
		snprintf(error, maxlength, "Failed to create_inline: %s", buffer);
		return false;
	}

	buffer = "CTerrorPlayer::SelectWeightedSequence";
	addr = nullptr;
	if (!gamedata->GetMemSig(buffer, &addr) || !addr)  
	{
		snprintf(error, maxlength, "Failed to GetMemSig: %s", buffer);
		return false;
	}
	g_hook_SelectWeightedSequence = safetyhook::create_inline(addr, GetMemberFuncAddr(&CTerrorPlayerExt::SelectWeightedSequence));
	if (!g_hook_SelectWeightedSequence.enabled())
	{
		snprintf(error, maxlength, "Failed to create_inline: %s", buffer);
		return false;
	}

	buffer = "CTankRock::OnRelease";
	addr = nullptr;
	if (!gamedata->GetMemSig(buffer, &addr) || !addr) 
	{
		snprintf(error, maxlength, "Failed to GetMemSig: %s", buffer);
		return false;
	}
	g_hook_CTankRock_OnRelease = safetyhook::create_inline(addr, GetMemberFuncAddr(&CTankRockExt::OnRelease));
	if (!g_hook_CTankRock_OnRelease.enabled())
	{
		snprintf(error, maxlength, "Failed to create_inline: %s", buffer);
		return false;
	}

	buffer = "BossZombiePlayerBot::ChooseVictim";
	addr = nullptr;
	if (!gamedata->GetMemSig(buffer, &addr) || !addr) 
	{
		snprintf(error, maxlength, "Failed to GetMemSig: %s", buffer);
		return false;
	}
	g_hook_ChooseVictim = safetyhook::create_inline(addr, GetMemberFuncAddr(&BossZombiePlayerBotExt::ChooseVictim));
	if (!g_hook_ChooseVictim.enabled())
	{
		snprintf(error, maxlength, "Failed to create_inline: %s", buffer);
		return false;
	}

	buffer = "CDirectorScriptedEventManager::ChangeFinaleStage";
	addr = nullptr;
	if (!gamedata->GetMemSig(buffer, &addr) || !addr) 
	{
		snprintf(error, maxlength, "Failed to GetMemSig: %s", buffer);
		return false;
	}
	g_hook_ChangeFinaleStage_Pre = safetyhook::create_inline(addr, GetMemberFuncAddr(&CDirectorScriptedEventManagerExt::ChangeFinaleStage));
	if (!g_hook_ChangeFinaleStage_Pre.enabled())
	{
		snprintf(error, maxlength, "Failed to create_inline: %s", buffer);
		return false;
	}

	return true;
}

void left4dhooks::ClearForward()
{
	g_hook_SpawnTank = {};
	g_hook_SpawnWitch = {};
	g_hook_SpawnSpecial = {};
	g_hook_SpawnMob = {};
	g_hook_SelectWeightedSequence = {};
	g_hook_CTankRock_OnRelease = {};
	g_hook_ChooseVictim = {};
	g_hook_ChangeFinaleStage_Pre = {};

	forwards->ReleaseForward(g_fwd_OnSpawnTank_Pre);
	forwards->ReleaseForward(g_fwd_OnSpawnTank_Post);
	forwards->ReleaseForward(g_fwd_OnSpawnWitch_Pre);
	forwards->ReleaseForward(g_fwd_OnSpawnWitch_Post);
	forwards->ReleaseForward(g_fwd_OnSpawnSpecial_Pre);
	forwards->ReleaseForward(g_fwd_OnSpawnSpecial_Post);
	forwards->ReleaseForward(g_fwd_OnSpawnMob_Pre);
	forwards->ReleaseForward(g_fwd_OnSelectWeightedSequence_Post);
	forwards->ReleaseForward(g_fwd_CTankRock_OnRelease_Pre);
	forwards->ReleaseForward(g_fwd_OnChooseVictim_Post);
	forwards->ReleaseForward(g_fwd_OnChangeFinaleStage_Pre);
}