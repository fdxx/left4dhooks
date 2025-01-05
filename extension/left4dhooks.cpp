#include "left4dhooks.h"

left4dhooks g_left4dhooks;
SMEXT_LINK(&g_left4dhooks);

bool left4dhooks::SDK_OnLoad( char *error, size_t maxlength, bool late )
{
	IGameConfig *gamedata = nullptr;
	const char *buffer = nullptr;

	buffer = "left4dhooks.ext";
	if (!gameconfs->LoadGameConfigFile(buffer, &gamedata, error, maxlength))
	{
		snprintf(error, maxlength, "Failed to LoadGameConfigFile: %s", buffer);
		return false;
	}

	if (!PrepForward(gamedata, error, maxlength))
		return false;

	if (!PrepNatives(gamedata, error, maxlength))
		return false;
	
	sharesys->RegisterLibrary(myself, "left4dhooks");
	gameconfs->CloseGameConfigFile(gamedata);
	return true;
}

void left4dhooks::SDK_OnUnload()
{
	ClearForward();
	ClearNatives();
}


