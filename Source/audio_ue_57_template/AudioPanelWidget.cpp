// Copyright vecnode. All Rights Reserved.

#include "AudioPanelWidget.h"

#include "Styling/CoreStyle.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SSlider.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Text/STextBlock.h"

void SAudioPanelWidget::Construct(const FArguments& InArgs)
{
	constexpr int32 NumMixerTracks = 5;
	constexpr float DefaultSliderValue = 0.8f;

	TrackSliderValues.Init(DefaultSliderValue, NumMixerTracks);
	TrackSliderStyles.Init(FCoreStyle::Get().GetWidgetStyle<FSliderStyle>("Slider"), NumMixerTracks);

	TSharedRef<SHorizontalBox> MixerTrackRow = SNew(SHorizontalBox);

	for (int32 TrackIndex = 0; TrackIndex < NumMixerTracks; ++TrackIndex)
	{
		FSliderStyle& TrackSliderStyle = TrackSliderStyles[TrackIndex];
		const FSlateBrush* WhiteBrush = FCoreStyle::Get().GetBrush("WhiteBrush");

		TrackSliderStyle.NormalThumbImage = *WhiteBrush;
		TrackSliderStyle.HoveredThumbImage = *WhiteBrush;
		TrackSliderStyle.DisabledThumbImage = *WhiteBrush;
		TrackSliderStyle.NormalThumbImage.ImageSize = FVector2D(18.0f, 18.0f);
		TrackSliderStyle.HoveredThumbImage.ImageSize = FVector2D(18.0f, 18.0f);
		TrackSliderStyle.DisabledThumbImage.ImageSize = FVector2D(18.0f, 18.0f);
		TrackSliderStyle.NormalThumbImage.TintColor = FSlateColor(FLinearColor(0.80f, 0.82f, 0.85f, 1.0f));
		TrackSliderStyle.HoveredThumbImage.TintColor = FSlateColor(FLinearColor(0.90f, 0.92f, 0.95f, 1.0f));
		TrackSliderStyle.DisabledThumbImage.TintColor = FSlateColor(FLinearColor(0.60f, 0.62f, 0.65f, 1.0f));
		TrackSliderStyle.BarThickness = 18.0f;

		TrackSliderStyle.NormalBarImage = *WhiteBrush;
		TrackSliderStyle.HoveredBarImage = *WhiteBrush;
		TrackSliderStyle.DisabledBarImage = *WhiteBrush;
		TrackSliderStyle.NormalBarImage.TintColor = FSlateColor(FLinearColor(0.22f, 0.23f, 0.25f, 1.0f));
		TrackSliderStyle.HoveredBarImage.TintColor = FSlateColor(FLinearColor(0.27f, 0.28f, 0.30f, 1.0f));
		TrackSliderStyle.DisabledBarImage.TintColor = FSlateColor(FLinearColor(0.15f, 0.16f, 0.18f, 1.0f));
		MixerTrackRow->AddSlot()
		.FillWidth(1.0f)
		.Padding(4.0f, 0.0f)
		[
			SNew(SBorder)
			.Padding(FMargin(1.0f))
			.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
			.BorderBackgroundColor(FLinearColor(0.60f, 0.62f, 0.66f, 1.0f))
			[
				SNew(SBorder)
				.Padding(FMargin(10.0f))
				.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
				.BorderBackgroundColor(FLinearColor(0.93f, 0.94f, 0.96f, 1.0f))
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.AutoHeight()
					.HAlign(HAlign_Center)
					.Padding(0.0f, 0.0f, 0.0f, 8.0f)
					[
						SNew(STextBlock)
						.Text(FText::FromString(FString::Printf(TEXT("Track %d"), TrackIndex + 1)))
						.ColorAndOpacity(FLinearColor(0.16f, 0.18f, 0.21f, 1.0f))
						.Font(FCoreStyle::GetDefaultFontStyle("Regular", 12))
					]
					+ SVerticalBox::Slot()
					.FillHeight(1.0f)
					.HAlign(HAlign_Center)
					.Padding(0.0f, 0.0f, 0.0f, 8.0f)
					[
						SNew(SBorder)
						.Padding(FMargin(1.0f))
						.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
						.BorderBackgroundColor(FLinearColor(0.56f, 0.58f, 0.62f, 1.0f))
						[
							SNew(SBorder)
							.Padding(FMargin(0.0f))
						.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
						.BorderBackgroundColor(FLinearColor(0.10f, 0.11f, 0.12f, 1.0f))
						[
							SNew(SBox)
							.WidthOverride(18.0f)
							.HeightOverride(170.0f)
							.HAlign(HAlign_Fill)
							.VAlign(VAlign_Fill)
							[
								SNew(SSlider)
								.Style(&TrackSliderStyle)
								.IndentHandle(false)
								.MouseUsesStep(false)
								.Orientation(Orient_Vertical)
								.Value_Lambda([this, TrackIndex]()
								{
									return TrackSliderValues.IsValidIndex(TrackIndex) ? TrackSliderValues[TrackIndex] : 0.0f;
								})
								.OnValueChanged_Lambda([this, TrackIndex](float NewValue)
								{
									if (TrackSliderValues.IsValidIndex(TrackIndex))
									{
										TrackSliderValues[TrackIndex] = FMath::Clamp(NewValue, 0.0f, 1.0f);
									}
								})
							]
						]
						]
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					.HAlign(HAlign_Center)
					.Padding(0.0f, 0.0f, 0.0f, 10.0f)
					[
						SNew(STextBlock)
						.Text_Lambda([this, TrackIndex]()
						{
							const float Value = TrackSliderValues.IsValidIndex(TrackIndex) ? TrackSliderValues[TrackIndex] : 0.0f;
							return FText::FromString(FString::Printf(TEXT("%.2f"), Value));
						})
						.ColorAndOpacity(FLinearColor(0.16f, 0.18f, 0.21f, 1.0f))
						.Font(FCoreStyle::GetDefaultFontStyle("Regular", 11))
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0.0f, 0.0f, 0.0f, 4.0f)
					[
						SNew(SButton)
						.Text(FText::FromString(TEXT("Play")))
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0.0f, 0.0f, 0.0f, 4.0f)
					[
						SNew(SButton)
						.Text(FText::FromString(TEXT("Mute")))
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0.0f, 0.0f, 0.0f, 4.0f)
					[
						SNew(SButton)
						.Text(FText::FromString(TEXT("Solo")))
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0.0f, 0.0f, 0.0f, 4.0f)
					[
						SNew(SButton)
						.Text(FText::FromString(TEXT("FX")))
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SButton)
						.Text(FText::FromString(TEXT("Rec")))
					]
				]
			]
		];
	}

	ChildSlot
	[
		SNew(SBorder)
		.Padding(FMargin(18.0f))
		.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
		.BorderBackgroundColor(FLinearColor(0.90f, 0.91f, 0.93f, 1.0f))
		[
			SNew(SBorder)
			.Padding(FMargin(20.0f))
			.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
			.BorderBackgroundColor(FLinearColor(0.96f, 0.96f, 0.97f, 1.0f))
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0.0f, 0.0f, 0.0f, 12.0f)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("UE5 Audio Mixer Template")))
					.ColorAndOpacity(FLinearColor(0.09f, 0.11f, 0.14f, 1.0f))
					.Font(FCoreStyle::GetDefaultFontStyle("Bold", 24))
				]
				+ SVerticalBox::Slot()
				.FillHeight(1.0f)
				.Padding(0.0f, 0.0f, 0.0f, 12.0f)
				[
					SNew(SBorder)
					.Padding(FMargin(12.0f))
					.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
					.BorderBackgroundColor(FLinearColor(0.90f, 0.92f, 0.95f, 1.0f))
					[
						SNew(SScrollBox)
						+ SScrollBox::Slot()
						[
							SAssignNew(ReportTextBlock, STextBlock)
							.Text(FText::FromString(InArgs._InitialReport))
							.ColorAndOpacity(FLinearColor(0.16f, 0.18f, 0.21f, 1.0f))
							.Font(FCoreStyle::GetDefaultFontStyle("Regular", 16))
						]
					]
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SBorder)
					.Padding(FMargin(12.0f))
					.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
					.BorderBackgroundColor(FLinearColor(0.89f, 0.90f, 0.92f, 1.0f))
					[
						SNew(SBox)
						.HeightOverride(320.0f)
						[
							MixerTrackRow
						]
					]
				]
			]
		]
	];
}

void SAudioPanelWidget::SetReportText(const FString& InReportText)
{
	if (ReportTextBlock.IsValid())
	{
		ReportTextBlock->SetText(FText::FromString(InReportText));
	}
}
