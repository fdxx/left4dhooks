#if defined _left4dhooks_ext_included
 #endinput
#endif
#define _left4dhooks_ext_included

#include <sdktools>
#include <sdkhooks>

// Provided by "BHaType":
// For the "L4D_State_Transition" native.
// X -> Y (means X state will become Y state on next frame or some seconds later)
enum
{
	STATE_ACTIVE = 0,
	STATE_WELCOME,				// -> STATE_PICKING_TEAM
	STATE_PICKING_TEAM,
	STATE_PICKINGCLASS,			// -> STATE_ACTIVE
	STATE_DEATH_ANIM,			// -> STATE_DEATH_WAIT_FOR_KEY
	STATE_DEATH_WAIT_FOR_KEY,	// -> STATE_OBSERVER_MODE
	STATE_OBSERVER_MODE,
	STATE_WAITING_FOR_RESCUE,
	STATE_GHOST,
	STATE_INTRO_CAMERA
}


/**
 * @brief Called whenever ZombieManager::SpawnTank(Vector&,QAngle&) is invoked
 * @remarks Not invoked if z_spawn tank is used and it gives a ghosted/dead player tank
 *
 * @param vecPos		Vector coordinate where tank is spawned
 * @param vecAng		QAngle where tank will be facing
 *
 * @return				Plugin_Handled to block tank from spawning, Plugin_Continue otherwise.
 */
forward Action L4D_OnSpawnTank(const float vecPos[3], const float vecAng[3]);

/**
 * @brief Called whenever ZombieManager::SpawnTank(Vector&,QAngle&) is invoked
 * @remarks Not invoked if z_spawn tank is used and it gives a ghosted/dead player tank
 * @remarks This forward will not trigger if the relative pre-hook forward has been blocked with Plugin_Handled
 *
 * @param client		The client index who spawned (can be 0 if blocked in pre hook)
 * @param vecPos		Vector coordinate where tank is spawned
 * @param vecAng		QAngle where tank will be facing
 *
 * @noreturn
 */
forward void L4D_OnSpawnTank_Post(int client, const float vecPos[3], const float vecAng[3]);

/**
 * @brief Called whenever ZombieManager::SpawnWitch(Vector&,QAngle&) is invoked
 * @brief Called when a Witch spawns
 *
 * @param vecPos		Vector coordinate where witch is spawned
 * @param vecAng		QAngle where witch will be facing
 *
 * @return 				Plugin_Handled to block witch from spawning, Plugin_Continue otherwise.
 */
forward Action L4D_OnSpawnWitch(const float vecPos[3], const float vecAng[3]);

/**
 * @brief Called whenever ZombieManager::SpawnWitch(Vector&,QAngle&) is invoked
 * @brief Called when a Witch spawns
 * @remarks This forward will not trigger if the relative pre-hook forward has been blocked with Plugin_Handled
 *
 * @param entity		Entity index that spawned
 * @param vecPos		Vector coordinate where witch is spawned
 * @param vecAng		QAngle where witch will be facing
 *
 * @noreturn
 */
forward void L4D_OnSpawnWitch_Post(int entity, const float vecPos[3], const float vecAng[3]);

/**
 * @brief Called whenever ZombieManager::SpawnSpecial(ZombieClassType,Vector&,QAngle&) is invoked
 * @remarks Only used for bot special spawns (not players)
 * @remarks zombieClass: 1=Smoker, 2=Boomer, 3=Hunter, 4=Spitter, 5=Jockey, 6=Charger
 *
 * @param zombieClass	Zombie class that will be spawned
 * @param vecPos		Vector coordinate where special will be spawned
 * @param vecAng		QAngle where special will be facing
 *
 * @return				Plugin_Handled to block special from spawning,
 * 						Plugin_Changed to change the zombie class type to spawn, Plugin_Continue otherwise.
 */
forward Action L4D_OnSpawnSpecial(int &zombieClass, const float vecPos[3], const float vecAng[3]);

/**
 * @brief Called whenever ZombieManager::SpawnSpecial(ZombieClassType,Vector&,QAngle&) is invoked
 * @remarks Only used for bot special spawns (not players)
 * @remarks zombieClass: 1=Smoker, 2=Boomer, 3=Hunter, 4=Spitter, 5=Jockey, 6=Charger
 * @remarks This forward will not trigger if the relative pre-hook forward has been blocked with Plugin_Handled
 *
 * @param client		The client index who spawned. Can be 0 if spawning failed
 * @param zombieClass	Zombie class that will be spawned
 * @param vecPos		Vector coordinate where special will be spawned
 * @param vecAng		QAngle where special will be facing
 *
 * @noreturn
 */
forward void L4D_OnSpawnSpecial_Post(int client, int zombieClass, const float vecPos[3], const float vecAng[3]);


/**
 * @brief Called whenever ZombieManager::SpawnMob(int) is invoked
 * @remarks called on natural hordes & z_spawn mob, increases Zombie Spawn
 *			Queue, triggers player OnMobSpawned (vocalizations), sets horde
 *			direction, and plays horde music.
 *
 * @param amount		Amount of Zombies to add to Queue
 *
 * @return				Plugin_Handled to block, Plugin_Changed to use overwritten values from plugin, Plugin_Continue otherwise
 */
forward Action L4D_OnSpawnMob(int &amount);

/**
 * @brief Called when CTerrorPlayer::SelectWeightedSequence(Activity activity)
 *
 * @param client	client index
 * @param sequence	current selected activity for attack, option to override the return value with it
 *
 * @return			Plugin_Handled to override return value, Plugin_Continue otherwise.
 */
forward Action L4D2_OnSelectWeightedSequence_Post(int client, int &sequence); // customize. 

/**
 * @brief Called whenever CTankRock::OnRelease(Vector const&, QAngle const&, Vector const&, Vector const&) is invoked
 * @remarks When a tank rock is thrown.
 *
 * @param tank			tank client index (can be -1 if the rock was created by the native "L4D_TankRockPrj" or the "env_rock_launcher" entity)
 * @param rock			the rock entity index
 * @param vecPos		the position vector of the rock
 * @param vecAng		the angle vector of the rock
 * @param vecVel		the velocity vector of the rock
 * @param vecRot		the rotation vector of the rock
 *
 * @return				Plugin_Changed to modify the vector values, Plugin_Continue otherwise.
 */
forward Action L4D_TankRock_OnRelease(int tank, int rock, float vecPos[3], float vecAng[3], float vecVel[3], float vecRot[3]);


/**
 * @brief Called whenever BossZombiePlayer(CTerrorPlayer *, int, CBaseEntity *) is invoked
 * @remarks Called when Special Infected are targeting a victim
 *
 * @param specialInfected	the SI entity index
 * @param curTarget			the survivor player index who is chosen victim
 *
 * @return					Plugin_Handled to block, Plugin_Changed to use overwritten values from plugin, Plugin_Continue otherwise
 */
forward Action L4D2_OnChooseVictim(int specialInfected, int &curTarget); //post hook

/**
 * @brief Called whenever CDirectorScriptedEventManager::ChangeFinaleStage is invoked
 * @remarks Called when the director stage changes
 * @remarks some values for FinaleStageType: 1 - Finale Started; 6 - Rescue Vehicle Ready; 7 - Zombie Hordes; 8 - Tank; 10 - Combat Respite (nothing spawns)
 * @remarks SendInRescueVehicle does not depend on Finale Stage being 6, that only signals endless Hordes/Tanks
 * @remarks Can use the "FINALE_*" enums (search for them above) for the finaleType value.
 *
 * @param FinaleStageType	integer value
 *
 * @return			Plugin_Handled to block, Plugin_Changed to change finaleType, Plugin_Continue otherwise
 */
// L4D2 only.
forward Action L4D2_OnChangeFinaleStage(int &finaleType, const char[] arg);









/**
 * @brief Calls ZombieManager::SpawnTank(Vector&,QAngle&)
 *
 * @param vecPos	Vector coordinate where the tank will be spawned
 * @param vecAng	QAngle where the tank will be facing
 *
 * @return			Entity index of the spawned tank
 */
native int L4D2_SpawnTank(const float vecPos[3], const float vecAng[3]);

/**
 * @brief Calls ZombieManager::SpawnWitch(Vector&,QAngle&)
 *
 * @param vecPos	Vector coordinate where the witch will be spawned
 * @param vecAng	QAngle where the witch will be facing
 *
 * @return			Entity index of the spawned witch
 */
native int L4D2_SpawnWitch(const float vecPos[3], const float vecAng[3]);

/**
 * @brief Calls ZombieManager::SpawnSpecial(ZombieClassType,Vector&,QAngle&)
 * @remarks Only used for bot special spawns (not players)
 *
 * @param vecPos	Vector coordinate where the SI will be spawned
 * @param vecAng	QAngle where the SI will be facing
 *
 * @return			Entity index of the spawned SI
 */
native int L4D2_SpawnSpecial(int zombieClass, const float vecPos[3], const float vecAng[3]);

/**
 * @brief Attempts to find a random valid position to spawn a Special Infected.
 * @remarks The zombieClass does not matter but different values yield different results:
 * @remarks Using the Tank zombieClass probably searches for a larger area that's clear of objects
 *
 * @param client			Client id to find an area near this player. Accepts 0 to find a random area instead
 * @param zombieClass		Special Infected class to search for a spawn position for
 * @param attempts			How many tries to find a valid location
 * @param vecPos			The vector array to store the valid location on success
 *
 * @return					True on success, false on failure to find valid location
 */
native bool L4D_GetRandomPZSpawnPosition(int client, int zombieClass, int attempts, float vecPos[3]);

/**
 * @brief Checks if a Survivor is currently staggering
 *
 * @param client			Client ID of the player to affect
 *
 * @return Returns true if player is staggering, false otherwise
 */
native bool L4D_IsPlayerStaggering(int client) // 自定义

/**
 * @brief Plays specified music string name on the music channel to a client.
 * @remarks Music strings such as "Event.BleedingOut"
 * @remarks List of strings can be found inside the games VPK files here: scripts\game_sounds_music.txt
 *
 * @param client		Client id of the Survivor player
 * @param music_str		Music string name to play
 * @param source_ent	Source entity to play from (can be 0)
 * @param one_float		Unknown, maybe duration? Please report what this is when using.
 * @param one_bool		Unknown. Please report what this is when using.
 * @param two_bool		Unknown. Please report what this is when using.
 *
 * @noreturn
 */
native void L4D_PlayMusic(int client, const char[] music_str, int source_ent = 0, float one_float, bool one_bool, bool two_bool);

/**
 * @brief Stops playing the specified music_str to the client.
 * @remarks Music strings such as "Event.BleedingOut"
 * @remarks List of strings can be found inside the games VPK files here: scripts\game_sounds_music.txt
 *
 * @param client		Client id of the Survivor player
 * @param music_str		Music string name to stop playing
 * @param one_float		Unknown, maybe duration? Please report what this is when using.
 * @param one_bool		Unknown. Please report what this is when using.
 *
 * @noreturn
 */
native void L4D_StopMusic(int client, const char[] music_str, float one_float = 0.0, bool one_bool = false);

/**
 * @brief Returns if the current game mode is Coop/Realism mode.
 *
 * @return				True if the current game mode is Coop/Realism mode, false otherwise.
 **/
// L4D2 only.
native bool L4D2_IsGenericCooperativeMode();

/**
 * An alternative to TeleportEntity that maintains lerped movement client-side when setting an entity's new origin.
 * Note: Does not account for parented entities, uses world space.
 *
 * @param entity 		Entity index to set new origin of.
 * @param vec			New origin of the entity.
 *
 * @noreturn
 */
native void SetAbsOrigin(int entity, const float vec[3]) //自定义

/**
 * An alternative to TeleportEntity that maintains lerped movement client-side when setting an entity's new angles.
 * Note: Does not account for parented entities, uses world space.
 *
 * @param entity 		Entity index to set new angles of.
 * @param vec			New angles of the entity.
 *
 * @noreturn
 */
native void SetAbsAngles(int entity, const float vec[3]) //自定义

/**
 * An alternative to TeleportEntity that maintains lerped movement client-side when setting an entity's new velocity.
 * Note: Does not account for parented entities, uses world space.
 * Note: Does not overwrite "m_vecBaseVelocity".
 *
 * @param entity 		Entity index to set new velocity of.
 * @param vec			Velocity to apply to entity.
 *
 * @noreturn
 */
native void SetAbsVelocity(int entity, const float vec[3]) //自定义

/**
 * @brief To takeover a Survivor bot. First use "ChangeClientTeam" and change them to 0. Then call "L4D_SetHumanSpec" then call "L4D_TakeOverBot".
 *
 * @param bot		Bot ID of the person to set spectator
 * @param client	Client ID of the spectator
 *
 * @return			True or false
 */
native bool L4D_SetHumanSpec(int bot, int client);

/**
 * @brief To takeover a Survivor bot. First use "ChangeClientTeam" and change them to 0. Then call "L4D_SetHumanSpec" then call "L4D_TakeOverBot".
 * 
 * @param client	Client ID of who should takeover
 *
 * @return			True or false
 */
native bool L4D_TakeOverBot(int client);

/**
 * @brief Enter ghost/dead mode. Some state values may have different results. Unknown.
 * @remarks 6 and 8 are commonly used by the game.
 * @remarks Can use the "STATE_*" enums (search for them above).
 *
 * @param client			Client ID of the player to affect
 *
 * @noreturn
 */
native void L4D_State_Transition(int client, any state);

/**
 * @brief Sets a players zombie class, special infected can be alive and change!
 * @remarks Valid values L4D1: 1-3. L4D2: 1-6
 * @remarks zombieClass: 1=Smoker, 2=Boomer, 3=Hunter, 4=Spitter, 5=Jockey, 6=Charger
 *
 * @param client			Client ID of the player to kill. Not common infected
 * @param zombieClass		Zombie class number to change to
 *
 * @noreturn
 */
native void L4D_SetClass(int client, int zombieClass);


/**
 * @brief Takeover another special infected.
 * @remarks L4D1: Due to some bug and a workaround, when spawning you'll hear another special infected sound other than your own type
 *
 * @param client			Client ID of the special infected taking over
 * @param target			Client ID of the special infected losing control
 *
 * @noreturn
 */
native void L4D_TakeOverZombieBot(int client, int target);

/**
 * @brief Tells if the Mission (map) is the first map of the campaign
 *
 * @return			true if the map is the first map of the campaign
 */
native bool L4D_IsFirstMapInScenario();

/**
 * @brief Tells if the Mission (map) is the final map of the campaign
 *
 * @return			true if the map is the last map of the campaign (finale)
 */
native bool L4D_IsMissionFinalMap();

/**
 * Get the max flow distance (in flow units) for the current map.
 * @note The flow distance for each map is generated as it is loaded, and it can change slightly (a few hundred units) with each load.
 * @note You can use this value to convert a flow distance to a flow percent, and vice versa.
 *
 * @return				Max flow distance for the current loaded map.
 * @error				TerrorNavMesh address not found.
 */
native float L4D2Direct_GetMapMaxFlowDistance();

/**
 * Gets a player's distance in flow units.
 *
 * @param client			Client ID
 *
 * @return					0.0 on error otherwise flow distance
 */
native float L4D2Direct_GetFlowDistance(int client);

/**
 * @brief Returns client who is furthest in flow
 *
 * @return					Client ID of the player furthest ahead
 */
native int L4D_GetHighestFlowSurvivor();

/**
 * @brief Gets the maximum flow distance any survivor has achieved.
 *
 * @return			Returns the maximum flow distance any survivor has achieved.
 */
// L4D2 only.
native float L4D2_GetFurthestSurvivorFlow();

/**
 * @brief Returns the current Finale stage type.
 * @remarks some values for FinaleStageType: 1 - Finale Started; 6 - Rescue Vehicle Ready; 7 - Zombie Hordes; 8 - Tank; 10 - Combat Respite (nothing spawns)
 * @remarks Seems to return 18 for non-finale maps
 * @remarks Can use the "FINALE_*" enums (search for them above) for the finaleType value.
 *
 * @return			finaleType stage value
 */
// L4D2 only.
native any L4D2_GetCurrentFinaleStage();

/**
 * Sets the number of infected waiting to spawn
 *
 * @param count				Mob size
 *
 * @noreturn
 */
native void L4D2Direct_SetPendingMobCount(int count);

/**
 * @brief Revive a dead player by defib.
 *
 * @param client		Client to revive
 *
 * @return				True on success (does not guarantee they were revived), false on script error.
 */
// L4D2 only.
native void L4D2_VScriptWrapper_ReviveByDefib(int client); //自定义

/**
 * @brief Returns the value of the specified Director Variable key.
 * @remarks You should provide a valid default value to use as the native is likely to return that unless the key value has been modified by a mutation or director script.
 * @remarks See the "left4dhooks_test.sp" plugin and search for "L4D2_GetScriptValueInt" to see a list of keys and their default values and related cvars.
 *
 * @param key		Director variable key name to search for
 * @param value		Default value to use when the variable is not found
 *
 * @return			Value of the variable, or provided default value on failure
 */
// L4D2 only.
native int L4D2_GetScriptValueInt(const char[] key, int value);

/**
 * @brief Returns the value of the specified Director Variable key.
 * @remarks You should provide a valid default value to use as the native is likely to return that unless the key value has been modified by a mutation or director script.
 * @remarks See the "left4dhooks_test.sp" plugin and search for "L4D2_GetScriptValueFloat" to see a list of keys and their default values and related cvars.
 *
 * @param key		Director variable key name to search for
 * @param value		Default value to use when the variable is not found
 *
 * @return			Value of the variable, or provided default value on failure
 */
// L4D2 only.
native float L4D2_GetScriptValueFloat(const char[] key, float value);


public Extension __ext_left4dhooks =
{
	name = "left4dhooks",
	file = "left4dhooks.ext",
	autoload = 1,
	required = 1,
};

#if !defined REQUIRE_EXTENSIONS
public __ext_left4dhooks_SetNTVOptional()
{
}
#endif
