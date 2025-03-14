#!/bin/bash
# Build script for GainPlugin on macOS

# Exit on error
set -e

# Check if Xcode command line tools are installed
if ! xcode-select -p &> /dev/null; then
    echo "Xcode Command Line Tools not found. Installing..."
    xcode-select --install
    echo "Please run this script again after installation completes."
    exit 1
fi

# Check if Homebrew is installed
if ! command -v brew &> /dev/null; then
    echo "Homebrew not found. Installing..."
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
fi

# Check if LLVM is installed
if ! brew list llvm &> /dev/null; then
    echo "Installing LLVM..."
    brew install llvm lld
fi

# Create SDKs directory if it doesn't exist
mkdir -p ~/SDKs/VST_SDK

# Check if JUCE is installed
JUCE_PATH="$(pwd)/../../JUCE"
if [ ! -d "$JUCE_PATH" ]; then
    echo "JUCE not found at $JUCE_PATH"
    echo "Please make sure JUCE is installed at the correct location."
    echo "The expected structure is:"
    echo "- Parent directory"
    echo "  - JUCE"
    echo "  - GainPlugin (this project)"
    exit 1
fi

# Generate Xcode project using Projucer
echo "Generating Xcode project..."
if [ -f "$JUCE_PATH/Projucer.app/Contents/MacOS/Projucer" ]; then
    "$JUCE_PATH/Projucer.app/Contents/MacOS/Projucer" --resave "$(pwd)/GainPlugin/GainPlugin.jucer"
else
    echo "Projucer not found at $JUCE_PATH/Projucer.app"
    echo "Please make sure Projucer is built and available."
    exit 1
fi

# Build the plugin
echo "Building plugin..."
cd "$(pwd)/GainPlugin/Builds/MacOSX"
xcodebuild -configuration Release

echo "Build completed successfully!"
echo "The VST3 plugin is located at: $(pwd)/build/Release/GainPlugin.vst3"

# Instructions for packaging
echo ""
echo "To use the plugin, copy the GainPlugin.vst3 folder to:"
echo "~/Library/Audio/Plug-Ins/VST3/"
echo ""
echo "Note: Make sure the plugin has the correct bundle structure:"
echo "GainPlugin.vst3/"
echo "└── Contents/"
echo "    ├── Info.plist"
echo "    ├── MacOS/"
echo "    │   └── GainPlugin (compiled binary)"
echo "    └── Resources/"
echo "        └── moduleinfo.json" 