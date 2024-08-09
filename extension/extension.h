#pragma once

#include "smsdk_ext.h"

class left4dhooks : public SDKExtension
{
public:
	bool SDK_OnLoad(char *error, size_t maxlen, bool late);
	void SDK_OnUnload();
	bool PrepForward(IGameConfig *gamedata, char *error, size_t maxlength);
	bool PrepNatives(IGameConfig *gamedata, char *error, size_t maxlength);
	void ClearForward();
	void ClearNatives();
};

