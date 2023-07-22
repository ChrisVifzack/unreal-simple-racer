// Copyright 2023 Christina Piberger. All rights reserved.


#include "SR_GameData.h"
#include "SR_AssetManager.h"


USR_GameData::USR_GameData()
{
}

const USR_GameData& USR_GameData::Get()
{
	return USR_AssetManager::Get().GetGameData();
}
