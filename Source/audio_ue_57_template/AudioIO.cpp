// Copyright vecnode. All Rights Reserved.

#include "AudioIO.h"

#include "AudioCaptureCore.h"
#include "AudioMixer.h"
#include "AudioMixerDevice.h"
#include "Engine/Engine.h"

DEFINE_LOG_CATEGORY_STATIC(LogAudioUE57Template, Log, All);

static Audio::FMixerDevice* AUDIO_MIXER = nullptr;

static void InitializeAudioMixerGlobal()
{
	// Cache the current main mixer pointer for this query pass.
	if (!GEngine)
	{
		AUDIO_MIXER = nullptr;
		return;
	}

	AUDIO_MIXER = static_cast<Audio::FMixerDevice*>(GEngine->GetMainAudioDeviceRaw());
}

FString BuildAudioIOReportText()
{
	// Build a UI-friendly report string with graceful degradation when optional interfaces are unavailable.
	FString Report = TEXT("Audio I/O Devices\n");

	InitializeAudioMixerGlobal();
	Report += TEXT("\nOUTPUT:\n");
	if (AUDIO_MIXER)
	{
		if (Audio::IAudioMixerPlatformInterface* MixerPlatform = AUDIO_MIXER->GetAudioMixerPlatform())
		{
			if (Audio::IAudioPlatformDeviceInfoCache* DeviceInfoCache = MixerPlatform->GetDeviceInfoCache())
			{
				// Preferred path: query the platform cache for active output devices.
				const TArray<Audio::FAudioPlatformDeviceInfo> OutputDevices = DeviceInfoCache->GetAllActiveOutputDevices();
				for (const Audio::FAudioPlatformDeviceInfo& Device : OutputDevices)
				{
					Report += FString::Printf(
						TEXT("- %s (%s), %d ch, %d Hz%s\n"),
						*Device.Name,
						*Device.DeviceId,
						Device.NumChannels,
						Device.SampleRate,
						Device.bIsSystemDefault ? TEXT(" [default]") : TEXT("")
					);
				}
			}
			else
			{
				// Fallback path for platforms that do not expose a device info cache.
				uint32 NumOutputDevices = 0;
				if (MixerPlatform->GetNumOutputDevices(NumOutputDevices))
				{
					for (uint32 DeviceIndex = 0; DeviceIndex < NumOutputDevices; ++DeviceIndex)
					{
						Audio::FAudioPlatformDeviceInfo DeviceInfo;
						if (MixerPlatform->GetOutputDeviceInfo(DeviceIndex, DeviceInfo))
						{
							Report += FString::Printf(
								TEXT("- %s (%s), %d ch, %d Hz%s\n"),
								*DeviceInfo.Name,
								*DeviceInfo.DeviceId,
								DeviceInfo.NumChannels,
								DeviceInfo.SampleRate,
								DeviceInfo.bIsSystemDefault ? TEXT(" [default]") : TEXT("")
							);
						}
					}
				}
			}
		}
		else
		{
			Report += TEXT("- Mixer platform interface unavailable\n");
		}
	}
	else
	{
		Report += TEXT("- AUDIO_MIXER is null\n");
	}

	Report += TEXT("\nINPUT:\n");
	Audio::FAudioCapture AudioCapture;
	TArray<Audio::FCaptureDeviceInfo> InputDevices;
	const int32 NumInputDevices = AudioCapture.GetCaptureDevicesAvailable(InputDevices);
	if (NumInputDevices <= 0)
	{
		Report += TEXT("- No input devices found\n");
	}
	else
	{
		for (const Audio::FCaptureDeviceInfo& Device : InputDevices)
		{
			Report += FString::Printf(
				TEXT("- %s (%s), %d ch, %d Hz%s\n"),
				*Device.DeviceName,
				*Device.DeviceId,
				Device.InputChannels,
				Device.PreferredSampleRate,
				Device.bSupportsHardwareAEC ? TEXT(" [AEC]") : TEXT("")
			);
		}
	}

	return Report;
}

void PrintAllAudioInputAndOutputDevices()
{
	// Emit the same information to logs for diagnostics and automated capture.
	InitializeAudioMixerGlobal();

	UE_LOG(LogAudioUE57Template, Display, TEXT("==== Audio Output Devices ===="));
	if (AUDIO_MIXER)
	{
		if (Audio::IAudioMixerPlatformInterface* MixerPlatform = AUDIO_MIXER->GetAudioMixerPlatform())
		{
			if (Audio::IAudioPlatformDeviceInfoCache* DeviceInfoCache = MixerPlatform->GetDeviceInfoCache())
			{
				const TArray<Audio::FAudioPlatformDeviceInfo> OutputDevices = DeviceInfoCache->GetAllActiveOutputDevices();
				for (const Audio::FAudioPlatformDeviceInfo& Device : OutputDevices)
				{
					UE_LOG(
						LogAudioUE57Template,
						Display,
						TEXT("OUTPUT: Name=\"%s\" Id=\"%s\" Channels=%d SampleRate=%d SystemDefault=%s"),
						*Device.Name,
						*Device.DeviceId,
						Device.NumChannels,
						Device.SampleRate,
						Device.bIsSystemDefault ? TEXT("true") : TEXT("false")
					);
				}
			}
			else
			{
				uint32 NumOutputDevices = 0;
				if (MixerPlatform->GetNumOutputDevices(NumOutputDevices))
				{
					for (uint32 DeviceIndex = 0; DeviceIndex < NumOutputDevices; ++DeviceIndex)
					{
						Audio::FAudioPlatformDeviceInfo DeviceInfo;
						if (MixerPlatform->GetOutputDeviceInfo(DeviceIndex, DeviceInfo))
						{
							UE_LOG(
								LogAudioUE57Template,
								Display,
								TEXT("OUTPUT: Name=\"%s\" Id=\"%s\" Channels=%d SampleRate=%d SystemDefault=%s"),
								*DeviceInfo.Name,
								*DeviceInfo.DeviceId,
								DeviceInfo.NumChannels,
								DeviceInfo.SampleRate,
								DeviceInfo.bIsSystemDefault ? TEXT("true") : TEXT("false")
							);
						}
					}
				}
			}
		}
		else
		{
			UE_LOG(LogAudioUE57Template, Warning, TEXT("AUDIO_MIXER is set, but no platform interface is available."));
		}
	}
	else
	{
		UE_LOG(LogAudioUE57Template, Warning, TEXT("AUDIO_MIXER is null. Output device enumeration is unavailable."));
	}

	UE_LOG(LogAudioUE57Template, Display, TEXT("==== Audio Input Devices ===="));
	Audio::FAudioCapture AudioCapture;
	TArray<Audio::FCaptureDeviceInfo> InputDevices;
	const int32 NumInputDevices = AudioCapture.GetCaptureDevicesAvailable(InputDevices);

	if (NumInputDevices <= 0)
	{
		UE_LOG(LogAudioUE57Template, Warning, TEXT("No audio input devices were found."));
		return;
	}

	for (const Audio::FCaptureDeviceInfo& Device : InputDevices)
	{
		UE_LOG(
			LogAudioUE57Template,
			Display,
			TEXT("INPUT: Name=\"%s\" Id=\"%s\" Channels=%d PreferredSampleRate=%d HardwareAEC=%s"),
			*Device.DeviceName,
			*Device.DeviceId,
			Device.InputChannels,
			Device.PreferredSampleRate,
			Device.bSupportsHardwareAEC ? TEXT("true") : TEXT("false")
		);
	}
}
