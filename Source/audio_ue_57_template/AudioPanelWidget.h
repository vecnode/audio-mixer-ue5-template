// Copyright vecnode. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class STextBlock;

class SAudioPanelWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SAudioPanelWidget)
		: _InitialReport(TEXT(""))
	{}
		SLATE_ARGUMENT(FString, InitialReport)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	void SetReportText(const FString& InReportText);

private:
	TSharedPtr<STextBlock> ReportTextBlock;
};
