# Akira Multi

Akira Multi is a small JUCE audio plugin project. At the moment it is a clean
gain, EQ, and reverb utility with a simple editor, built as a Standalone app,
VST3, and AU.

The project is a starting point for a guitar-focused drive plugin. The current
code establishes the plugin shell, parameter management, state saving, audio
callback, and a basic UI that can be extended into distortion, amp/cab
simulation, and more detailed ambience.

## Current Features

- Mono and stereo audio bus support.
- One `Gain` parameter from `-24.0 dB` to `+24.0 dB`.
- Three-band EQ:
  - `Bass` low shelf at 120 Hz.
  - `Mid` peak filter at 1 kHz.
  - `Treble` high shelf at 5 kHz.
- JUCE reverb with mix, room size, damping, and width controls.
- Rotary controls in the plugin editor.
- Parameter automation and host state recall through
  `juce::AudioProcessorValueTreeState`.
- XML-based preset/session state save and restore.
- Builds as Standalone, VST3, and AU through CMake and JUCE.
- No MIDI input/output.

## How It Works

The plugin is split into two main classes:

- `AkiraMultiAudioProcessor` handles audio processing, parameters, plugin
  metadata, supported layouts, and host state.
- `AkiraMultiAudioProcessorEditor` creates the visible UI and connects the gain
  slider to the processor parameter.

### Audio Processing

`Source/PluginProcessor.cpp` defines APVTS parameters for gain, EQ, and reverb.
During
`processBlock`, the processor:

1. Reads the current gain value in decibels.
2. Converts it to linear gain with `juce::Decibels::decibelsToGain`.
3. Multiplies every sample in every audio channel by that gain amount.
4. Updates and applies the JUCE DSP EQ filters.
5. Updates and applies the JUCE DSP reverb.

There is no saturation, oversampling, cabinet response, or latency compensation
yet. The signal path is currently:

```text
input -> gain -> bass/mid/treble EQ -> reverb -> output
```

### Parameters and State

The `AudioProcessorValueTreeState` stores plugin parameters in a way that works
with DAW automation and session recall. `getStateInformation` serializes the
APVTS state to XML, and `setStateInformation` restores that XML when the host
reloads a project or preset.

### User Interface

`Source/PluginEditor.cpp` creates a compact control panel. It contains:

- A centered title label.
- Rotary sliders for gain, bass, mid, treble, reverb mix, room size, damping,
  and width.
- `SliderAttachment` instances that keep each slider and APVTS parameter in
  sync.

## Building Locally

This project uses CMake. For local development, point CMake at a JUCE checkout:

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DJUCE_DIR=/path/to/JUCE
cmake --build build
```

On this machine, the helper script uses:

```sh
./build.sh
```

If `JUCE_DIR` is not provided, CMake downloads the pinned JUCE release defined by
`JUCE_GIT_TAG` in `CMakeLists.txt`. That is how GitHub Actions builds the
project.

## Continuous Builds

GitHub Actions builds Akira Multi automatically on macOS and Windows for pushes,
pull requests, manual workflow runs, and version tags.

The workflow produces:

- macOS: universal arm64/x86_64 Standalone app, VST3, and AU.
- Windows: Standalone `.exe` and VST3.

Each build uploads a zipped artifact:

- `AkiraMulti-macOS.zip`
- `AkiraMulti-Windows.zip`

To create a public release with downloadable plugin builds, push a version tag:

```sh
git tag v0.0.1
git push origin v0.0.1
```

The release job attaches the macOS and Windows zip files to the GitHub Release.

## License

Akira Multi is released under the MIT License. See `LICENSE`.

## Suggested Next Steps

### 1. Turn Gain Into Saturation

- Add an input drive parameter separate from output level.
- Apply a nonlinear waveshaper such as `std::tanh`, soft clipping, or diode-style
  clipping.
- Add an output trim so louder distortion can be level-matched.
- Consider oversampling before distortion to reduce aliasing.

Possible signal path:

```text
input -> input gain -> oversampling -> waveshaper -> downsampling -> output gain
```

### 2. Refine Tone Controls

- Add optional low cut and high cut filters before or after distortion.
- Add a simple tone knob using a tilt EQ or low-pass filter if the three-band EQ
  feels too detailed for the main view.
- Add parameter smoothing so fast automation does not recalculate filters
  abruptly.

Possible signal path:

```text
input -> pre-filter -> drive -> tone filter -> output
```

### 3. Refine Reverb

- Add a reverb bypass control.
- Add freeze if needed.
- Tune default values for guitar use.
- Consider a higher-quality reverb algorithm if JUCE's built-in reverb becomes
  too limited.

Possible signal path:

```text
input -> drive -> tone -> reverb -> output
```

### 4. Add Amp Simulation

- Model the amp in stages: preamp gain, tone stack, power amp coloration, and
  output level.
- Use separate parameters for drive, bass, mid, treble, presence, and master.
- Start with simple filters and waveshapers before moving to more detailed
  circuit-inspired models.
- Add oversampling around nonlinear amp stages.

Possible signal path:

```text
input -> preamp drive -> tone stack -> power amp saturation -> output
```

### 5. Add Cab Simulation

- Use convolution with cabinet impulse responses for the most realistic first
  version.
- Add `juce::dsp::Convolution` and load an embedded or user-selected IR.
- Provide a cab on/off switch and possibly a few built-in cab choices.
- Place cab simulation after amp simulation and before reverb.

Possible signal path:

```text
input -> drive/amp -> cab IR -> reverb -> output
```

### 6. Improve Plugin Structure

- Move DSP code into separate classes instead of keeping all processing inside
  `processBlock`.
- Add parameter IDs as constants to avoid string duplication.
- Add smoothing for gain and tone parameter changes.
- Add tests or small debug tools for DSP behavior where practical.
- Add a preset system once there are enough parameters to make presets useful.

## Current Status

Akira Multi is currently a working JUCE plugin foundation with clean gain,
three-band EQ, and reverb. It is not yet a drive, amp, or cab effect in the
sonic sense. The next major milestone is to replace the simple linear gain stage
with a real drive circuit or waveshaper, then build the amp and cabinet stages
around it.
