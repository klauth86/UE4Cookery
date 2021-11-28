// Copyright Epic Games, Inc. All Rights Reserved.

#include "WorldWidgetComponent.h"
#include "PrimitiveViewRelevance.h"
#include "PrimitiveSceneProxy.h"
#include "UObject/ConstructorHelpers.h"
#include "EngineGlobals.h"
#include "MaterialShared.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/Engine.h"
#include "Widgets/SWindow.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Framework/Application/SlateApplication.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Input/HittestGrid.h"
#include "SceneManagement.h"
#include "DynamicMeshBuilder.h"
#include "PhysicsEngine/BoxElem.h"
#include "PhysicsEngine/BodySetup.h"
#include "Slate/SGameLayerManager.h"
#include "Slate/WidgetRenderer.h"
#include "UObject/EditorObjectVersion.h"
#include "Widgets/SViewport.h"

DECLARE_CYCLE_STAT(TEXT("3DHitTesting"), STAT_Slate3DHitTesting, STATGROUP_Slate);

static int32 MaximumRenderTargetWidth = 3840;
static FAutoConsoleVariableRef CVarMaximumRenderTargetWidth
(
	TEXT("WidgetComponent.MaximumRenderTargetWidth"),
	MaximumRenderTargetWidth,
	TEXT("Sets the maximum width of the render target used by a Widget Component.")
);

static int32 MaximumRenderTargetHeight = 2160;
static FAutoConsoleVariableRef CVarMaximumRenderTargetHeight
(
	TEXT("WidgetComponent.MaximumRenderTargetHeight"),
	MaximumRenderTargetHeight,
	TEXT("Sets the maximum height of the render target used by a Widget Component.")
);




/**
* The hit tester used by all Widget Component objects.
*/
class FWidget3DHitTester : public ICustomHitTestPath {
public:
	FWidget3DHitTester(UWorld* InWorld)
		: World(InWorld)
		, CachedFrame(-1) {}

	// ICustomHitTestPath implementation
	virtual TArray<FWidgetAndPointer> GetBubblePathAndVirtualCursors(const FGeometry& InGeometry, FVector2D DesktopSpaceCoordinate, bool bIgnoreEnabledStatus) const override {
		SCOPE_CYCLE_COUNTER(STAT_Slate3DHitTesting);

		if (World.IsValid() /*&& ensure( World->IsGameWorld() )*/) {
			UWorld* SafeWorld = World.Get();
			if (SafeWorld) {
				ULocalPlayer* const TargetPlayer = GEngine->GetLocalPlayerFromControllerId(SafeWorld, 0);

				if (TargetPlayer && TargetPlayer->PlayerController) {
					FVector2D LocalMouseCoordinate = InGeometry.AbsoluteToLocal(DesktopSpaceCoordinate) * InGeometry.Scale;

					if (UPrimitiveComponent* HitComponent = GetHitResultAtScreenPositionAndCache(TargetPlayer->PlayerController, LocalMouseCoordinate)) {
						if (UWorldWidgetComponent* WidgetComponent = Cast<UWorldWidgetComponent>(HitComponent)) {
							if (WidgetComponent->GetReceiveHardwareInput()) {
								if (WidgetComponent->GetCurrentDrawSize().X != 0 && WidgetComponent->GetCurrentDrawSize().Y != 0) {
									// Get the "forward" vector based on the current rotation system.
									const FVector ForwardVector = WidgetComponent->GetForwardVector();

									// Make sure the player is interacting with the front of the widget
									if (FVector::DotProduct(ForwardVector, CachedHitResult.ImpactPoint - CachedHitResult.TraceStart) < 0.f) {
										return WidgetComponent->GetHitWidgetPath(CachedHitResult.Location, bIgnoreEnabledStatus);
									}
								}
							}
						}
					}
				}
			}
		}

		return TArray<FWidgetAndPointer>();
	}

	virtual void ArrangeCustomHitTestChildren(FArrangedChildren& ArrangedChildren) const override {
		for (TWeakObjectPtr<UWorldWidgetComponent> Component : RegisteredComponents) {
			UWorldWidgetComponent* WidgetComponent = Component.Get();
			// Check if visible;
			if (WidgetComponent && WidgetComponent->GetSlateWindow().IsValid()) {
				FGeometry WidgetGeom;

				ArrangedChildren.AddWidget(FArrangedWidget(WidgetComponent->GetSlateWindow().ToSharedRef(), WidgetGeom.MakeChild(WidgetComponent->GetCurrentDrawSize(), FSlateLayoutTransform())));
			}
		}
	}

	virtual TSharedPtr<struct FVirtualPointerPosition> TranslateMouseCoordinateForCustomHitTestChild(const TSharedRef<SWidget>& ChildWidget, const FGeometry& ViewportGeometry, const FVector2D& ScreenSpaceMouseCoordinate, const FVector2D& LastScreenSpaceMouseCoordinate) const override {
		if (World.IsValid() && ensure(World->IsGameWorld())) {
			ULocalPlayer* const TargetPlayer = GEngine->GetLocalPlayerFromControllerId(World.Get(), 0);
			if (TargetPlayer && TargetPlayer->PlayerController) {
				FVector2D LocalMouseCoordinate = ViewportGeometry.AbsoluteToLocal(ScreenSpaceMouseCoordinate) * ViewportGeometry.Scale;

				// Check for a hit against any widget components in the world
				for (TWeakObjectPtr<UWorldWidgetComponent> Component : RegisteredComponents) {
					UWorldWidgetComponent* WidgetComponent = Component.Get();
					// Check if visible;
					if (WidgetComponent && WidgetComponent->GetSlateWindow() == ChildWidget) {
						if (UPrimitiveComponent* HitComponent = GetHitResultAtScreenPositionAndCache(TargetPlayer->PlayerController, LocalMouseCoordinate)) {
							if (WidgetComponent->GetReceiveHardwareInput()) {
								if (WidgetComponent->GetCurrentDrawSize().X != 0 && WidgetComponent->GetCurrentDrawSize().Y != 0) {
									if (WidgetComponent == HitComponent) {
										TSharedPtr<FVirtualPointerPosition> VirtualCursorPos = MakeShareable(new FVirtualPointerPosition);

										FVector2D LocalHitLocation;
										WidgetComponent->GetLocalHitLocation(CachedHitResult.Location, LocalHitLocation);

										VirtualCursorPos->CurrentCursorPosition = LocalHitLocation;
										VirtualCursorPos->LastCursorPosition = LocalHitLocation;

										return VirtualCursorPos;
									}
								}
							}
						}
					}
				}
			}
		}

		return nullptr;
	}
	// End ICustomHitTestPath

	UPrimitiveComponent* GetHitResultAtScreenPositionAndCache(APlayerController* PlayerController, FVector2D ScreenPosition) const {
		UPrimitiveComponent* HitComponent = nullptr;

		if (GFrameNumber != CachedFrame || CachedScreenPosition != ScreenPosition) {
			CachedFrame = GFrameNumber;
			CachedScreenPosition = ScreenPosition;

			if (PlayerController) {
				if (PlayerController->GetHitResultAtScreenPosition(ScreenPosition, ECC_Visibility, true, CachedHitResult)) {
					return CachedHitResult.Component.Get();
				}
			}
		}
		else {
			return CachedHitResult.Component.Get();
		}

		return nullptr;
	}

	void RegisterWidgetComponent(UWorldWidgetComponent* InComponent) {
		RegisteredComponents.AddUnique(InComponent);
	}

	void UnregisterWidgetComponent(UWorldWidgetComponent* InComponent) {
		RegisteredComponents.RemoveSingleSwap(InComponent);
	}

	uint32 GetNumRegisteredComponents() const { return RegisteredComponents.Num(); }

	UWorld* GetWorld() const { return World.Get(); }

private:
	TArray< TWeakObjectPtr<UWorldWidgetComponent> > RegisteredComponents;
	TWeakObjectPtr<UWorld> World;

	mutable int64 CachedFrame;
	mutable FVector2D CachedScreenPosition;
	mutable FHitResult CachedHitResult;
};



/** Represents a billboard sprite to the scene manager. */
class FWidget3DSceneProxy final : public FPrimitiveSceneProxy {
public:
	SIZE_T GetTypeHash() const override {
		static size_t UniquePointer;
		return reinterpret_cast<size_t>(&UniquePointer);
	}

	/** Initialization constructor. */
	FWidget3DSceneProxy(UWorldWidgetComponent* InComponent, ISlate3DRenderer& InRenderer)
		: FPrimitiveSceneProxy(InComponent)
		, Pivot(InComponent->GetPivot())
		, Renderer(InRenderer)
		, RenderTarget(InComponent->GetRenderTarget())
		, MaterialInstance(InComponent->GetMaterialInstance())
		, BlendMode(InComponent->GetBlendMode())
		, BodySetup(InComponent->GetBodySetup()) {
		bWillEverBeLit = false;

		MaterialRelevance = MaterialInstance->GetRelevance_Concurrent(GetScene().GetFeatureLevel());
	}

	// FPrimitiveSceneProxy interface.
	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override {
#if WITH_EDITOR
		const bool bWireframe = AllowDebugViewmodes() && ViewFamily.EngineShowFlags.Wireframe;

		auto WireframeMaterialInstance = new FColoredMaterialRenderProxy(
			GEngine->WireframeMaterial ? GEngine->WireframeMaterial->GetRenderProxy() : nullptr,
			FLinearColor(0, 0.5f, 1.f)
		);

		Collector.RegisterOneFrameMaterialProxy(WireframeMaterialInstance);

		FMaterialRenderProxy* ParentMaterialProxy = nullptr;
		if (bWireframe) {
			ParentMaterialProxy = WireframeMaterialInstance;
		}
		else {
			ParentMaterialProxy = MaterialInstance->GetRenderProxy();
		}
#else
		FMaterialRenderProxy* ParentMaterialProxy = MaterialInstance->GetRenderProxy();
#endif

		//FSpriteTextureOverrideRenderProxy* TextureOverrideMaterialProxy = new FSpriteTextureOverrideRenderProxy(ParentMaterialProxy,

		const FMatrix& ViewportLocalToWorld = GetLocalToWorld();

		FMatrix PreviousLocalToWorld;

		if (!GetScene().GetPreviousLocalToWorld(GetPrimitiveSceneInfo(), PreviousLocalToWorld)) {
			PreviousLocalToWorld = GetLocalToWorld();
		}

		if (RenderTarget) {
			FTextureResource* TextureResource = RenderTarget->Resource;
			if (TextureResource) {
				float U = -RenderTarget->SizeX * Pivot.X;
				float V = -RenderTarget->SizeY * Pivot.Y;
				float UL = RenderTarget->SizeX * (1.0f - Pivot.X);
				float VL = RenderTarget->SizeY * (1.0f - Pivot.Y);

				int32 VertexIndices[4];

				for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++) {
					auto projMatrix = Views[ViewIndex]->ViewMatrices.GetProjectionMatrix();

					FDynamicMeshBuilder MeshBuilder(Views[ViewIndex]->GetFeatureLevel());

					if (VisibilityMap & (1 << ViewIndex)) {
						VertexIndices[0] = MeshBuilder.AddVertex(-FVector(0, U, V), FVector2D(0, 0), FVector(0, -1, 0), FVector(0, 0, -1), FVector(1, 0, 0), FColor::White);
						VertexIndices[1] = MeshBuilder.AddVertex(-FVector(0, U, VL), FVector2D(0, 1), FVector(0, -1, 0), FVector(0, 0, -1), FVector(1, 0, 0), FColor::White);
						VertexIndices[2] = MeshBuilder.AddVertex(-FVector(0, UL, VL), FVector2D(1, 1), FVector(0, -1, 0), FVector(0, 0, -1), FVector(1, 0, 0), FColor::White);
						VertexIndices[3] = MeshBuilder.AddVertex(-FVector(0, UL, V), FVector2D(1, 0), FVector(0, -1, 0), FVector(0, 0, -1), FVector(1, 0, 0), FColor::White);

						MeshBuilder.AddTriangle(VertexIndices[0], VertexIndices[1], VertexIndices[2]);
						MeshBuilder.AddTriangle(VertexIndices[0], VertexIndices[2], VertexIndices[3]);

						FDynamicMeshBuilderSettings Settings;
						Settings.bDisableBackfaceCulling = false;
						Settings.bReceivesDecals = true;
						Settings.bUseSelectionOutline = true;
						MeshBuilder.GetMesh(ViewportLocalToWorld, PreviousLocalToWorld, ParentMaterialProxy, SDPG_World, Settings, nullptr, ViewIndex, Collector, FHitProxyId());


						UE_LOG(LogTemp, Warning, TEXT("!!!"))
					}
				}
			}
		}

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++) {
			if (VisibilityMap & (1 << ViewIndex)) {
				RenderCollision(BodySetup, Collector, ViewIndex, ViewFamily.EngineShowFlags, GetBounds(), IsSelected());
				RenderBounds(Collector.GetPDI(ViewIndex), ViewFamily.EngineShowFlags, GetBounds(), IsSelected());
			}
		}
#endif
	}

	void RenderCollision(UBodySetup* InBodySetup, FMeshElementCollector& Collector, int32 ViewIndex, const FEngineShowFlags& EngineShowFlags, const FBoxSphereBounds& InBounds, bool bRenderInEditor) const {
		if (InBodySetup) {
			bool bDrawCollision = EngineShowFlags.Collision && IsCollisionEnabled();

			if (bDrawCollision && AllowDebugViewmodes()) {
				// Draw simple collision as wireframe if 'show collision', collision is enabled, and we are not using the complex as the simple
				const bool bDrawSimpleWireframeCollision = InBodySetup->CollisionTraceFlag != ECollisionTraceFlag::CTF_UseComplexAsSimple;

				if (FMath::Abs(GetLocalToWorld().Determinant()) < SMALL_NUMBER) {
					// Catch this here or otherwise GeomTransform below will assert
					// This spams so commented out
					//UE_LOG(LogStaticMesh, Log, TEXT("Zero scaling not supported (%s)"), *StaticMesh->GetPathName());
				}
				else {
					const bool bDrawSolid = !bDrawSimpleWireframeCollision;
					const bool bProxyIsSelected = IsSelected();

					if (bDrawSolid) {
						// Make a material for drawing solid collision stuff
						auto SolidMaterialInstance = new FColoredMaterialRenderProxy(
							GEngine->ShadedLevelColorationUnlitMaterial->GetRenderProxy(),
							GetWireframeColor()
						);

						Collector.RegisterOneFrameMaterialProxy(SolidMaterialInstance);

						FTransform GeomTransform(GetLocalToWorld());
						InBodySetup->AggGeom.GetAggGeom(GeomTransform, GetWireframeColor().ToFColor(true), SolidMaterialInstance, false, true, DrawsVelocity(), ViewIndex, Collector);
					}
					// wireframe
					else {
						FColor CollisionColor = FColor(157, 149, 223, 255);
						FTransform GeomTransform(GetLocalToWorld());
						InBodySetup->AggGeom.GetAggGeom(GeomTransform, GetSelectionColor(CollisionColor, bProxyIsSelected, IsHovered()).ToFColor(true), nullptr, false, false, DrawsVelocity(), ViewIndex, Collector);
					}
				}
			}
		}
	}

	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override {
		bool bVisible = true;

		FPrimitiveViewRelevance Result;

		MaterialRelevance.SetPrimitiveViewRelevance(Result);

		Result.bDrawRelevance = IsShown(View) && bVisible && View->Family->EngineShowFlags.WidgetComponents;
		Result.bDynamicRelevance = true;
		Result.bRenderCustomDepth = ShouldRenderCustomDepth();
		Result.bRenderInMainPass = ShouldRenderInMainPass();
		Result.bUsesLightingChannels = GetLightingChannelMask() != GetDefaultLightingChannelMask();
		Result.bShadowRelevance = IsShadowCast(View);
		Result.bTranslucentSelfShadow = bCastVolumetricTranslucentShadow;
		Result.bEditorPrimitiveRelevance = false;
		Result.bVelocityRelevance = IsMovable() && Result.bOpaque && Result.bRenderInMainPass;

		return Result;
	}

	virtual void GetLightRelevance(const FLightSceneProxy* LightSceneProxy, bool& bDynamic, bool& bRelevant, bool& bLightMapped, bool& bShadowMapped) const override {
		bDynamic = false;
		bRelevant = false;
		bLightMapped = false;
		bShadowMapped = false;
	}

	virtual void OnTransformChanged() override {
		Origin = GetLocalToWorld().GetOrigin();
	}

	virtual bool CanBeOccluded() const override {
		return !MaterialRelevance.bDisableDepthTest;
	}

	virtual uint32 GetMemoryFootprint(void) const override { return(sizeof(*this) + GetAllocatedSize()); }

	uint32 GetAllocatedSize(void) const { return(FPrimitiveSceneProxy::GetAllocatedSize()); }

private:
	FVector Origin;
	float ArcAngle;
	FVector2D Pivot;
	ISlate3DRenderer& Renderer;
	UTextureRenderTarget2D* RenderTarget;
	UMaterialInstanceDynamic* MaterialInstance;
	FMaterialRelevance MaterialRelevance;
	EWidgetBlendMode BlendMode;
	UBodySetup* BodySetup;
};






UWorldWidgetComponent::UWorldWidgetComponent(const FObjectInitializer& PCIP)
	: Super(PCIP)
	, DrawSize(FIntPoint(500, 500))
	, bManuallyRedraw(false)
	, bRedrawRequested(true)
	, RedrawTime(0)
	, LastWidgetRenderTime(0)
	, bReceiveHardwareInput(false)
	, bWindowFocusable(true)
	, WindowVisibility(EWindowVisibility::SelfHitTestInvisible)
	, bApplyGammaCorrection(false)
	, BackgroundColor(FLinearColor::Transparent)
	, TintColorAndOpacity(FLinearColor::White)
	, OpacityFromTexture(1.0f)
	, BlendMode(EWidgetBlendMode::Masked)
	, bIsTwoSided(false)
	, TickWhenOffscreen(false)
	, TickMode(ETickMode::Enabled)
	, bRenderCleared(false)
	, bOnWidgetVisibilityChangedRegistered(false) {
	PrimaryComponentTick.bCanEverTick = true;
	bTickInEditor = true;

	SetRelativeRotation(FRotator::ZeroRotator);

	BodyInstance.SetCollisionProfileName(FName(TEXT("UI")));

	// Translucent material instances
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> TranslucentMaterial_Finder(TEXT("/Engine/EngineMaterials/Widget3DPassThrough_Translucent"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> TranslucentMaterial_OneSided_Finder(TEXT("/Engine/EngineMaterials/Widget3DPassThrough_Translucent_OneSided"));
	TranslucentMaterial = TranslucentMaterial_Finder.Object;
	TranslucentMaterial_OneSided = TranslucentMaterial_OneSided_Finder.Object;

	// Opaque material instances
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> OpaqueMaterial_Finder(TEXT("/Engine/EngineMaterials/Widget3DPassThrough_Opaque"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> OpaqueMaterial_OneSided_Finder(TEXT("/Engine/EngineMaterials/Widget3DPassThrough_Opaque_OneSided"));
	OpaqueMaterial = OpaqueMaterial_Finder.Object;
	OpaqueMaterial_OneSided = OpaqueMaterial_OneSided_Finder.Object;

	// Masked material instances
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaskedMaterial_Finder(TEXT("/Engine/EngineMaterials/Widget3DPassThrough_Masked"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaskedMaterial_OneSided_Finder(TEXT("/Engine/EngineMaterials/Widget3DPassThrough_Masked_OneSided"));
	MaskedMaterial = MaskedMaterial_Finder.Object;
	MaskedMaterial_OneSided = MaskedMaterial_OneSided_Finder.Object;

	LastLocalHitLocation = FVector2D::ZeroVector;
	//SetGenerateOverlapEvents(false);
	bUseEditorCompositing = false;

	TimingPolicy = EWidgetTimingPolicy::RealTime;
	Pivot = FVector2D(0.5f, 0.5f);
}

bool UWorldWidgetComponent::CanBeInCluster() const {
	return false;
}

void UWorldWidgetComponent::BeginPlay() {
	SetComponentTickEnabled(TickMode != ETickMode::Disabled);
	InitWidget();
	Super::BeginPlay();

	CurrentDrawSize = DrawSize;
	UpdateBodySetup(true);
	RecreatePhysicsState();
}

void UWorldWidgetComponent::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);

	ReleaseResources();
}

void UWorldWidgetComponent::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld) {
	// If the InLevel is null, it's a signal that the entire world is about to disappear, so
	// go ahead and remove this widget from the viewport, it could be holding onto too many
	// dangerous actor references that won't carry over into the next world.
	if (InLevel == nullptr && InWorld == GetWorld()) {
		ReleaseResources();
	}
}


void UWorldWidgetComponent::SetMaterial(int32 ElementIndex, UMaterialInterface* Material) {
	Super::SetMaterial(ElementIndex, Material);

	UpdateMaterialInstance();
}

void UWorldWidgetComponent::UpdateMaterialInstance() {
	// Always clear the material instance in case we're going from 3D to 2D.
	MaterialInstance = nullptr;

	UMaterialInterface* BaseMaterial = GetMaterial(0);
	MaterialInstance = UMaterialInstanceDynamic::Create(BaseMaterial, this);
	UpdateMaterialInstanceParameters();

	MarkRenderStateDirty();
}

FPrimitiveSceneProxy* UWorldWidgetComponent::CreateSceneProxy() {
	if (WidgetRenderer && CurrentSlateWidget.IsValid()) {
		RequestRenderUpdate();
		LastWidgetRenderTime = 0;

		return new FWidget3DSceneProxy(this, *WidgetRenderer->GetSlateRenderer());
	}

#if WITH_EDITOR
	// make something so we can see this component in the editor
	class FWidgetBoxProxy final : public FPrimitiveSceneProxy {
	public:
		SIZE_T GetTypeHash() const override {
			static size_t UniquePointer;
			return reinterpret_cast<size_t>(&UniquePointer);
		}

		FWidgetBoxProxy(const UWorldWidgetComponent* InComponent)
			: FPrimitiveSceneProxy(InComponent)
			, BoxExtents(1.f, InComponent->GetCurrentDrawSize().X / 2.0f, InComponent->GetCurrentDrawSize().Y / 2.0f) {
			bWillEverBeLit = false;
		}

		virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override {
			QUICK_SCOPE_CYCLE_COUNTER(STAT_BoxSceneProxy_GetDynamicMeshElements);

			const FMatrix& LocalToWorld = GetLocalToWorld();

			for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++) {
				if (VisibilityMap & (1 << ViewIndex)) {
					const FSceneView* View = Views[ViewIndex];

					const FLinearColor DrawColor = GetViewSelectionColor(FColor::White, *View, IsSelected(), IsHovered(), false, IsIndividuallySelected());

					FPrimitiveDrawInterface* PDI = Collector.GetPDI(ViewIndex);
					DrawOrientedWireBox(PDI, LocalToWorld.GetOrigin(), LocalToWorld.GetScaledAxis(EAxis::X), LocalToWorld.GetScaledAxis(EAxis::Y), LocalToWorld.GetScaledAxis(EAxis::Z), BoxExtents, DrawColor, SDPG_World);
				}
			}
		}

		virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override {
			FPrimitiveViewRelevance Result;
			if (!View->bIsGameView) {
				// Should we draw this because collision drawing is enabled, and we have collision
				const bool bShowForCollision = View->Family->EngineShowFlags.Collision && IsCollisionEnabled();
				Result.bDrawRelevance = IsShown(View) || bShowForCollision;
				Result.bDynamicRelevance = true;
				Result.bShadowRelevance = IsShadowCast(View);
				Result.bEditorPrimitiveRelevance = UseEditorCompositing(View);
			}
			return Result;
		}
		virtual uint32 GetMemoryFootprint(void) const override { return(sizeof(*this) + GetAllocatedSize()); }
		uint32 GetAllocatedSize(void) const { return(FPrimitiveSceneProxy::GetAllocatedSize()); }

	private:
		const FVector	BoxExtents;
	};

	return new FWidgetBoxProxy(this);
#else
	return nullptr;
#endif
}

FBoxSphereBounds UWorldWidgetComponent::CalcBounds(const FTransform& LocalToWorld) const {
	const FVector Origin = FVector(.5f,
		-(CurrentDrawSize.X * 0.5f) + (CurrentDrawSize.X * Pivot.X),
		-(CurrentDrawSize.Y * 0.5f) + (CurrentDrawSize.Y * Pivot.Y));

	const FVector BoxExtent = FVector(1.f, CurrentDrawSize.X / 2.0f, CurrentDrawSize.Y / 2.0f);

	FBoxSphereBounds NewBounds(Origin, BoxExtent, CurrentDrawSize.Size() / 2.0f);
	NewBounds = NewBounds.TransformBy(LocalToWorld);

	NewBounds.BoxExtent *= BoundsScale;
	NewBounds.SphereRadius *= BoundsScale;

	return NewBounds;
}

UBodySetup* UWorldWidgetComponent::GetBodySetup() {
	UpdateBodySetup();
	return BodySetup;
}

FCollisionShape UWorldWidgetComponent::GetCollisionShape(float Inflation) const {
	FVector BoxHalfExtent = (FVector(0.01f, CurrentDrawSize.X * 0.5f, CurrentDrawSize.Y * 0.5f) * GetComponentTransform().GetScale3D()) + Inflation;

	if (Inflation < 0.0f) {
		// Don't shrink below zero size.
		BoxHalfExtent = BoxHalfExtent.ComponentMax(FVector::ZeroVector);
	}

	return FCollisionShape::MakeBox(BoxHalfExtent);
}

void UWorldWidgetComponent::OnRegister() {
	// Set this prior to registering the scene component so that bounds are calculated correctly.
	CurrentDrawSize = DrawSize;

	Super::OnRegister();

#if !UE_SERVER
	FWorldDelegates::LevelRemovedFromWorld.AddUObject(this, &ThisClass::OnLevelRemovedFromWorld);

	if (!IsRunningDedicatedServer()) {
		const bool bIsGameWorld = GetWorld()->IsGameWorld();
		if (bReceiveHardwareInput && bIsGameWorld) {
			TSharedPtr<SViewport> GameViewportWidget = GEngine->GetGameViewportWidget();
			RegisterHitTesterWithViewport(GameViewportWidget);
		}

		if (!WidgetRenderer && !GUsingNullRHI) {
			WidgetRenderer = new FWidgetRenderer(bApplyGammaCorrection);
		}

		BodySetup = nullptr;

#if WITH_EDITOR
		if (!bIsGameWorld) {
			InitWidget();
		}
#endif
	}
#endif // !UE_SERVER
}

void UWorldWidgetComponent::SetWindowFocusable(bool bInWindowFocusable) {
	bWindowFocusable = bInWindowFocusable;
	if (SlateWindow.IsValid()) {
		SlateWindow->SetIsFocusable(bWindowFocusable);
	}
};

EVisibility UWorldWidgetComponent::ConvertWindowVisibilityToVisibility(EWindowVisibility visibility) {
	switch (visibility) {
	case EWindowVisibility::Visible:
		return EVisibility::Visible;
	case EWindowVisibility::SelfHitTestInvisible:
		return EVisibility::SelfHitTestInvisible;
	default:
		checkNoEntry();
		return EVisibility::SelfHitTestInvisible;
	}
}

void UWorldWidgetComponent::OnWidgetVisibilityChanged(ESlateVisibility InVisibility) {
	ensure(TickMode == ETickMode::Automatic);
	ensure(Widget);
	ensure(bOnWidgetVisibilityChangedRegistered);

	if (InVisibility != ESlateVisibility::Collapsed && InVisibility != ESlateVisibility::Hidden) {
		SetComponentTickEnabled(true);
		if (bOnWidgetVisibilityChangedRegistered) {
			Widget->OnNativeVisibilityChanged.RemoveAll(this);
			bOnWidgetVisibilityChangedRegistered = false;
		}
	}
}

void UWorldWidgetComponent::SetWindowVisibility(EWindowVisibility InVisibility) {
	ensure(TickMode == ETickMode::Automatic);
	ensure(Widget);
	ensure(bOnWidgetVisibilityChangedRegistered);

	WindowVisibility = InVisibility;
	if (SlateWindow.IsValid()) {
		SlateWindow->SetVisibility(ConvertWindowVisibilityToVisibility(WindowVisibility));
	}

	if (IsWidgetVisible()) {
		SetComponentTickEnabled(TickMode != ETickMode::Disabled);
		if (bOnWidgetVisibilityChangedRegistered) {
			if (Widget) {
				Widget->OnNativeVisibilityChanged.RemoveAll(this);
			}
			bOnWidgetVisibilityChangedRegistered = false;
		}
	}
}

void UWorldWidgetComponent::SetTickMode(ETickMode InTickMode) {
	TickMode = InTickMode;
	SetComponentTickEnabled(InTickMode != ETickMode::Disabled);
}

bool UWorldWidgetComponent::IsWidgetVisible() const {
	if (!SlateWindow.IsValid() || !SlateWindow->GetVisibility().IsVisible()) {
		return false;
	}

	if (Widget) {
		return Widget->IsVisible();
	}

	return SlateWidget.IsValid() && SlateWidget->GetVisibility().IsVisible();
}

void UWorldWidgetComponent::RegisterHitTesterWithViewport(TSharedPtr<SViewport> ViewportWidget) {
#if !UE_SERVER
	if (ViewportWidget.IsValid()) {
		TSharedPtr<ICustomHitTestPath> CustomHitTestPath = ViewportWidget->GetCustomHitTestPath();
		if (!CustomHitTestPath.IsValid()) {
			CustomHitTestPath = MakeShareable(new FWidget3DHitTester(GetWorld()));
			ViewportWidget->SetCustomHitTestPath(CustomHitTestPath);
		}

		TSharedPtr<FWidget3DHitTester> Widget3DHitTester = StaticCastSharedPtr<FWidget3DHitTester>(CustomHitTestPath);
		if (Widget3DHitTester->GetWorld() == GetWorld()) {
			Widget3DHitTester->RegisterWidgetComponent(this);
		}
	}
#endif

}

void UWorldWidgetComponent::UnregisterHitTesterWithViewport(TSharedPtr<SViewport> ViewportWidget) {
#if !UE_SERVER
	if (bReceiveHardwareInput) {
		TSharedPtr<ICustomHitTestPath> CustomHitTestPath = ViewportWidget->GetCustomHitTestPath();
		if (CustomHitTestPath.IsValid()) {
			TSharedPtr<FWidget3DHitTester> WidgetHitTestPath = StaticCastSharedPtr<FWidget3DHitTester>(CustomHitTestPath);

			WidgetHitTestPath->UnregisterWidgetComponent(this);

			if (WidgetHitTestPath->GetNumRegisteredComponents() == 0) {
				ViewportWidget->SetCustomHitTestPath(nullptr);
			}
		}
	}
#endif
}

void UWorldWidgetComponent::OnUnregister() {
#if !UE_SERVER
	FWorldDelegates::LevelRemovedFromWorld.RemoveAll(this);

	if (GetWorld()->IsGameWorld()) {
		TSharedPtr<SViewport> GameViewportWidget = GEngine->GetGameViewportWidget();
		if (GameViewportWidget.IsValid()) {
			UnregisterHitTesterWithViewport(GameViewportWidget);
		}
	}
#endif

#if WITH_EDITOR
	if (!GetWorld()->IsGameWorld()) {
		ReleaseResources();
	}
#endif

	Super::OnUnregister();
}

void UWorldWidgetComponent::DestroyComponent(bool bPromoteChildren/*= false*/) {
	Super::DestroyComponent(bPromoteChildren);

	ReleaseResources();
}

void UWorldWidgetComponent::ReleaseResources() {
	if (Widget) {
		Widget = nullptr;
	}

	if (SlateWidget.IsValid()) {
		SlateWidget.Reset();
	}

	if (WidgetRenderer) {
		BeginCleanup(WidgetRenderer);
		WidgetRenderer = nullptr;
	}

	UnregisterWindow();
}

void UWorldWidgetComponent::RegisterWindow() {
	if (SlateWindow.IsValid()) {
		if (!bReceiveHardwareInput && FSlateApplication::IsInitialized()) {
			FSlateApplication::Get().RegisterVirtualWindow(SlateWindow.ToSharedRef());
		}

		if (Widget && !Widget->IsDesignTime()) {
			if (UWorld* LocalWorld = GetWorld()) {
				if (LocalWorld->IsGameWorld()) {
					UGameInstance* GameInstance = LocalWorld->GetGameInstance();
					check(GameInstance);

					UGameViewportClient* GameViewportClient = GameInstance->GetGameViewportClient();
					if (GameViewportClient) {
						SlateWindow->AssignParentWidget(GameViewportClient->GetGameViewportWidget());
					}
				}
			}
		}
	}
}

void UWorldWidgetComponent::UnregisterWindow() {
	if (SlateWindow.IsValid()) {
		if (!bReceiveHardwareInput && FSlateApplication::IsInitialized()) {
			FSlateApplication::Get().UnregisterVirtualWindow(SlateWindow.ToSharedRef());
		}

		SlateWindow.Reset();
	}
}

void UWorldWidgetComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

#if !UE_SERVER
	if (!IsRunningDedicatedServer()) {
		UpdateWidget();

		if (Widget == nullptr && !SlateWidget.IsValid() && bRenderCleared) {
			// We will enter here if the WidgetClass is empty and we already renderered an empty widget. No need to continue.
			return;
		}

		if (Widget && TickMode == ETickMode::Automatic && !IsWidgetVisible()) {
			SetComponentTickEnabled(false);
			if (!bOnWidgetVisibilityChangedRegistered) {
				Widget->OnNativeVisibilityChanged.AddUObject(this, &UWorldWidgetComponent::OnWidgetVisibilityChanged);
				bOnWidgetVisibilityChangedRegistered = true;
			}
			return;
		}

		if (TickMode == ETickMode::Disabled) {
			SetComponentTickEnabled(false);
			return;
		}

		if (ShouldDrawWidget()) {
			// Calculate the actual delta time since we last drew, this handles the case where we're ticking when
			// the world is paused, this also takes care of the case where the widget component is rendering at
			// a different rate than the rest of the world.
			const float DeltaTimeFromLastDraw = LastWidgetRenderTime == 0 ? 0 : (GetCurrentTime() - LastWidgetRenderTime);
			DrawWidgetToRenderTarget(DeltaTimeFromLastDraw);

			// We draw an empty widget.
			if (Widget == nullptr && !SlateWidget.IsValid()) {
				bRenderCleared = true;
			}
		}
	}
#endif // !UE_SERVER
}

bool UWorldWidgetComponent::ShouldDrawWidget() const {
	const float RenderTimeThreshold = .5f;
	if (IsVisible()) {
		// If we don't tick when off-screen, don't bother ticking if it hasn't been rendered recently
		if (TickWhenOffscreen || GetWorld()->TimeSince(GetLastRenderTime()) <= RenderTimeThreshold) {
			if ((GetCurrentTime() - LastWidgetRenderTime) >= RedrawTime) {
				return bManuallyRedraw ? bRedrawRequested : true;
			}
		}
	}

	return false;
}

void UWorldWidgetComponent::DrawWidgetToRenderTarget(float DeltaTime) {
	if (GUsingNullRHI) {
		return;
	}

	if (!SlateWindow.IsValid()) {
		return;
	}

	if (!WidgetRenderer) {
		return;
	}

	const int32 MaxAllowedDrawSize = GetMax2DTextureDimension();
	if (DrawSize.X <= 0 || DrawSize.Y <= 0 || DrawSize.X > MaxAllowedDrawSize || DrawSize.Y > MaxAllowedDrawSize) {
		return;
	}

	const FIntPoint PreviousDrawSize = CurrentDrawSize;
	CurrentDrawSize = DrawSize;

	const float DrawScale = 1.0f;

	if (bDrawAtDesiredSize) {
		SlateWindow->SlatePrepass(DrawScale);

		FVector2D DesiredSize = SlateWindow->GetDesiredSize();
		DesiredSize.X = FMath::RoundToInt(DesiredSize.X);
		DesiredSize.Y = FMath::RoundToInt(DesiredSize.Y);
		CurrentDrawSize = DesiredSize.IntPoint();

		WidgetRenderer->SetIsPrepassNeeded(false);
	}
	else {
		WidgetRenderer->SetIsPrepassNeeded(true);
	}

	if (CurrentDrawSize != PreviousDrawSize) {
		UpdateBodySetup(true);
		RecreatePhysicsState();
	}

	UpdateRenderTarget(CurrentDrawSize);

	// The render target could be null if the current draw size is zero
	if (RenderTarget) {
		bRedrawRequested = false;

		UE_LOG(LogTemp, Warning, TEXT("[%d, %d]"), CurrentDrawSize.X, CurrentDrawSize.Y)

			WidgetRenderer->DrawWindow(
				RenderTarget,
				SlateWindow->GetHittestGrid(),
				SlateWindow.ToSharedRef(),
				DrawScale,
				CurrentDrawSize,
				DeltaTime);

		LastWidgetRenderTime = GetCurrentTime();

		if (TickMode == ETickMode::Disabled && IsComponentTickEnabled()) {
			SetComponentTickEnabled(false);
		}
	}
}

float UWorldWidgetComponent::ComputeComponentWidth() const {
	return CurrentDrawSize.X;
}

double UWorldWidgetComponent::GetCurrentTime() const {
	return (TimingPolicy == EWidgetTimingPolicy::RealTime) ? FApp::GetCurrentTime() : static_cast<double>(GetWorld()->GetTimeSeconds());
}

void UWorldWidgetComponent::GetUsedMaterials(TArray<UMaterialInterface*>& OutMaterials, bool bGetDebugMaterials) const {
	if (MaterialInstance) {
		OutMaterials.AddUnique(MaterialInstance);
	}
}

#if WITH_EDITOR

bool UWorldWidgetComponent::CanEditChange(const FProperty* InProperty) const {
	if (InProperty) {
		FString PropertyName = InProperty->GetName();

		if (PropertyName == GET_MEMBER_NAME_STRING_CHECKED(UWorldWidgetComponent, TimingPolicy) ||
			PropertyName == GET_MEMBER_NAME_STRING_CHECKED(UWorldWidgetComponent, bWindowFocusable) ||
			PropertyName == GET_MEMBER_NAME_STRING_CHECKED(UWorldWidgetComponent, WindowVisibility) ||
			PropertyName == GET_MEMBER_NAME_STRING_CHECKED(UWorldWidgetComponent, bManuallyRedraw) ||
			PropertyName == GET_MEMBER_NAME_STRING_CHECKED(UWorldWidgetComponent, RedrawTime) ||
			PropertyName == GET_MEMBER_NAME_STRING_CHECKED(UWorldWidgetComponent, BackgroundColor) ||
			PropertyName == GET_MEMBER_NAME_STRING_CHECKED(UWorldWidgetComponent, TintColorAndOpacity) ||
			PropertyName == GET_MEMBER_NAME_STRING_CHECKED(UWorldWidgetComponent, OpacityFromTexture) ||
			PropertyName == GET_MEMBER_NAME_STRING_CHECKED(UWorldWidgetComponent, BlendMode) ||
			PropertyName == GET_MEMBER_NAME_STRING_CHECKED(UWorldWidgetComponent, bIsTwoSided) ||
			PropertyName == GET_MEMBER_NAME_STRING_CHECKED(UWorldWidgetComponent, TickWhenOffscreen)) {
			return true;
		}

		if (PropertyName == GET_MEMBER_NAME_STRING_CHECKED(UWorldWidgetComponent, bReceiveHardwareInput)) {
			return true;
		}
	}

	return Super::CanEditChange(InProperty);
}

void UWorldWidgetComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) {
	FProperty* Property = PropertyChangedEvent.MemberProperty;

	if (Property && PropertyChangedEvent.ChangeType != EPropertyChangeType::Interactive) {
		static FName DrawSizeName("DrawSize");
		static FName PivotName("Pivot");
		static FName WidgetClassName("WidgetClass");
		static FName IsOpaqueName("bIsOpaque");
		static FName IsTwoSidedName("bIsTwoSided");
		static FName BackgroundColorName("BackgroundColor");
		static FName TintColorAndOpacityName("TintColorAndOpacity");
		static FName OpacityFromTextureName("OpacityFromTexture");
		static FName ParabolaDistortionName(TEXT("ParabolaDistortion"));
		static FName BlendModeName(TEXT("BlendMode"));
		static FName bWindowFocusableName(TEXT("bWindowFocusable"));
		static FName WindowVisibilityName(TEXT("WindowVisibility"));

		auto PropertyName = Property->GetFName();

		if (PropertyName == DrawSizeName) {
			if (DrawSize.X > MaximumRenderTargetWidth) {
				DrawSize.X = MaximumRenderTargetWidth;
			}
			if (DrawSize.Y > MaximumRenderTargetHeight) {
				DrawSize.Y = MaximumRenderTargetHeight;
			}
		}

		if (PropertyName == WidgetClassName) {
			Widget = nullptr;

			UpdateWidget();
			MarkRenderStateDirty();
		}
		else if (PropertyName == DrawSizeName || PropertyName == PivotName) {
			MarkRenderStateDirty();
			UpdateBodySetup(true);
			RecreatePhysicsState();
		}
		else if (PropertyName == IsOpaqueName || PropertyName == IsTwoSidedName || PropertyName == BlendModeName) {
			MarkRenderStateDirty();
		}
		else if (PropertyName == BackgroundColorName || PropertyName == ParabolaDistortionName) {
			MarkRenderStateDirty();
		}
		else if (PropertyName == TintColorAndOpacityName || PropertyName == OpacityFromTextureName) {
			MarkRenderStateDirty();
		}
		else if (PropertyName == bWindowFocusableName) {
			SetWindowFocusable(bWindowFocusable);
		}
		else if (PropertyName == WindowVisibilityName) {
			SetWindowVisibility(WindowVisibility);
		}

	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

#endif

void UWorldWidgetComponent::InitWidget() {
	// Don't do any work if Slate is not initialized
	if (FSlateApplication::IsInitialized()) {
		UWorld* World = GetWorld();

		if (WidgetClass && Widget == nullptr && World && !World->bIsTearingDown) {
			Widget = CreateWidget(GetWorld(), WidgetClass);
			SetTickMode(TickMode);
		}

#if WITH_EDITOR
		if (Widget && !World->IsGameWorld() && !bEditTimeUsable) {
			if (!GEnableVREditorHacks) {
				// Prevent native ticking of editor component previews
				Widget->SetDesignerFlags(EWidgetDesignFlags::Designing);
			}
		}
#endif
	}
}

void UWorldWidgetComponent::SetManuallyRedraw(bool bUseManualRedraw) {
	bManuallyRedraw = bUseManualRedraw;
}

UUserWidget* UWorldWidgetComponent::GetWidget() const {
	return Widget;
}

void UWorldWidgetComponent::SetWidget(UUserWidget* InWidget) {
	if (InWidget != nullptr) {
		SetSlateWidget(nullptr);
	}

	Widget = InWidget;

	UpdateWidget();
}

void UWorldWidgetComponent::SetSlateWidget(const TSharedPtr<SWidget>& InSlateWidget) {
	if (Widget != nullptr) {
		SetWidget(nullptr);
	}

	if (SlateWidget.IsValid()) {
		SlateWidget.Reset();
	}

	SlateWidget = InSlateWidget;

	UpdateWidget();
}

void UWorldWidgetComponent::UpdateWidget() {
	// Don't do any work if Slate is not initialized
	if (FSlateApplication::IsInitialized() && !IsPendingKill()) {
		// Look for a UMG widget set
		TSharedPtr<SWidget> NewSlateWidget;
		if (Widget) {
			NewSlateWidget = Widget->TakeWidget();
		}

		// Create the SlateWindow if it doesn't exists
		bool bNeededNewWindow = false;
		if (!SlateWindow.IsValid()) {
			UpdateMaterialInstance();

			SlateWindow = SNew(SVirtualWindow).Size(CurrentDrawSize);
			SlateWindow->SetIsFocusable(bWindowFocusable);
			SlateWindow->SetVisibility(ConvertWindowVisibilityToVisibility(WindowVisibility));
			RegisterWindow();

			bNeededNewWindow = true;
		}

		SlateWindow->Resize(CurrentDrawSize);

		// Add the UMG or SlateWidget to the Component
		bool bWidgetChanged = false;

		// We Get here if we have a UMG Widget
		if (NewSlateWidget.IsValid()) {
			if (NewSlateWidget != CurrentSlateWidget || bNeededNewWindow) {
				CurrentSlateWidget = NewSlateWidget;
				SlateWindow->SetContent(NewSlateWidget.ToSharedRef());
				bRenderCleared = false;
				bWidgetChanged = true;
			}
		}
		// If we don't have one, we look for a Slate Widget
		else if (SlateWidget.IsValid()) {
			if (SlateWidget != CurrentSlateWidget || bNeededNewWindow) {
				CurrentSlateWidget = SlateWidget;
				SlateWindow->SetContent(SlateWidget.ToSharedRef());
				bRenderCleared = false;
				bWidgetChanged = true;
			}
		}
		else {
			if (CurrentSlateWidget != SNullWidget::NullWidget) {
				CurrentSlateWidget = SNullWidget::NullWidget;
				bRenderCleared = false;
				bWidgetChanged = true;
			}
			SlateWindow->SetContent(SNullWidget::NullWidget);
		}

		if (bNeededNewWindow || bWidgetChanged) {
			MarkRenderStateDirty();
			SetComponentTickEnabled(true);
		}
	}
}

void UWorldWidgetComponent::UpdateRenderTarget(FIntPoint DesiredRenderTargetSize) {
	bool bWidgetRenderStateDirty = false;
	bool bClearColorChanged = false;

	FLinearColor ActualBackgroundColor = BackgroundColor;
	switch (BlendMode) {
	case EWidgetBlendMode::Opaque:
		ActualBackgroundColor.A = 1.0f;
		break;
	case EWidgetBlendMode::Masked:
		ActualBackgroundColor.A = 0.0f;
		break;
	}

	if (DesiredRenderTargetSize.X != 0 && DesiredRenderTargetSize.Y != 0) {
		const EPixelFormat requestedFormat = FSlateApplication::Get().GetRenderer()->GetSlateRecommendedColorFormat();

		if (RenderTarget == nullptr) {
			RenderTarget = NewObject<UTextureRenderTarget2D>(this);
			RenderTarget->ClearColor = ActualBackgroundColor;

			bClearColorChanged = bWidgetRenderStateDirty = true;

			RenderTarget->InitCustomFormat(DesiredRenderTargetSize.X, DesiredRenderTargetSize.Y, requestedFormat, false);

			if (MaterialInstance) {
				MaterialInstance->SetTextureParameterValue("SlateUI", RenderTarget);
			}
		}
		else {
			bClearColorChanged = (RenderTarget->ClearColor != ActualBackgroundColor);

			// Update the clear color or format
			if (bClearColorChanged || RenderTarget->SizeX != DesiredRenderTargetSize.X || RenderTarget->SizeY != DesiredRenderTargetSize.Y) {
				RenderTarget->ClearColor = ActualBackgroundColor;
				RenderTarget->InitCustomFormat(DesiredRenderTargetSize.X, DesiredRenderTargetSize.Y, PF_B8G8R8A8, false);
				RenderTarget->UpdateResourceImmediate();
				bWidgetRenderStateDirty = true;
			}
		}
	}

	if (RenderTarget) {
		// If the clear color of the render target changed, update the BackColor of the material to match
		if (bClearColorChanged && MaterialInstance) {
			MaterialInstance->SetVectorParameterValue("BackColor", RenderTarget->ClearColor);
		}

		if (bWidgetRenderStateDirty) {
			MarkRenderStateDirty();
		}
	}
}

void UWorldWidgetComponent::UpdateBodySetup(bool bDrawSizeChanged) {

	if (!BodySetup || bDrawSizeChanged) {
		BodySetup = NewObject<UBodySetup>(this);
		BodySetup->CollisionTraceFlag = CTF_UseSimpleAsComplex;
		BodySetup->AggGeom.BoxElems.Add(FKBoxElem());

		FKBoxElem* BoxElem = BodySetup->AggGeom.BoxElems.GetData();

		const float Width = ComputeComponentWidth();
		const float Height = CurrentDrawSize.Y;
		const FVector Origin = FVector(.5f,
			-(Width * 0.5f) + (Width * Pivot.X),
			-(Height * 0.5f) + (Height * Pivot.Y));

		BoxElem->X = 0.01f;
		BoxElem->Y = Width;
		BoxElem->Z = Height;

		BoxElem->SetTransform(FTransform::Identity);
		BoxElem->Center = Origin;
	}
}

void UWorldWidgetComponent::GetLocalHitLocation(FVector WorldHitLocation, FVector2D& OutLocalWidgetHitLocation) const {
	// Find the hit location on the component
	FVector ComponentHitLocation = GetComponentTransform().InverseTransformPosition(WorldHitLocation);

	// Convert the 3D position of component space, into the 2D equivalent
	OutLocalWidgetHitLocation = FVector2D(-ComponentHitLocation.Y, -ComponentHitLocation.Z);

	// Offset the position by the pivot to get the position in widget space.
	OutLocalWidgetHitLocation.X += CurrentDrawSize.X * Pivot.X;
	OutLocalWidgetHitLocation.Y += CurrentDrawSize.Y * Pivot.Y;

	// Apply the parabola distortion
	FVector2D NormalizedLocation = OutLocalWidgetHitLocation / CurrentDrawSize;

	OutLocalWidgetHitLocation.Y = CurrentDrawSize.Y * NormalizedLocation.Y;
}

UUserWidget* UWorldWidgetComponent::GetUserWidgetObject() const {
	return Widget;
}

UTextureRenderTarget2D* UWorldWidgetComponent::GetRenderTarget() const {
	return RenderTarget;
}

UMaterialInstanceDynamic* UWorldWidgetComponent::GetMaterialInstance() const {
	return MaterialInstance;
}

const TSharedPtr<SWidget>& UWorldWidgetComponent::GetSlateWidget() const {
	return SlateWidget;
}

TArray<FWidgetAndPointer> UWorldWidgetComponent::GetHitWidgetPath(FVector WorldHitLocation, bool bIgnoreEnabledStatus, float CursorRadius) {
	FVector2D LocalHitLocation;
	GetLocalHitLocation(WorldHitLocation, LocalHitLocation);

	return GetHitWidgetPath(LocalHitLocation, bIgnoreEnabledStatus, CursorRadius);
}


TArray<FWidgetAndPointer> UWorldWidgetComponent::GetHitWidgetPath(FVector2D WidgetSpaceHitCoordinate, bool bIgnoreEnabledStatus, float CursorRadius /*= 0.0f*/) {
	TSharedRef<FVirtualPointerPosition> VirtualMouseCoordinate = MakeShareable(new FVirtualPointerPosition);

	const FVector2D& LocalHitLocation = WidgetSpaceHitCoordinate;

	VirtualMouseCoordinate->CurrentCursorPosition = LocalHitLocation;
	VirtualMouseCoordinate->LastCursorPosition = LastLocalHitLocation;

	// Cache the location of the hit
	LastLocalHitLocation = LocalHitLocation;

	TArray<FWidgetAndPointer> ArrangedWidgets;
	if (SlateWindow.IsValid()) {
		// @todo slate - widget components would need to be associated with a user for this to be anthing valid
		const int32 UserIndex = INDEX_NONE;
		ArrangedWidgets = SlateWindow->GetHittestGrid().GetBubblePath(LocalHitLocation, CursorRadius, bIgnoreEnabledStatus, UserIndex);

		for (FWidgetAndPointer& ArrangedWidget : ArrangedWidgets) {
			ArrangedWidget.PointerPosition = VirtualMouseCoordinate;
		}
	}

	return ArrangedWidgets;
}

TSharedPtr<SWindow> UWorldWidgetComponent::GetSlateWindow() const {
	return SlateWindow;
}

FVector2D UWorldWidgetComponent::GetDrawSize() const {
	return DrawSize;
}

FVector2D UWorldWidgetComponent::GetCurrentDrawSize() const {
	return CurrentDrawSize;
}

void UWorldWidgetComponent::SetDrawSize(FVector2D Size) {
	FIntPoint NewDrawSize((int32)Size.X, (int32)Size.Y);

	if (NewDrawSize != DrawSize) {
		DrawSize = NewDrawSize;
		MarkRenderStateDirty();
	}
}

void UWorldWidgetComponent::RequestRedraw() {
	bRedrawRequested = true;
}

void UWorldWidgetComponent::RequestRenderUpdate() {
	bRedrawRequested = true;
	if (TickMode == ETickMode::Disabled) {
		SetComponentTickEnabled(true);
	}
}

void UWorldWidgetComponent::SetBlendMode(const EWidgetBlendMode NewBlendMode) {
	if (NewBlendMode != this->BlendMode) {
		this->BlendMode = NewBlendMode;
		if (IsRegistered()) {
			MarkRenderStateDirty();
		}
	}
}

void UWorldWidgetComponent::SetTwoSided(const bool bWantTwoSided) {
	if (bWantTwoSided != this->bIsTwoSided) {
		this->bIsTwoSided = bWantTwoSided;
		if (IsRegistered()) {
			MarkRenderStateDirty();
		}
	}
}

void UWorldWidgetComponent::SetBackgroundColor(const FLinearColor NewBackgroundColor) {
	if (NewBackgroundColor != this->BackgroundColor) {
		this->BackgroundColor = NewBackgroundColor;
		MarkRenderStateDirty();
	}
}

void UWorldWidgetComponent::SetTintColorAndOpacity(const FLinearColor NewTintColorAndOpacity) {
	if (NewTintColorAndOpacity != this->TintColorAndOpacity) {
		this->TintColorAndOpacity = NewTintColorAndOpacity;
		UpdateMaterialInstanceParameters();
	}
}

void UWorldWidgetComponent::SetOpacityFromTexture(const float NewOpacityFromTexture) {
	if (NewOpacityFromTexture != this->OpacityFromTexture) {
		this->OpacityFromTexture = NewOpacityFromTexture;
		UpdateMaterialInstanceParameters();
	}
}

TSharedPtr< SWindow > UWorldWidgetComponent::GetVirtualWindow() const {
	return StaticCastSharedPtr<SWindow>(SlateWindow);
}

UMaterialInterface* UWorldWidgetComponent::GetMaterial(int32 MaterialIndex) const {
	if (OverrideMaterials.IsValidIndex(MaterialIndex) && (OverrideMaterials[MaterialIndex] != nullptr)) {
		return OverrideMaterials[MaterialIndex];
	}
	else {
		switch (BlendMode) {
		case EWidgetBlendMode::Opaque:
			return bIsTwoSided ? OpaqueMaterial : OpaqueMaterial_OneSided;
			break;
		case EWidgetBlendMode::Masked:
			return bIsTwoSided ? MaskedMaterial : MaskedMaterial_OneSided;
			break;
		case EWidgetBlendMode::Transparent:
			return bIsTwoSided ? TranslucentMaterial : TranslucentMaterial_OneSided;
			break;
		}
	}

	return nullptr;
}

int32 UWorldWidgetComponent::GetNumMaterials() const {
	return FMath::Max<int32>(OverrideMaterials.Num(), 1);
}

void UWorldWidgetComponent::UpdateMaterialInstanceParameters() {
	if (MaterialInstance) {
		MaterialInstance->SetTextureParameterValue("SlateUI", RenderTarget);
		MaterialInstance->SetVectorParameterValue("TintColorAndOpacity", TintColorAndOpacity);
		MaterialInstance->SetScalarParameterValue("OpacityFromTexture", OpacityFromTexture);
	}
}

void UWorldWidgetComponent::SetWidgetClass(TSubclassOf<UUserWidget> InWidgetClass) {
	if (WidgetClass != InWidgetClass) {
		WidgetClass = InWidgetClass;

		if (FSlateApplication::IsInitialized()) {
			if (HasBegunPlay() && !GetWorld()->bIsTearingDown) {
				if (WidgetClass) {
					UUserWidget* NewWidget = CreateWidget(GetWorld(), WidgetClass);
					SetWidget(NewWidget);
				}
				else {
					SetWidget(nullptr);
				}
			}
		}
	}
}