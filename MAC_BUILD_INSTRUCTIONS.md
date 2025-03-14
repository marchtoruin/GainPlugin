# Building GainPlugin for macOS

This document provides instructions for building the GainPlugin VST3 plugin on macOS.

## Prerequisites

Before you begin, make sure you have the following installed:

1. **Xcode** - Download from the Mac App Store
2. **Xcode Command Line Tools** - Install by running `xcode-select --install` in Terminal
3. **JUCE Framework** - Download from [JUCE.com](https://juce.com/get-juce/)
4. **VST3 SDK** (optional for cross-compilation) - Download from [Steinberg's website](https://developer.steinberg.help/display/VST/VST+3+SDK)

## Option 1: Direct Build on macOS (Recommended)

This is the simplest approach if you're building directly on a Mac:

1. Clone or download this repository
2. Make sure JUCE is installed in a directory adjacent to this project:
   ```
   - Parent directory
     - JUCE
     - GainPlugin (this project)
   ```
3. Run the build script:
   ```bash
   cd GainPlugin
   chmod +x build_mac.sh
   ./build_mac.sh
   ```
4. The script will:
   - Install any missing dependencies
   - Generate the Xcode project
   - Build the plugin
   - Provide instructions for installing the plugin

5. After building, copy the VST3 plugin to:
   ```
   ~/Library/Audio/Plug-Ins/VST3/
   ```

## Option 2: Cross-Compilation (Advanced)

If you're using a virtual macOS environment or need to cross-compile:

1. Set up a macOS environment with Xcode and command line tools
2. Download the VST3 SDK and place it in `~/SDKs/VST_SDK/VST3_SDK`
3. Run the cross-compilation script:
   ```bash
   cd GainPlugin
   chmod +x cross_compile_mac.sh
   ./cross_compile_mac.sh
   ```
4. The script will:
   - Install LLVM and other dependencies
   - Create a CMake toolchain file for cross-compilation
   - Build the plugin targeting arm64 architecture
   - Verify the binary format

5. After building, copy the VST3 plugin to:
   ```
   ~/Library/Audio/Plug-Ins/VST3/
   ```

## VST3 Bundle Structure

The VST3 plugin must have the following structure to work correctly:

```
GainPlugin.vst3/
└── Contents/
    ├── Info.plist
    ├── MacOS/
    │   └── GainPlugin (compiled binary)
    └── Resources/
        └── moduleinfo.json
```

## Troubleshooting

If you encounter issues:

1. **JUCE not found**: Make sure JUCE is installed in the correct location
2. **Build errors**: Check that you have the latest Xcode and Command Line Tools
3. **Plugin not loading**: Verify the bundle structure and that the binary is compatible with your Mac's architecture

## Testing the Plugin

After installing the plugin, you can test it in any VST3-compatible DAW such as:
- Logic Pro
- GarageBand
- Ableton Live
- Reaper

The plugin should appear in the VST3 plugins list in your DAW. 