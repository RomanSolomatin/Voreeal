#include "VoreealPrivatePCH.h"
#include "VBasicVolumeComponent.h"

#include "MessageLog.h"
#include "VBlueprintLibrary.h"

UBasicVolumeComponent::UBasicVolumeComponent(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, m_octree(nullptr)
{
	PrimaryComponentTick.bCanEverTick = false;
}

UBasicVolumeComponent::~UBasicVolumeComponent()
{
	if (m_octree != nullptr)
	{
		delete m_octree;
	}

	if (timerHandle.IsValid())
	{
		//GetWorld()->GetTimerManager().ClearTimer(timerHandle);
	}
}

void UBasicVolumeComponent::PostLoad()
{
	Super::PostLoad();

	if (Volume)
	{
		Volume->ConditionalPostLoad();
		EnsureRendering();
	}
}

FString UBasicVolumeComponent::GetDetailedInfoInternal() const
{
	return (Volume != NULL) ? Volume->GetPathName(NULL) : TEXT("No_Volume");
}

bool UBasicVolumeComponent::SetBasicVolume(class UBasicVolume* NewVolume)
{
	if (NewVolume == Volume)
	{
		return false;
	}

	AActor* Owner = GetOwner();
	if (!AreDynamicDataChangesAllowed() && Owner != NULL)
	{
		FMessageLog("PIE").Warning(FText::Format(
			FText::FromString(TEXT("Calling SetBasicVolume on '{0}' but Mobility is Static.")),
			FText::FromString(GetPathName())));
		return false;
	}

	Volume = NewVolume;

	EnsureRendering();
	// TODO: Mark dirty

	return true;
}

void UBasicVolumeComponent::DrawDebugOctree(const FColor& Color, float Duration, float Thickness)
{
	if (m_octree)
	{
		m_octree->Traverse([=](FSparseOctreeNode* node) -> ETraverseOptions
		{
			if (node->m_hasChildren)
			{
				return ETraverseOptions::Continue;
			}

			UVBlueprintLibrary::DrawDebugRegion(this, this->GetComponentTransform(), node->m_bounds, Color, Duration, Thickness);

			return ETraverseOptions::Skip;
		});
	}
}

void UBasicVolumeComponent::EnsureRendering()
{
	if (Volume != nullptr && m_octree == nullptr)
	{
		m_octree = new FSparseOctree(Volume, MeshComponent, EOctreeConstructionModes::BoundCells);


		UWorld* World = GetWorld();
		FTimerManager& TimerManager = World->GetTimerManager();
		TimerManager.SetTimer(timerHandle, this, &UBasicVolumeComponent::Update, 1.0f, true);
	}
}

void UBasicVolumeComponent::Update()
{
	if (m_octree)
	{
		m_octree->Update(FVector(0, 0, 0));
	}
}
