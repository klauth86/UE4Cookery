// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/WidgetComponent.h"
#include "WorldWidgetComponent.generated.h"

UCLASS(meta = (BlueprintSpawnableComponent))
class CPP012_API UWorldWidgetComponent : public UMeshComponent {

	GENERATED_UCLASS_BODY()

public:
	//UObject interface
	virtual bool CanBeInCluster() const override;
	//~ End UObject Interface

	/** UActorComponent Interface */
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/* UPrimitiveComponent Interface */
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
	virtual UBodySetup* GetBodySetup() override;
	virtual FCollisionShape GetCollisionShape(float Inflation) const override;
	virtual void OnRegister() override;
	virtual void OnUnregister() override;
	virtual void DestroyComponent(bool bPromoteChildren = false) override;
	UMaterialInterface* GetMaterial(int32 MaterialIndex) const override;
	virtual void SetMaterial(int32 ElementIndex, UMaterialInterface* Material) override;
	int32 GetNumMaterials() const override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetUsedMaterials(TArray<UMaterialInterface*>& OutMaterials, bool bGetDebugMaterials = false) const override;

#if WITH_EDITOR
	virtual bool CanEditChange(const FProperty* InProperty) const override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	/** Ensures the user widget is initialized */
	virtual void InitWidget();

	/** Release resources associated with the widget. */
	virtual void ReleaseResources();

	/** Ensures the 3d window is created its size and content. */
	virtual void UpdateWidget();

	/** Ensure the render target is initialized and updates it if needed. */
	virtual void UpdateRenderTarget(FIntPoint DesiredRenderTargetSize);

	/**
	* Ensures the body setup is initialized and updates it if needed.
	* @param bDrawSizeChanged Whether the draw size of this component has changed since the last update call.
	*/
	void UpdateBodySetup(bool bDrawSizeChanged = false);

	/**
	 * Converts a world-space hit result to a hit location on the widget
	 * @param HitResult The hit on this widget in the world
	 * @param (Out) The transformed 2D hit location on the widget
	 */
	virtual void GetLocalHitLocation(FVector WorldHitLocation, FVector2D& OutLocalHitLocation) const;

	/** Gets the last local location that was hit */
	FVector2D GetLastLocalHitLocation() const {
		return LastLocalHitLocation;
	}

	/** Returns the class of the user widget displayed by this component */
	TSubclassOf<UUserWidget> GetWidgetClass() const { return WidgetClass; }

	/** Returns the user widget object displayed by this component */
	UFUNCTION(BlueprintCallable, Category = UserInterface, meta = (UnsafeDuringActorConstruction = true))
		UUserWidget* GetUserWidgetObject() const;

	/** Returns the Slate widget that was assigned to this component, if any */
	const TSharedPtr<SWidget>& GetSlateWidget() const;

	/** Returns the list of widgets with their geometry and the cursor position transformed into this Widget component's space. */
	TArray<FWidgetAndPointer> GetHitWidgetPath(FVector WorldHitLocation, bool bIgnoreEnabledStatus, float CursorRadius = 0.0f);

	/** Returns the list of widgets with their geometry and the cursor position transformed into this Widget space. The widget space is expressed as a Vector2D. */
	TArray<FWidgetAndPointer> GetHitWidgetPath(FVector2D WidgetSpaceHitCoordinate, bool bIgnoreEnabledStatus, float CursorRadius = 0.0f);

	/** Returns the render target to which the user widget is rendered */
	UFUNCTION(BlueprintCallable, Category = UserInterface)
		UTextureRenderTarget2D* GetRenderTarget() const;

	/** Returns the dynamic material instance used to render the user widget */
	UFUNCTION(BlueprintCallable, Category = UserInterface)
		UMaterialInstanceDynamic* GetMaterialInstance() const;

	/** Returns the window containing the user widget content */
	TSharedPtr<SWindow> GetSlateWindow() const;

	/**
	 *  Gets the widget that is used by this Widget Component. It will be null if a Slate Widget was set using SetSlateWidget function.
	 */
	UFUNCTION(BlueprintCallable, Category = UserInterface)
		virtual UUserWidget* GetWidget() const;

	/**
	 *  Sets the widget to use directly. This function will keep track of the widget till the next time it's called
	 *	with either a newer widget or a nullptr
	 */
	UFUNCTION(BlueprintCallable, Category = UserInterface)
		virtual void SetWidget(UUserWidget* Widget);

	/**
	 *  Sets a Slate widget to be rendered.  You can use this to draw native Slate widgets using a WidgetComponent, instead
	 *  of drawing user widgets.
	 */
	virtual void SetSlateWidget(const TSharedPtr<SWidget>& InSlateWidget);

	/**
	 * Sets the local player that owns this widget component.  Setting the owning player controls
	 * which player's viewport the widget appears on in a split screen scenario.  Additionally it
	 * forwards the owning player to the actual UserWidget that is spawned.
	 */
	UFUNCTION(BlueprintCallable, Category = UserInterface)
		void SetOwnerPlayer(ULocalPlayer* LocalPlayer);

	/** @see bManuallyRedraw */
	UFUNCTION(BlueprintCallable, Category = UserInterface)
		bool GetManuallyRedraw() const {
		return bManuallyRedraw;
	};

	/** @see bManuallyRedraw */
	UFUNCTION(BlueprintCallable, Category = UserInterface)
		void SetManuallyRedraw(bool bUseManualRedraw);

	/** Gets the local player that owns this widget component. */
	UFUNCTION(BlueprintCallable, Category = UserInterface)
		ULocalPlayer* GetOwnerPlayer() const;

	/** Returns the "specified" draw size of the quad in the world */
	UFUNCTION(BlueprintCallable, Category = UserInterface)
		FVector2D GetDrawSize() const;

	/** Returns the "actual" draw size of the quad in the world */
	UFUNCTION(BlueprintCallable, Category = UserInterface)
		FVector2D GetCurrentDrawSize() const;

	/** Sets the draw size of the quad in the world */
	UFUNCTION(BlueprintCallable, Category = UserInterface)
		void SetDrawSize(FVector2D Size);

	/** Requests that the widget be redrawn.  */
	UFUNCTION(BlueprintCallable, Category = UserInterface, meta = (DeprecatedFunction, DeprecationMessage = "Use RequestRenderUpdate instead"))
		virtual void RequestRedraw();

	/** Requests that the widget have it's render target updated, if TickMode is disabled, this will force a tick to happen to update the render target. */
	UFUNCTION(BlueprintCallable, Category = UserInterface)
		virtual void RequestRenderUpdate();

	/** Gets the blend mode for the widget. */
	EWidgetBlendMode GetBlendMode() const { return BlendMode; }

	/** Sets the blend mode to use for this widget */
	void SetBlendMode(const EWidgetBlendMode NewBlendMode);

	/** Gets whether the widget is two-sided or not */
	UFUNCTION(BlueprintCallable, Category = Rendering)
		bool GetTwoSided() const {
		return bIsTwoSided;
	};

	/** Sets whether the widget is two-sided or not */
	UFUNCTION(BlueprintCallable, Category = Rendering)
		void SetTwoSided(const bool bWantTwoSided);

	/** Gets whether the widget ticks when offscreen or not */
	UFUNCTION(BlueprintCallable, Category = Animation)
		bool GetTickWhenOffscreen() const {
		return TickWhenOffscreen;
	};

	/** Sets whether the widget ticks when offscreen or not */
	UFUNCTION(BlueprintCallable, Category = Animation)
		void SetTickWhenOffscreen(const bool bWantTickWhenOffscreen) {
		TickWhenOffscreen = bWantTickWhenOffscreen;
	};

	/** Sets the background color and opacityscale for this widget */
	UFUNCTION(BlueprintCallable, Category = UserInterface)
		void SetBackgroundColor(const FLinearColor NewBackgroundColor);

	/** Sets the tint color and opacity scale for this widget */
	UFUNCTION(BlueprintCallable, Category = UserInterface)
		void SetTintColorAndOpacity(const FLinearColor NewTintColorAndOpacity);

	/** Sets how much opacity from the UI widget's texture alpha is used when rendering to the viewport (0.0-1.0) */
	void SetOpacityFromTexture(const float NewOpacityFromTexture);

	/** Returns the pivot point where the UI is rendered about the origin. */
	UFUNCTION(BlueprintCallable, Category = UserInterface)
		FVector2D GetPivot() const { return Pivot; }

	/**  */
	UFUNCTION(BlueprintCallable, Category = UserInterface)
		void SetPivot(const FVector2D& InPivot) { Pivot = InPivot; }

	/**  */
	UFUNCTION(BlueprintCallable, Category = UserInterface)
		bool GetDrawAtDesiredSize() const { return bDrawAtDesiredSize; }

	/**  */
	UFUNCTION(BlueprintCallable, Category = UserInterface)
		void SetDrawAtDesiredSize(bool bInDrawAtDesiredSize) { bDrawAtDesiredSize = bInDrawAtDesiredSize; }

	/**  */
	UFUNCTION(BlueprintCallable, Category = UserInterface)
		float GetRedrawTime() const { return RedrawTime; }

	/**  */
	UFUNCTION(BlueprintCallable, Category = UserInterface)
		void SetRedrawTime(float InRedrawTime) { RedrawTime = InRedrawTime; }

	/** Get the fake window we create for widgets displayed in the world. */
	TSharedPtr< SWindow > GetVirtualWindow() const;

	/** Updates the dynamic parameters on the material instance, without re-creating it */
	void UpdateMaterialInstanceParameters();

	/** Sets the widget class used to generate the widget for this component */
	void SetWidgetClass(TSubclassOf<UUserWidget> InWidgetClass);

	bool GetEditTimeUsable() const { return bEditTimeUsable; }

	void SetEditTimeUsable(bool Value) { bEditTimeUsable = Value; }

	bool GetReceiveHardwareInput() const { return bReceiveHardwareInput; }

	/** Sets shared layer name used when this widget is initialized */
	void SetInitialSharedLayerName(FName NewSharedLayerName) { SharedLayerName = NewSharedLayerName; }

	/** @see bWindowFocusable */
	UFUNCTION(BlueprintCallable, Category = UserInterface)
		bool GetWindowFocusable() const {
		return bWindowFocusable;
	};

	/** @see bWindowFocusable */
	UFUNCTION(BlueprintCallable, Category = UserInterface)
		void SetWindowFocusable(bool bInWindowFocusable);

	/** Gets the visibility of the virtual window created to host the widget focusable. */
	UFUNCTION(BlueprintCallable, Category = UserInterface)
		EWindowVisibility GetWindowVisiblility() const {
		return WindowVisibility;
	}

	/** Sets the visibility of the virtual window created to host the widget focusable. */
	UFUNCTION(BlueprintCallable, Category = UserInterface)
		void SetWindowVisibility(EWindowVisibility InVisibility);

	/** Sets the Tick mode of the Widget Component.*/
	UFUNCTION(BlueprintCallable, Category = UserInterface)
		void SetTickMode(ETickMode InTickMode);

	/** Returns true if the the Slate window is visible and that the widget is also visible, false otherwise. */
	UFUNCTION(BlueprintCallable, Category = UserInterface)
		bool IsWidgetVisible() const;

	/** Hook to allow this component modify the local position of the widget after it has been projected from world space to screen space. */
	virtual FVector2D ModifyProjectedLocalPosition(const FGeometry& ViewportGeometry, const FVector2D& LocalPosition) { return LocalPosition; }

protected:
	void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld);

	void RegisterHitTesterWithViewport(TSharedPtr<SViewport> ViewportWidget);
	void UnregisterHitTesterWithViewport(TSharedPtr<SViewport> ViewportWidget);

	void RegisterWindow();
	void UnregisterWindow();

	/** Allows subclasses to control if the widget should be drawn.  Called right before we draw the widget. */
	virtual bool ShouldDrawWidget() const;

	/** Draws the current widget to the render target if possible. */
	virtual void DrawWidgetToRenderTarget(float DeltaTime);

	/** Returns the width of the widget component taking GeometryMode into account. */
	float ComputeComponentWidth() const;

	void UpdateMaterialInstance();

protected:

	/** How this widget should deal with timing, pausing, etc. */
	UPROPERTY(EditAnywhere, Category = UserInterface)
		EWidgetTimingPolicy TimingPolicy;

	/** The class of User Widget to create and display an instance of */
	UPROPERTY(EditAnywhere, Category = UserInterface)
		TSubclassOf<UUserWidget> WidgetClass;

	/** The size of the displayed quad. */
	UPROPERTY(EditAnywhere, Category = UserInterface)
		FIntPoint DrawSize;

	/** Should we wait to be told to redraw to actually draw? */
	UPROPERTY(EditAnywhere, Category = UserInterface)
		bool bManuallyRedraw;

	/** Has anyone requested we redraw? */
	UPROPERTY()
		bool bRedrawRequested;

	/**
	 * The time in between draws, if 0 - we would redraw every frame.  If 1, we would redraw every second.
	 * This will work with bManuallyRedraw as well.  So you can say, manually redraw, but only redraw at this
	 * maximum rate.
	 */
	UPROPERTY(EditAnywhere, Category = UserInterface)
		float RedrawTime;

	/** What was the last time we rendered the widget? */
	double LastWidgetRenderTime;

	/** Returns current absolute time, respecting TimingPolicy. */
	double GetCurrentTime() const;

	/**
	 * The actual draw size, this changes based on DrawSize - or the desired size of the widget if
	 * bDrawAtDesiredSize is true.
	 */
	UPROPERTY()
		FIntPoint CurrentDrawSize;

	/**
	 * Causes the render target to automatically match the desired size.
	 *
	 * WARNING: If you change this every frame, it will be very expensive.  If you need
	 *    that effect, you should keep the outer widget's sized locked and dynamically
	 *    scale or resize some inner widget.
	 */
	UPROPERTY(EditAnywhere, Category = UserInterface)
		bool bDrawAtDesiredSize;

	/** The Alignment/Pivot point that the widget is placed at relative to the position. */
	UPROPERTY(EditAnywhere, Category = UserInterface)
		FVector2D Pivot;

	/**
	 * Register with the viewport for hardware input from the true mouse and keyboard.  These widgets
	 * will more or less react like regular 2D widgets in the viewport, e.g. they can and will steal focus
	 * from the viewport.
	 *
	 * WARNING: If you are making a VR game, definitely do not change this to true.  This option should ONLY be used
	 * if you're making what would otherwise be a normal menu for a game, just in 3D.  If you also need the game to
	 * remain responsive and for the player to be able to interact with UI and move around the world (such as a keypad on a door),
	 * use the WidgetInteractionComponent instead.
	 */
	UPROPERTY(EditAnywhere, Category = Interaction)
		bool bReceiveHardwareInput;

	/** Is the virtual window created to host the widget focusable? */
	UPROPERTY(EditAnywhere, Category = Interaction)
		bool bWindowFocusable;

	/** The visibility of the virtual window created to host the widget */
	UPROPERTY(EditAnywhere, Category = Interaction)
		EWindowVisibility WindowVisibility;

	/**
	 * Widget components that appear in the world will be gamma corrected by the 3D renderer.
	 * In some cases, widget components are blitted directly into the backbuffer, in which case gamma correction should be enabled.
	 */
	UPROPERTY(EditAnywhere, Category = UserInterface, AdvancedDisplay)
		bool bApplyGammaCorrection;

	/**
	 * The owner player for a widget component, if this widget is drawn on the screen, this controls
	 * what player's screen it appears on for split screen, if not set, users player 0.
	 */
	UPROPERTY()
		ULocalPlayer* OwnerPlayer;

	/** The background color of the component */
	UPROPERTY(EditAnywhere, Category = Rendering)
		FLinearColor BackgroundColor;

	/** Tint color and opacity for this component */
	UPROPERTY(EditAnywhere, Category = Rendering)
		FLinearColor TintColorAndOpacity;

	/** Sets the amount of opacity from the widget's UI texture to use when rendering the translucent or masked UI to the viewport (0.0-1.0) */
	UPROPERTY(EditAnywhere, Category = Rendering, meta = (ClampMin = 0.0f, ClampMax = 1.0f))
		float OpacityFromTexture;

	/** The blend mode for the widget. */
	UPROPERTY(EditAnywhere, Category = Rendering)
		EWidgetBlendMode BlendMode;

	/** Is the component visible from behind? */
	UPROPERTY(EditAnywhere, Category = Rendering)
		bool bIsTwoSided;

	/** Should the component tick the widget when it's off screen? */
	UPROPERTY(EditAnywhere, Category = Animation)
		bool TickWhenOffscreen;

	/** The body setup of the displayed quad */
	UPROPERTY(Transient, DuplicateTransient)
		class UBodySetup* BodySetup;

	/** The material instance for translucent widget components */
	UPROPERTY()
		UMaterialInterface* TranslucentMaterial;

	/** The material instance for translucent, one-sided widget components */
	UPROPERTY()
		UMaterialInterface* TranslucentMaterial_OneSided;

	/** The material instance for opaque widget components */
	UPROPERTY()
		UMaterialInterface* OpaqueMaterial;

	/** The material instance for opaque, one-sided widget components */
	UPROPERTY()
		UMaterialInterface* OpaqueMaterial_OneSided;

	/** The material instance for masked widget components. */
	UPROPERTY()
		UMaterialInterface* MaskedMaterial;

	/** The material instance for masked, one-sided widget components. */
	UPROPERTY()
		UMaterialInterface* MaskedMaterial_OneSided;

	/** The target to which the user widget is rendered */
	UPROPERTY(Transient, DuplicateTransient)
		UTextureRenderTarget2D* RenderTarget;

	/** The dynamic instance of the material that the render target is attached to */
	UPROPERTY(Transient, DuplicateTransient)
		UMaterialInstanceDynamic* MaterialInstance;

	/**
	 * Allows the widget component to be used at editor time.  For use in the VR-Editor.
	 */
	UPROPERTY()
		bool bEditTimeUsable;

protected:

	/** Layer Name the widget will live on */
	UPROPERTY(EditDefaultsOnly, Category = Layers)
		FName SharedLayerName;

	UPROPERTY(EditAnywhere, Category = UserInterface)
		ETickMode TickMode;

	/** The slate window that contains the user widget content */
	TSharedPtr<class SVirtualWindow> SlateWindow;

	/** The relative location of the last hit on this component */
	FVector2D LastLocalHitLocation;

	/** The hit tester to use for this component */
	static TSharedPtr<class FWidget3DHitTester> WidgetHitTester;

	/** Helper class for drawing widgets to a render target. */
	class FWidgetRenderer* WidgetRenderer;

private:

	/** The User Widget object displayed and managed by this component */
	UPROPERTY(Transient, DuplicateTransient)
		UUserWidget* Widget;

	/** The Slate widget to be displayed by this component.  Only one of either Widget or SlateWidget can be used */
	TSharedPtr<SWidget> SlateWidget;

	/** The slate widget currently being drawn. */
	TWeakPtr<SWidget> CurrentSlateWidget;

	static EVisibility ConvertWindowVisibilityToVisibility(EWindowVisibility visibility);

	void OnWidgetVisibilityChanged(ESlateVisibility InVisibility);
	/** Set to true after a draw of an empty component.*/
	bool bRenderCleared;
	bool bOnWidgetVisibilityChangedRegistered;
};