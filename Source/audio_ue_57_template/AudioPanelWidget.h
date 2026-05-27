// Copyright vecnode. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateTypes.h"
#include "Widgets/SCompoundWidget.h"

class STextBlock;
DECLARE_DELEGATE_OneParam(FOnPlayTrackRequested, int32);
DECLARE_DELEGATE_TwoParams(FOnTrackVolumeChanged, int32, float);

// Slate widget that renders the audio report and the mixer-style control strip.
class SAudioPanelWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SAudioPanelWidget)
		: _InitialReport(TEXT(""))
	{}
		// Initial report text shown in the scrollable status area.
		SLATE_ARGUMENT(FString, InitialReport)
		// Fired when a track Play button is pressed.
		SLATE_EVENT(FOnPlayTrackRequested, OnPlayTrackRequested)
		// Fired when a track volume slider changes.
		SLATE_EVENT(FOnTrackVolumeChanged, OnTrackVolumeChanged)
	SLATE_END_ARGS()

	// Builds the widget tree and initializes per-track state.
	void Construct(const FArguments& InArgs);
	// Updates the report text block when new device information is available.
	void SetReportText(const FString& InReportText);

private:
	// Main text block used by SetReportText.
	TSharedPtr<STextBlock> ReportTextBlock;
	// Callback to module code that owns runtime audio playback.
	FOnPlayTrackRequested OnPlayTrackRequested;
	// Callback to module code that owns runtime volume routing.
	FOnTrackVolumeChanged OnTrackVolumeChanged;
	// Backing values for each track slider.
	TArray<float> TrackSliderValues;
	// Per-track style instances (allows independent visual tuning).
	TArray<FSliderStyle> TrackSliderStyles;
};
