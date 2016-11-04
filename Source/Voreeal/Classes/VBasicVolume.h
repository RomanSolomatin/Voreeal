#pragma once

#include <PolyVox/RawVolume.h>

#include "VBaseVolume.h"
#include "VBasicVolume.generated.h"

/**
 * Basic Voxel Volume;
 *	Enclosed volume with just basic LOD.
 */
UCLASS(Blueprintable, meta = (DisplayThumbnail = "true"))
class VOREEAL_API UBasicVolume : public UBaseVolume
{
	GENERATED_BODY()
	
public:
	typedef uint32 VoxelType;
	typedef PolyVox::RawVolume<VoxelType> VolumeType;
	
public:
	UBasicVolume(const class FObjectInitializer& ObjectInitializer);

	// Begin UObject Interface.
	virtual void Serialize(FArchive& Ar) override;
	virtual void PostInitProperties() override;
	virtual void PostLoad() override;
#if WITH_EDITORONLY_DATA
	virtual void GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const override;
#endif
	// End UObject Interface

	// Begin UBaseVolume Interface
	virtual bool IsValid() const override;
	// End UBaseVolume Interface
	
	/// Gets the enclosing region.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Voreeal")
	FRegion GetEnclosingRegion() const;

	/// Resize the volume.
	UFUNCTION(BlueprintCallable, Category = "Voreeal")
	void Resize(const FIntVector& NewSize);

	/// Resize the volume.
	UFUNCTION(BlueprintCallable, Category = "Voreeal")
	void ResizeRegion(const FRegion& NewRegion);

public:
#if WITH_EDITORONLY_DATA
	UPROPERTY(Category = ImportSettings, VisibleAnywhere, Instanced)
	class UAssetImportData* AssetImportData;
	class USceneThumbnailInfo* ThumbnailInfo;
#endif

protected:
	virtual bool Internal_SetVoxel(const FVector& Location, const uint32& Data) override;
	virtual void Internal_GetVoxel(const FVector& Location, uint32& Data) override;
	virtual void Internal_SetSize(const FRegion& Region, bool New);

private:
	std::unique_ptr<VolumeType> Volume;

	UPROPERTY()
	TArray<uint8> ImportedData;

	friend class UVVolumeImporterFactory;
};
