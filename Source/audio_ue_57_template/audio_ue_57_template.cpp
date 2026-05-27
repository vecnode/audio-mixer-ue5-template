// Copyright vecnode. All Rights Reserved.

// Based on Unreal Engine generated project scaffolding.

#include "audio_ue_57_template.h"
#include "AudioIO.h"
#include "AudioPanelWidget.h"
#include "Components/AudioComponent.h"
#include "Engine/Engine.h"
#include "Engine/EngineBaseTypes.h"
#include "Engine/GameViewportClient.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Misc/CoreDelegates.h"
#include "Modules/ModuleManager.h"
#include "Sound/SoundBase.h"
#include "UObject/SoftObjectPath.h"
#include "Widgets/SOverlay.h"

class FAudioUE57TemplateModule : public FDefaultGameModuleImpl
{
public:
	virtual void StartupModule() override
	{
		// Register module hooks after engine initialization so viewport/UI dependencies are valid.
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
		// Always unbind delegates first to avoid callbacks into an unloading module.
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
	static constexpr int32 NumMixerTracks = 5;
	static constexpr int32 NumConfiguredMetaSoundTracks = 2;
	static constexpr float DefaultTrackVolume = 0.8f;

	bool bHasAttemptedInitialShow = false;
	int32 RemainingInputFixFrames = 0;
	TWeakObjectPtr<UWorld> InputFixWorld;
	TSharedPtr<SWidget> RootViewportWidget;
	TSharedPtr<SAudioPanelWidget> AudioPanelWidget;
	TArray<TSoftObjectPtr<USoundBase>> TrackSoundAssets;
	TArray<TWeakObjectPtr<UAudioComponent>> TrackAudioComponents;
	TArray<float> TrackVolumeMultipliers;

	void InitializeTrackVolumeMultipliers()
	{
		if (TrackVolumeMultipliers.Num() == NumMixerTracks)
		{
			return;
		}

		TrackVolumeMultipliers.Init(DefaultTrackVolume, NumMixerTracks);
	}

	void InitializeTrackSoundAssets()
	{
		if (TrackSoundAssets.Num() == NumMixerTracks)
		{
			return;
		}

		TrackSoundAssets.Reset();
		TrackSoundAssets.SetNum(NumMixerTracks);
		TrackSoundAssets[0] = TSoftObjectPtr<USoundBase>(FSoftObjectPath(TEXT("/Game/METASOUND1.METASOUND1")));
		TrackSoundAssets[1] = TSoftObjectPtr<USoundBase>(FSoftObjectPath(TEXT("/Game/METASOUND2.METASOUND2")));
	}

	void ReleaseTrackAudioComponents()
	{
		for (const TWeakObjectPtr<UAudioComponent>& TrackAudioComponent : TrackAudioComponents)
		{
			if (UAudioComponent* AudioComponent = TrackAudioComponent.Get())
			{
				AudioComponent->Stop();
				AudioComponent->DestroyComponent();
			}
		}

		TrackAudioComponents.Reset();
	}

	void EnsureTrackAudioComponents(UWorld* InWorld)
	{
		if (!InWorld)
		{
			return;
		}

		InitializeTrackSoundAssets();
		InitializeTrackVolumeMultipliers();

		if (TrackAudioComponents.Num() != NumMixerTracks)
		{
			TrackAudioComponents.SetNum(NumMixerTracks);
		}

		for (int32 TrackIndex = 0; TrackIndex < NumConfiguredMetaSoundTracks; ++TrackIndex)
		{
			if (!TrackSoundAssets.IsValidIndex(TrackIndex) || TrackSoundAssets[TrackIndex].IsNull())
			{
				continue;
			}

			USoundBase* TrackSound = TrackSoundAssets[TrackIndex].Get();
			if (!TrackSound)
			{
				TrackSound = TrackSoundAssets[TrackIndex].LoadSynchronous();
			}

			if (!TrackSound)
			{
				continue;
			}

			UAudioComponent* AudioComponent = TrackAudioComponents[TrackIndex].Get();
			if (!AudioComponent)
			{
				AudioComponent = NewObject<UAudioComponent>(InWorld);
				if (!AudioComponent)
				{
					continue;
				}

				AudioComponent->bAutoActivate = false;
				AudioComponent->RegisterComponentWithWorld(InWorld);
				TrackAudioComponents[TrackIndex] = AudioComponent;
			}

			AudioComponent->SetSound(TrackSound);
			if (TrackVolumeMultipliers.IsValidIndex(TrackIndex))
			{
				AudioComponent->SetVolumeMultiplier(TrackVolumeMultipliers[TrackIndex]);
			}
		}
	}

	void HandleTrackVolumeChanged(int32 TrackIndex, float NewVolume)
	{
		if (TrackIndex < 0 || TrackIndex >= NumConfiguredMetaSoundTracks)
		{
			return;
		}

		InitializeTrackVolumeMultipliers();
		if (!TrackVolumeMultipliers.IsValidIndex(TrackIndex))
		{
			return;
		}

		const float ClampedVolume = FMath::Clamp(NewVolume, 0.0f, 1.0f);
		TrackVolumeMultipliers[TrackIndex] = ClampedVolume;

		if (TrackAudioComponents.IsValidIndex(TrackIndex))
		{
			if (UAudioComponent* AudioComponent = TrackAudioComponents[TrackIndex].Get())
			{
				AudioComponent->SetVolumeMultiplier(ClampedVolume);
			}
		}
	}

	void HandlePlayTrackRequested(int32 TrackIndex)
	{
		if (TrackIndex < 0 || TrackIndex >= NumConfiguredMetaSoundTracks)
		{
			return;
		}

		UWorld* TargetWorld = InputFixWorld.Get();
		if (!TargetWorld && GEngine && GEngine->GameViewport)
		{
			TargetWorld = GEngine->GameViewport->GetWorld();
		}

		if (!TargetWorld)
		{
			return;
		}

		EnsureTrackAudioComponents(TargetWorld);

		if (!TrackAudioComponents.IsValidIndex(TrackIndex))
		{
			return;
		}

		// Keep playback exclusive: one track's Play button should never trigger simultaneous output on another.
		for (int32 OtherTrackIndex = 0; OtherTrackIndex < TrackAudioComponents.Num(); ++OtherTrackIndex)
		{
			if (OtherTrackIndex == TrackIndex)
			{
				continue;
			}

			if (UAudioComponent* OtherAudioComponent = TrackAudioComponents[OtherTrackIndex].Get())
			{
				OtherAudioComponent->Stop();
			}
		}

		if (UAudioComponent* AudioComponent = TrackAudioComponents[TrackIndex].Get())
		{
			if (AudioComponent->Sound)
			{
				if (TrackVolumeMultipliers.IsValidIndex(TrackIndex))
				{
					AudioComponent->SetVolumeMultiplier(TrackVolumeMultipliers[TrackIndex]);
				}

				AudioComponent->Play(0.0f);
			}
		}
	}

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
			// Game+UI keeps world input available while allowing Slate widget interaction.
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
			ReleaseTrackAudioComponents();
		}

		SAssignNew(AudioPanelWidget, SAudioPanelWidget)
			.InitialReport(ReportText)
			.OnPlayTrackRequested(FOnPlayTrackRequested::CreateRaw(this, &FAudioUE57TemplateModule::HandlePlayTrackRequested))
			.OnTrackVolumeChanged(FOnTrackVolumeChanged::CreateRaw(this, &FAudioUE57TemplateModule::HandleTrackVolumeChanged));

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
		EnsureTrackAudioComponents(TargetWorld);
		ConfigurePanelInput(TargetWorld, true);

		// Reapply input mode for a short period to survive startup races with other systems.
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
		// TickType/DeltaSeconds are not currently used but kept for delegate signature parity.
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
		ReleaseTrackAudioComponents();
	}
};

IMPLEMENT_PRIMARY_GAME_MODULE(FAudioUE57TemplateModule, audio_ue_57_template, "audio_ue_57_template");
