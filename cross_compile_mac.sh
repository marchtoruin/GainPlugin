#!/bin/bash
# Cross-compilation script for building GainPlugin for macOS on a virtual macOS environment

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

# Install LLVM 19+ using Homebrew
echo "Installing/Updating LLVM..."
brew install llvm lld

# Create SDKs directory and download VST3 SDK if needed
mkdir -p ~/SDKs/VST_SDK
if [ ! -d ~/SDKs/VST_SDK/VST3_SDK ]; then
    echo "VST3 SDK not found. Please download it from Steinberg's website and place it in ~/SDKs/VST_SDK/"
    echo "After downloading, run this script again."
    exit 1
fi

# Create CMake toolchain file for cross-compilation
echo "Creating CMake toolchain file..."
cat > apple-cross.cmake << EOF
# apple-cross.cmake
set(CMAKE_SYSTEM_NAME Darwin)
set(CMAKE_OSX_ARCHITECTURES arm64)
set(CMAKE_C_COMPILER $(brew --prefix llvm)/bin/clang)
set(CMAKE_CXX_COMPILER $(brew --prefix llvm)/bin/clang++)
set(CMAKE_OSX_SYSROOT $(xcrun --show-sdk-path))
EOF

# Create build directory
mkdir -p build

# Configure CMake for cross-compilation
echo "Configuring CMake for cross-compilation..."
cmake -B build -G Xcode \
  -DCMAKE_TOOLCHAIN_FILE=apple-cross.cmake \
  -DJUCE_ROOT="../../JUCE" \
  -DVST3_SDK_PATH="$HOME/SDKs/VST_SDK/VST3_SDK"

# Build the plugin
echo "Building plugin..."
cmake --build build --config Release --target GainPlugin_VST3

# Verify the binary format
echo "Verifying binary format..."
file build/GainPlugin_artefacts/Release/VST3/GainPlugin.vst3/Contents/MacOS/GainPlugin

echo "Build completed successfully!"
echo "The VST3 plugin is located at: $(pwd)/build/GainPlugin_artefacts/Release/VST3/GainPlugin.vst3"

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