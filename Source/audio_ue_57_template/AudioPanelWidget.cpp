// Copyright vecnode. All Rights Reserved.

#include "AudioPanelWidget.h"

#include "Styling/CoreStyle.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Text/STextBlock.h"

void SAudioPanelWidget::Construct(const FArguments& InArgs)
{
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
