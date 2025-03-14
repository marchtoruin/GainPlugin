# GainPlugin

A simple audio plugin built with JUCE that provides gain control, panning, and mono checking functionality.

## Features

- **Gain Control**: Adjust the volume level with a rotary knob
- **Pan Control**: Balance the audio between left and right channels
- **Mono Check**: Toggle between stereo and mono modes for monitoring

## Requirements

- JUCE 8.0.6 or higher
- Visual Studio 2022 (for Windows)
- C++17 compatible compiler

## Building

1. Clone this repository
2. Open the `GainPlugin.jucer` file with Projucer
3. Generate the project files for your IDE
4. Build the project in your IDE

## Installation

Copy the built VST3 plugin to your system's VST3 directory:
- Windows: `C:\Program Files\Common Files\VST3`
- macOS: `/Library/Audio/Plug-Ins/VST3`

## Usage

1. Load the plugin in your DAW
2. Use the gain knob to adjust volume
3. Use the pan slider to balance between left and right channels
4. Toggle the mono button to check your mix in mono

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- Built with [JUCE](https://juce.com/)
- Created as a learning project for audio plugin development 