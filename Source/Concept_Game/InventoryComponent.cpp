// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"

#include "InventoryWidget.h"
#include "MainCharacter.h"
#include "PieMenu.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetArrayLibrary.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent() : bInventoryVisible(false),
                                             bQuickSelectVisible(false),
                                             bQuickSelectVisibleRef(false),
                                             bInventoryDirty(false) {
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UInventoryComponent::QuickSelectPieToggle(bool Visible) {
	if (OwningCharacter) {
		bQuickSelectVisible = Visible;
		QuickSelectInteract();
	}
}

void UInventoryComponent::QuickSelectInteract() {
	if (bQuickSelectVisible) {
		if (QuickSelectPieWidget) {
			if (bQuickSelectVisibleRef != bQuickSelectVisible) {
				UGameplayStatics::GetPlayerController(this, 0)->
					SetMouseLocation(GetViewportCenter().X, GetViewportCenter().Y);
				// UGameplayStatics::GetPlayerController(this, 0)->bShowMouseCursor = true;
				UGameplayStatics::GetPlayerController(this, 0)->ClientIgnoreMoveInput(true);
				UGameplayStatics::GetPlayerController(this, 0)->ClientIgnoreLookInput(true);
				// UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(UGameplayStatics::GetPlayerController(this, 0),
				// QuickSelectPieWidget);
				QuickSelectPieWidget->SetSectorsToPieMenuWidget();
				bQuickSelectVisibleRef = bQuickSelectVisible;
			}

			QuickSelectPieWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
			// QuickSelectWidget->AddToViewport();
			QuickSelectPieWidget->SetAlignmentInViewport(GetViewportCenter());
		}
	}
	else {
		if (QuickSelectPieWidget) {
			if (bQuickSelectVisibleRef != bQuickSelectVisible) {
				// UGameplayStatics::GetPlayerController(this, 0)->bShowMouseCursor = false;
				UGameplayStatics::GetPlayerController(this, 0)->ClientIgnoreMoveInput(false);
				UGameplayStatics::GetPlayerController(this, 0)->ClientIgnoreLookInput(false);
				// UWidgetBlueprintLibrary::SetInputMode_GameOnly(UGameplayStatics::GetPlayerController(this, 0));
				bQuickSelectVisibleRef = bQuickSelectVisible;
			}

			QuickSelectPieWidget->SetVisibility(ESlateVisibility::Hidden);
			// QuickSelectWidget->RemoveFromViewport();
		}
	}
}

void UInventoryComponent::InventoryToggle() {
	if (OwningCharacter) {
		bInventoryVisible = !bInventoryVisible;
		InventoryInteract();
	}
}

void UInventoryComponent::InventoryInteract() {
	UE_LOG(LogTemp, Warning, TEXT("InventoryInteract"));
	if (InventoryWidget) {
		if (bInventoryVisible) {
			UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(UGameplayStatics::GetPlayerController(this, 0),
			                                                  InventoryWidget);
			UGameplayStatics::GetPlayerController(this, 0)->bShowMouseCursor = true;
			// UGameplayStatics::GetPlayerController(this, 0)->ClientIgnoreMoveInput(true);
			// UGameplayStatics::GetPlayerController(this, 0)->ClientIgnoreLookInput(true);
			InventoryWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
			InventoryWidget->SetAlignmentInViewport(GetViewportCenter());
		}
		else {
			UWidgetBlueprintLibrary::SetInputMode_GameOnly(UGameplayStatics::GetPlayerController(this, 0));
			UGameplayStatics::GetPlayerController(this, 0)->bShowMouseCursor = false;
			// UGameplayStatics::GetPlayerController(this, 0)->ClientIgnoreMoveInput(false);
			// UGameplayStatics::GetPlayerController(this, 0)->ClientIgnoreLookInput(false);
			InventoryWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}


// Called when the game starts
void UInventoryComponent::BeginPlay() {
	Super::BeginPlay();

	OwningCharacter = Cast<AMainCharacter>(GetOwner());
	// ...
	InventoryItems.SetNum(InventoryColumns * InventoryRows);
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

FVector2D UInventoryComponent::GetViewportCenter() {
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport) {
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	return {ViewportSize.X / 2.0f, ViewportSize.Y / 2.0f};
}

void UInventoryComponent::SetQuickSelectPieWidgetSelection() {
	if (QuickSelectPieWidget) {
		QuickSelectPieWidget->UpdatePieMenuSector();
		// QuickSelectPieWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

bool UInventoryComponent::QuitActionButtonPressed() {
	bool bInteract = false;
	if (bInventoryVisible) {
		InventoryToggle();
		bInteract = true;
	}

	return bInteract;
}

void UInventoryComponent::CreateQuickSelectPieWidget(UPieMenu* InQuickSelectWidget) {
	QuickSelectPieWidget = InQuickSelectWidget;
	QuickSelectPieWidget->SetOwnerInventoryComponent(this);
	QuickSelectPieWidget->AddToViewport();
	QuickSelectPieWidget->SetVisibility(ESlateVisibility::Hidden);
}

void UInventoryComponent::CreateInventoryWidget(UInventoryWidget* InInventoryWidget) {
	InventoryWidget = InInventoryWidget;
	InventoryWidget->SetOwnerInventoryComponent(this);
	InventoryWidget->AddToViewport();
	InventoryWidget->SetVisibility(ESlateVisibility::Hidden);
}

FInventoryTile UInventoryComponent::IndexToTile(int Index) const {
	return {Index % InventoryColumns, Index / InventoryColumns};
}

int UInventoryComponent::TileToIndex(FInventoryTile InTile) const {
	return InTile.X + (InTile.Y * InventoryColumns);
}

FInventoryTile UInventoryComponent::OutputEachTile(AItem* InInventoryItem, int TopLeftIndex) {
	return {};
}

bool UInventoryComponent::CheckInventorySpace(AItem* InInventoryItem, int TopLeftIndex) {
	const FIntPoint ItemDimension = InInventoryItem->GetItemDimensions();
	const FInventoryTile Tiles = IndexToTile(TopLeftIndex);
	UE_LOG(LogTemp, Warning, TEXT("Check Inventory Space: %s Tiles: X -> %i Y -> %i"), *ItemDimension.ToString(),
	       Tiles.X, Tiles.Y)
	for (int i = Tiles.X; i <= Tiles.X + (ItemDimension.X - 1); i++) {
		for (int j = Tiles.Y; j <= Tiles.Y + (ItemDimension.Y - 1); j++) {
			const FInventoryTile NewTile = {i, j};
			if (NewTile.X >= 0 && NewTile.Y >= 0
				// && NewTile.X < 0 && NewTile.Y < 0
			) {
				if (GetItemAtIndex(TileToIndex(NewTile)) != nullptr) {
					return false;
				}
			}
			else {
				return false;
			}
		}
	}

	return true;

}

bool UInventoryComponent::TryAddInventoryItem(AItem* InInventoryItem) {
	bool bInventoryItemAdded = false;
	for (int i = 0; i <= InventoryItems.Num(); i++) {
		UE_LOG(LogTemp, Error, TEXT("Inventory -> %i"), i)
		if (CheckInventorySpace(InInventoryItem, i)) {
			return AddInventoryItem(InInventoryItem, i);
		}
	}

	return bInventoryItemAdded;
}

bool UInventoryComponent::AddInventoryItem(AItem* InInventoryItem, int TopLeftIndex) {
	const FIntPoint ItemDimension = InInventoryItem->GetItemDimensions();
	const FInventoryTile Tiles = IndexToTile(TopLeftIndex);
	for (int i = Tiles.X; i <= Tiles.X + (ItemDimension.X - 1); i++) {
		for (int j = Tiles.Y; j <= Tiles.Y + (ItemDimension.Y - 1); j++) {
			const FInventoryTile NewTile = {i, j};
			InventoryItems[TileToIndex(NewTile)] = InInventoryItem;
			bInventoryDirty = true;
		}
	}

	return bInventoryDirty;
}

AItem* UInventoryComponent::GetItemAtIndex(int InIndex) {
	if (InventoryItems.IsValidIndex(InIndex)) {
		return InventoryItems[InIndex];
	}

	return nullptr;
}

bool UInventoryComponent::RemoveInventoryItem(AItem* InInventoryItem) {
	return false;
}

bool UInventoryComponent::RemoveInventoryItemAtLocation(int32 Index) {
	return false;
}

AItem* UInventoryComponent::GetInventoryItemAtLocation(int32 Index) {
	return nullptr;
}

void UInventoryComponent::ModifyInventoryItem(AItem* InventoryItem) {
}

void UInventoryComponent::ModifyInventoryItemAtLocation(int32 Index) {
}

void UInventoryComponent::CombineInventoryItems(AItem* InventoryItemA, AItem* InventoryItemB) {
}

void UInventoryComponent::CombineInventoryItemsAtLocations(int32 IndexA, int32 IndexB) {
}

void UInventoryComponent::SetToQuickSelect(AItem* InventoryItem) {
}

void UInventoryComponent::RemoveFromQuickSelect(AItem* InventoryItem) {
}
