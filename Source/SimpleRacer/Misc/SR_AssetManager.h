// Copyright 2023 Christina Piberger. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "Engine/DataAsset.h"
#include "SR_AssetManagerStartupJob.h"
#include "SR_AssetManager.generated.h"

class USR_GameData;

/**
 * Asset Manager used for this project (see Lyra project as reference).
 * It provides a good place for game-specific loading logic. (e.g. native gameplay tags)
 * This class is used by setting 'AssetManagerClassName' in DefaultEngine.ini.
 */
UCLASS(Config = Game)
class SIMPLERACER_API USR_AssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	USR_AssetManager();

	// Returns the AssetManager singleton object.
	static USR_AssetManager& Get();

	// Returns the asset referenced by a TSoftObjectPtr. This will synchronously load the asset if it's not already loaded.
	template<typename AssetType>
	static AssetType* GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);

	// Returns the subclass referenced by a TSoftClassPtr. This will synchronously load the asset if it's not already loaded.
	template<typename AssetType>
	static TSubclassOf<AssetType> GetSubclass(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);

	static UObject* SynchronousLoadAsset(const FSoftObjectPath& AssetPath);

	static bool ShouldLogAssetLoads();

	// Thread safe way of adding a loaded asset to keep in memory.
	void AddLoadedAsset(const UObject* Asset);

	const USR_GameData& GetGameData();

protected:
	virtual void StartInitialLoading() override;

	// Global game data asset to use.
	UPROPERTY(Config)
	TSoftObjectPtr<USR_GameData> GameDataPath;

	// Loaded version of the game data
	UPROPERTY(Transient)
	TMap<TObjectPtr<UClass>, TObjectPtr<UPrimaryDataAsset>> GameDataMap;

	template <typename GameDataClass>
	const GameDataClass& GetOrLoadTypedGameData(const TSoftObjectPtr<GameDataClass>& DataPath);

	UPrimaryDataAsset* LoadGameDataOfClass(TSubclassOf<UPrimaryDataAsset> DataClass, const TSoftObjectPtr<UPrimaryDataAsset>& DataClassPath, FPrimaryAssetType PrimaryAssetType);

private:
	// Flushes the StartupJobs array. Processes all startup work.
	void DoAllStartupJobs();

	// Called periodically during loads, could be used to feed the status to a loading screen
	void UpdateInitialGameContentLoadPercent(float GameContentPercent);

	// The list of tasks to execute on startup. Used to track startup progress.
	TArray<FSR_AssetManagerStartupJob> StartupJobs;

	// Assets loaded and tracked by the asset manager.
	UPROPERTY()
	TSet<TObjectPtr<const UObject>> LoadedAssets;

	// Used for a scope lock when modifying the list of load assets.
	FCriticalSection LoadedAssetsCritical;
};

template <typename AssetType>
AssetType* USR_AssetManager::GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory)
{
	AssetType* LoadedAsset = nullptr;

	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();

	if (AssetPath.IsValid())
	{
		LoadedAsset = AssetPointer.Get();
		if (!LoadedAsset)
		{
			LoadedAsset = Cast<AssetType>(SynchronousLoadAsset(AssetPath));
			ensureAlwaysMsgf(LoadedAsset, TEXT("Failed to load asset [%s]"), *AssetPointer.ToString());
		}

		if (LoadedAsset && bKeepInMemory)
		{
			// Added to loaded asset list.
			Get().AddLoadedAsset(Cast<UObject>(LoadedAsset));
		}
	}

	return LoadedAsset;
}

template <typename AssetType>
TSubclassOf<AssetType> USR_AssetManager::GetSubclass(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepInMemory)
{
	TSubclassOf<AssetType> LoadedSubclass;

	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();

	if (AssetPath.IsValid())
	{
		LoadedSubclass = AssetPointer.Get();
		if (!LoadedSubclass)
		{
			LoadedSubclass = Cast<UClass>(SynchronousLoadAsset(AssetPath));
			ensureAlwaysMsgf(LoadedSubclass, TEXT("Failed to load asset class [%s]"), *AssetPointer.ToString());
		}

		if (LoadedSubclass && bKeepInMemory)
		{
			// Added to loaded asset list.
			Get().AddLoadedAsset(Cast<UObject>(LoadedSubclass));
		}
	}

	return LoadedSubclass;
}

template <typename GameDataClass>
const GameDataClass& USR_AssetManager::GetOrLoadTypedGameData(const TSoftObjectPtr<GameDataClass>& DataPath)
{
	if (TObjectPtr<UPrimaryDataAsset> const* pResult = GameDataMap.Find(GameDataClass::StaticClass()))
	{
		return *CastChecked<GameDataClass>(*pResult);
	}

	// Does a blocking load if needed
	return *CastChecked<const GameDataClass>(LoadGameDataOfClass(GameDataClass::StaticClass(), DataPath,
		GameDataClass::StaticClass()->GetFName()));
}
