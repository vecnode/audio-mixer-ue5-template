// Copyright vecnode. All Rights Reserved.

// Based on Unreal Engine generated project scaffolding.

#include "audio_ue_57_template.h"
#include "AudioIO.h"
#include "AudioPanelWidget.h"
#include "Engine/Engine.h"
#include "Engine/EngineBaseTypes.h"
#include "Engine/GameViewportClient.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Misc/CoreDelegates.h"
#include "Modules/ModuleManager.h"
#include "Widgets/SOverlay.h"

class FAudioUE57TemplateModule : public FDefaultGameModuleImpl
{
public:
	virtual void StartupModule() override
	{
		if (GEngine)
		{
			HandlePostEngineInit();
		}
		else
		{
			FCoreDelegates::OnPostEngineInit.AddRaw(this, &FAudioUE57TemplateModule::HandlePostEngineInit);
		}

		FWorldDelegates::OnWorldInitializedActors.AddRaw(this, &FAudioUE57TemplateModule::HandleWorldInitializedActors);
		FWorldDelegates::OnWorldTickStart.AddRaw(this, &FAudioUE57TemplateModule::HandleWorldTickStart);
		FWorldDelegates::OnWorldCleanup.AddRaw(this, &FAudioUE57TemplateModule::HandleWorldCleanup);
	}

	virtual void ShutdownModule() override
	{
		FCoreDelegates::OnPostEngineInit.RemoveAll(this);
		FWorldDelegates::OnWorldInitializedActors.RemoveAll(this);
		FWorldDelegates::OnWorldTickStart.RemoveAll(this);
		FWorldDelegates::OnWorldCleanup.RemoveAll(this);
		if (GEngine && GEngine->GameViewport && RootViewportWidget.IsValid())
		{
			GEngine->GameViewport->RemoveViewportWidgetContent(RootViewportWidget.ToSharedRef());
		}

		RootViewportWidget.Reset();
		AudioPanelWidget.Reset();
	}

private:
	bool bHasAttemptedInitialShow = false;
	int32 RemainingInputFixFrames = 0;
	TWeakObjectPtr<UWorld> InputFixWorld;
	TSharedPtr<SWidget> RootViewportWidget;
	TSharedPtr<SAudioPanelWidget> AudioPanelWidget;

	void ConfigurePanelInput(UWorld* InWorld, bool bApplyInputMode)
	{
		if (!GEngine || !InWorld || !AudioPanelWidget.IsValid())
		{
			return;
		}

		APlayerController* PlayerController = GEngine->GetFirstLocalPlayerController(InWorld);
		if (!PlayerController)
		{
			return;
		}

		PlayerController->bShowMouseCursor = true;
		PlayerController->bEnableClickEvents = true;
		PlayerController->bEnableMouseOverEvents = true;

		if (bApplyInputMode && GEngine->GameViewport)
		{
			GEngine->GameViewport->SetMouseCaptureMode(EMouseCaptureMode::CaptureDuringMouseDown);
			GEngine->GameViewport->SetMouseLockMode(EMouseLockMode::DoNotLock);
		}

		if (bApplyInputMode)
		{
			FInputModeGameAndUI InputMode;
			InputMode.SetWidgetToFocus(AudioPanelWidget);
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			InputMode.SetHideCursorDuringCapture(false);
			PlayerController->SetInputMode(InputMode);
		}
	}

	void ShowAudioIOReportPanel(const FString& ReportText, UWorld* InWorld)
	{
		if (!GEngine || !GEngine->GameViewport)
		{
			return;
		}

		if (RootViewportWidget.IsValid())
		{
			GEngine->GameViewport->RemoveViewportWidgetContent(RootViewportWidget.ToSharedRef());
			RootViewportWidget.Reset();
			AudioPanelWidget.Reset();
		}

		SAssignNew(AudioPanelWidget, SAudioPanelWidget)
			.InitialReport(ReportText);

		RootViewportWidget =
			SNew(SOverlay)
			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				AudioPanelWidget.ToSharedRef()
			];

		GEngine->GameViewport->AddViewportWidgetContent(RootViewportWidget.ToSharedRef(), 0);
		UWorld* TargetWorld = InWorld ? InWorld : GEngine->GameViewport->GetWorld();
		ConfigurePanelInput(TargetWorld, true);

		InputFixWorld = TargetWorld;
		RemainingInputFixFrames = 120;
	}

	void HandlePostEngineInit()
	{
		if (bHasAttemptedInitialShow)
		{
			return;
		}

		bHasAttemptedInitialShow = true;
		ShowAudioIOReportPanel(BuildAudioIOReportText(), GEngine && GEngine->GameViewport ? GEngine->GameViewport->GetWorld() : nullptr);
	}

	void HandleWorldInitializedActors(const FActorsInitializedParams& InParams)
	{
		if (!InParams.World || !InParams.World->IsGameWorld())
		{
			return;
		}

		if (GIsEditor && InParams.World->WorldType != EWorldType::PIE)
		{
			return;
		}

		ShowAudioIOReportPanel(BuildAudioIOReportText(), InParams.World);
	}

	void HandleWorldTickStart(UWorld* InWorld, ELevelTick TickType, float DeltaSeconds)
	{
		if (RemainingInputFixFrames <= 0 || !InWorld || !InputFixWorld.IsValid() || InWorld != InputFixWorld.Get())
		{
			return;
		}

		if (!InWorld->IsGameWorld())
		{
			return;
		}

		if (GIsEditor && InWorld->WorldType != EWorldType::PIE)
		{
			return;
		}

		ConfigurePanelInput(InWorld, true);
		--RemainingInputFixFrames;
	}

	void HandleWorldCleanup(UWorld* InWorld, bool bSessionEnded, bool bCleanupResources)
	{
		if (!InWorld)
		{
			return;
		}

		if (GIsEditor && InWorld->WorldType != EWorldType::PIE)
		{
			return;
		}

		if (GEngine && GEngine->GameViewport && RootViewportWidget.IsValid())
		{
			GEngine->GameViewport->RemoveViewportWidgetContent(RootViewportWidget.ToSharedRef());
		}

		RemainingInputFixFrames = 0;
		InputFixWorld.Reset();
		RootViewportWidget.Reset();
		AudioPanelWidget.Reset();
	}
};

IMPLEMENT_PRIMARY_GAME_MODULE(FAudioUE57TemplateModule, audio_ue_57_template, "audio_ue_57_template");
