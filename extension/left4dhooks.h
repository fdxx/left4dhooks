#pragma once

#include <smsdk_ext.h>

class left4dhooks : public SDKExtension
{
public:
	bool SDK_OnLoad(char *error, size_t maxlen, bool late);
	void SDK_OnUnload();
	bool PrepForward(IGameConfig *gamedata);
	bool PrepNatives(IGameConfig *gamedata);
	void ClearForward();
	void ClearNatives();
};


template <typename T>
inline void GetAddressExt(IGameConfig *gamedata, const char *name, T &addr, bool &result)
{
	if (!gamedata->GetAddress(name, (void**)&addr)) 
	{
		smutils->LogError(myself, "Failed to GetAddress: %s", name);
		result = false;
	}
}

template <typename T>
inline void GetOffsetExt(IGameConfig *gamedata, const char *name, T &value, bool &result)
{
	if (!gamedata->GetOffset(name, (int*)&value)) 
	{
		smutils->LogError(myself, "Failed to GetOffset: %s", name);
		result = false;
	}
}

template <typename T>
inline void GetMemSigExt(IGameConfig *gamedata, const char *name, T &addr, bool &result)
{
	if (!gamedata->GetMemSig(name, (void**)&addr) || !addr) 
	{
		smutils->LogError(myself, "Failed to GetMemSig: %s", name);
		result = false;
	}
}
