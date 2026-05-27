// Copyright vecnode. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

// Builds a formatted string with available audio input/output device information.
AUDIO_UE_57_TEMPLATE_API FString BuildAudioIOReportText();
// Writes detailed audio input/output device information to UE logs.
AUDIO_UE_57_TEMPLATE_API void PrintAllAudioInputAndOutputDevices();
