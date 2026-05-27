# UE5 Audio Mixer Template

## Dependencies

### Host OS (Windows)

- Windows 10/11
- Unreal Engine 5.7
- Visual Studio 2022 with C++ game development workload
- MSVC v143 toolset and Windows 10/11 SDK
- Audio input device (microphone/interface) for capture enumeration
- Audio output device (speakers/headphones/interface) for mixer output enumeration

### UE Module Dependencies

- Core
- CoreUObject
- Engine
- InputCore
- EnhancedInput
- AudioMixer
- AudioMixerCore
- AudioCaptureCore
- Slate
- SlateCore

## Features

- List Audio INPUT
- List Audio OUTPUT

- 5 Audio Tracks 
    - Play
    - Volume Sliders (0-1)

Each Audio Track loads audio DSP blueprints (MetaSounds BP)

- /Content/METASOUND1.uasset
- /Content/METASOUND2.uasset

## Source Features

- FAudioUE57TemplateModule
    - Hooks engine/world lifecycle delegates on startup and cleans them up on shutdown.
    - Injects a custom Slate audio panel into the game viewport after engine/world init.
    - Reapplies Game+UI input mode for startup stability so panel mouse interaction remains reliable.
    - Initializes and manages 5 mixer tracks with per-track volume state.
    - Loads MetaSound assets from /Game/METASOUND1 and /Game/METASOUND2.
    - Creates and owns UAudioComponent instances for configured tracks.
    - Enforces exclusive playback: pressing Play on one track stops the others.
    - Destroys audio components and viewport widgets during world cleanup and module shutdown.

- SAudioPanelWidget
    - Renders a Slate-based mixer UI with an audio I/O report area and 5 track strips.
    - Provides per-track vertical volume sliders with clamped values (0.0-1.0).
    - Emits OnPlayTrackRequested and OnTrackVolumeChanged delegates to module runtime logic.
    - Shows transport-style buttons per track (Play, Mute, Solo, FX, Rec).

- AudioIO utilities
    - BuildAudioIOReportText builds a UI-friendly report for output and input devices.
    - PrintAllAudioInputAndOutputDevices writes detailed device diagnostics to UE logs.
    - Uses preferred output-device cache path with fallback enumeration when cache is unavailable.
    - Enumerates capture devices and reports channel count, sample rate, and hardware AEC support.



## License

Licensed under the ![License: MIT](LICENSE)